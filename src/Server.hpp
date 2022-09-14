#pragma once

#include "utility.hpp"
#include "Request.hpp"
#include "HandleCode.hpp"
#include "cgi.hpp"
#include <netdb.h>
#include <arpa/inet.h>

class Server {
	public:
		typedef std::map<std::string, Route>		routes_t;
		typedef std::map<std::string, HandleCode>	codes_t;
		typedef std::vector<struct kevent>			events_t;
		typedef std::map<uintptr_t, Request>		clients_t;

		/* from config */
		bool			isdefault;
		unsigned int	port;
		string			serverName;
		string			host;
		routes_t		routes;
		struct sockaddr_in	sockaddr;

	
		/* for server operation */
		int				sockfd, kq;
		events_t		chlist;
		struct kevent	evlist[1024];
		clients_t		client;
		codes_t			codes;

		Server() : isdefault(false) {};
		Server(string serverName, string host, unsigned int port, bool isdefault, routes_t & routes, codes_t & codes) : 
			isdefault(isdefault), 
			port(port), 
			serverName(serverName),
			host(host),
			routes(routes), 
			codes(codes)
		{};
		~Server() {};
		
		// Server Launch 

		void init_server() {
			struct hostent *hp;

			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			assert(sockfd != -1);

			sockaddr.sin_family = AF_INET;

			hp = gethostbyname(this->host.c_str());
			assert(hp != NULL);
			sockaddr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *) hp->h_addr_list[0])));
			sockaddr.sin_port = htons(this->port);

			int	option_on = 1;
			assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option_on, sizeof(int)) == 0);
			assert(bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == 0);
			assert(listen(sockfd, 20) == 0);

			kq = kqueue();
			assert(kq != -1);

			chlist.resize(1);
			EV_SET(&*(chlist.end() - 1), sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
		};

		void acceptConnection(struct kevent &ev) {
			while (ev.data--) {
				socklen_t socklen = sizeof(sockaddr);
				int	newConnection = accept(sockfd, (struct sockaddr *) &sockaddr, &socklen);
				assert(newConnection != -1);
				fcntl(newConnection, F_SETFL, O_NONBLOCK);

				chlist.resize(chlist.size() + 2);
				EV_SET(&*(chlist.end() - 2), newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);
				EV_SET(&*(chlist.end() - 1), newConnection, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 6000, NULL); // TIMEOUT 

				client[newConnection] = Request(newConnection);
			}
		};
		
		void	disconnect(struct kevent &ev) {
			client.erase(ev.ident);
			close(ev.ident);
		};

		void	sendData(struct kevent &ev) {
			Request	&req = client[ev.ident];
			string	res = req.res.getStr();	

			/* send */
			size_t bits = send(ev.ident, res.c_str() + req.res.sendBits , res.length(), MSG_DONTWAIT);
			req.res.sendBits += bits;
			/* check if send all data */
			if (req.res.sendBits < res.length())
				return ;
			req.clean();
			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			client.erase(ev.ident);
			close(ev.ident);
		}

		void	findCodeHandler(int code, Request & req) {
			codes_t::iterator it = this->codes.begin();
			for (; it != this->codes.end(); it++) {
				if (it->second.code == code) {
					// req.putCustomError(code);
					// TODO HANDLE REDIRECTION
					return;
				}
			}
			req.putCustomError(code);
		}

		std::string find_extension(std::string path) {
			std::string temp = path;
			while (temp.find(".") != std::string::npos) {
				temp.erase(0, temp.find(".") + 1);
			}
			if (path.length() == temp.length()) {
				return "";
			}
			if (temp.find('/') != std::string::npos) {
				return "";
			}
			return temp;
		}

		std::string get_file_from_path(std::string path, std::string route) {
			std::string ret = path.substr(route.length());
			if ((*ret.rbegin()) == '/') {
				ret.pop_back();
			} 
			return ret;
		}

		std::string get_file_full_path(std::string requested_file, std::string root) {
			char buf [MAXPATHLEN];
			getcwd(buf, MAXPATHLEN);
			if (root.find('.') == 0 && root.find('/') == 1) {
				root.replace(0, 1, buf);
			} else if (root.find('/') != 0) {
				root.insert(0, 1, '/');
				root.insert(0, buf);
			}
			if (root.find('/', root.length() - 1) == std::string::npos && requested_file.find('/') != 0) {
				root.push_back('/');
			}
			return root + requested_file;
		}

		void doResBasedOnReq(Request & req, Route & match) {
			if (req.path.length() == match.path.length()) {
				std::string file = get_file_full_path(match.index, match.root);
				std::string extension = find_extension(match.index);
				if (check_if_file_exists(file)) {
					if (extension == "") {
						
					} else if (extension == match.perlcgiExtension) {
						execute_cgi(req, true);
					} else if (extension == match.pythoncgiExtension) {
						execute_cgi(req, false);
					} else {

					}
				} else if (check_if_file_is_dir(file) && match.autoindex) {
					req.res.putAutoIndex(req.path, match.root);
				} else {
					findCodeHandler(404, req);
				}
			} else {
				std::string extension = find_extension(req.path);
				std::string file = get_file_from_path(req.path, match.path);
				file = get_file_full_path(file, match.root);
				if (extension == "") {
					if (check_if_file_exists(file)) {

					} else if (check_if_file_is_dir(file) && match.autoindex) {
						req.res.putAutoIndex(req.path, match.root);
					} else {
						findCodeHandler(404, req);
					}
				} else if (extension == match.perlcgiExtension) {
					if (check_if_file_exists(file)) {
						execute_cgi(req, true);
					} else if (check_if_file_is_dir(file) && match.autoindex) {
						req.res.putAutoIndex(req.path, match.root);
					} else {
						findCodeHandler(404, req);
					}
				} else if (extension == match.pythoncgiExtension) {
					if (check_if_file_exists(file)) {
						execute_cgi(req, true);
					} else if (check_if_file_is_dir(file) && match.autoindex) {
						req.res.putAutoIndex(req.path, match.root);
					} else {
						findCodeHandler(404, req);
					}
				} else {
					if (check_if_file_exists(file)) {
						
					} else if (check_if_file_is_dir(file) && match.autoindex) {
						req.res.putAutoIndex(req.path, match.root);
					} else {
						findCodeHandler(404, req);
					}
				}
			}
		}

		void	recvData(struct kevent &ev) {
			char	buf[10000];
			int		ret;

			/* recv */
			ret = recv(ev.ident, buf, 9999, 0);
			if (ret <= 0)
				return ;
			buf[ret] = '\0';
			Request &req = client[ev.ident];

			/* check chunked request */
			if (!req.content.expected) {
				int reqCode = req.parseRequest(buf);
				if (reqCode != 200) {
					findCodeHandler(reqCode, req);
					return ;
				}
				req.content.initialize(buf, req.headers);
			} else
				req.content.parseByte(buf);
			
			if (!req.content.isFullyParsed)
			 	return ;

			int reqCode = req.parseRequest(req.content.raw);

			if (reqCode != 200) {
				findCodeHandler(reqCode, req);
				return ;
			} 

			std::vector<Route> matches;
			for (routes_t::iterator it = routes.begin(); it != routes.end(); it++ ) {
				if (req.path.substr(0, it->second.path.length()) == it->second.path) {
					std::vector<Route>::iterator mit = matches.begin();
					for (; mit != matches.end(); mit++ ) {
						if (mit->path == it->second.path) {
							break ;
						}
					}
					if (mit != matches.end())
						continue;
					matches.push_back(it->second);
				}
			}
			std::sort(matches.begin(), matches.end(), sortByComplex);
			Route matchingRoute = *(matches.begin());
			std::vector<std::string> a = matchingRoute.methods;
			if (std::find(a.begin(), a.end(), req.method) == a.end()) {
				findCodeHandler(405, req);
				return;
			} else if (req.method == "GET" || req.method == "POST" || req.method == "DELETE") {
				doResBasedOnReq(req, matchingRoute);
				return;
			} else {
				findCodeHandler(501, req);
				return;
			}

			if (req.method == "POST") {
				req.postContent(req, *this);				
			} else if (req.method == "DELETE") {
				if (!remove(("www" + req.path).c_str())) {
					findCodeHandler(204, req);
					return ;
				} else {
					findCodeHandler(500, req);
					return ;
				}
			} else if (is_CGI(req.path)) {
				if (exist("www" + req.path))
					return;
				else
					findCodeHandler(404, req);
			} else if (is_autoindex_on(req.path, this->routes)) {
				string body = req.res.putAutoIndex(req.path, this->routes["/autoindex"].root);
				if (body == "")
					findCodeHandler(404, req);
				else
					req.putAutoIndexRes(body);
			}
			else {
				req.putResponse(this->routes);
			}
			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		}

		void setTimeOut(struct kevent &ev) {
			Request &req = client[ev.ident];
			
			findCodeHandler(408, req);
			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		}

		void launch() {
			int evNb;

			init_server();
			std::cout << "WEBSERV launched." << std::endl;
			std::cout << this->host << ":" << this->port << std::endl;

			while (1) {
				evNb = kevent(kq, chlist.data(), chlist.size(), evlist, 1024, NULL);
				chlist.clear();
				if (evNb < 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
					return ;
				for (int i = 0; i < evNb; i++) {
					struct kevent &ev = evlist[i];
					if (ev.flags & EV_EOF)
						disconnect(ev);
					else if (static_cast<int>(ev.ident) == sockfd)
						acceptConnection(ev);
					// else if (ev.flags & EV_CLEAR)
					// 	setTimeOut(ev);
					else if (ev.filter & EVFILT_READ && !client[ev.ident].res.ready)
						recvData(ev);
					else if (ev.filter & EVFILT_WRITE)
						sendData(ev);
				}
			}
		}

		static void thread_launch(void *ptr) {
			Server *launch;

			launch = reinterpret_cast<Server *>(ptr);
			launch->launch();
		}

		static void start(std::vector<Server> & servers) {
			int	serverNb = servers.size();
			
			if (serverNb == 1)
				servers[0].launch();
			else {
				std::vector<pthread_t> threads(serverNb);

				for (int i = 0; i < serverNb; i++)
					pthread_create(&threads.at(i), NULL, (void * (*)(void *)) &thread_launch, &servers.at(i));
				for (int i = 0; i < serverNb; i++)
					pthread_detach(threads.at(i));
			}
			while (1);
		}

		class WebservError : public std::exception {
			public:
				const char *what() const throw() {
					return ("Error found");
				}
		};
};


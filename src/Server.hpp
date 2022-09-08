#pragma once

#include "utility.hpp"
#include "Request.hpp"
#include "cgi.hpp"

class Server {
	public:
		typedef std::map<std::string, Route>	routes_t;
		typedef std::vector<struct kevent>		events_t;
		typedef std::map<uintptr_t, Request>	clients_t;

		/* from config */
		unsigned int	port;
		string			serverName;
		string			maxBodySize;
		routes_t		routes;
		struct sockaddr_in	sockaddr;

	
		/* for server operation */
		int				sockfd, kq;
		events_t		chlist;
		struct kevent	evlist[1024];
		struct kevent	tt;
		clients_t		client;

		Server() {};
		~Server() {};
		
		// Server Launch 

		void init_server() {

			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			assert(sockfd != -1);

			sockaddr.sin_family = AF_INET;
			sockaddr.sin_addr.s_addr = inet_addr(this->serverName == "localhost" ? "127.0.0.1" : this->serverName.c_str());
			sockaddr.sin_port = htons(this->port);

			int	option_on = 1;
			assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option_on, sizeof(int)) == 0);
			assert(bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == 0);
			assert(listen(sockfd, 20) == 0);

			kq = kqueue();
			assert(kq != -1);

			chlist.resize(1);
			EV_SET(&*(chlist.end() - 1), sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
		
			// EV_SET(&tt, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			// kevent(kq, &tt, 1, NULL, 0, NULL);
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

				// EV_SET(&tt, newConnection, EVFILT_READ, EV_ADD, 0, 0, NULL);
				// kevent(kq, &tt, 1, NULL, 0, NULL);
				// EV_SET(&tt, newConnection, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 1000, NULL); // TIMEOUT 
				// kevent(kq, &tt, 1, NULL, 0, NULL);

				client[newConnection] = Request(newConnection);
				std::cout << "Connection accepted." << std::endl;
			}
		};
		
		void	disconnect(struct kevent &ev) {
			std::cout << "disconnect" << std::endl;
			client.erase(ev.ident);
			// chlist.resize(chlist.size() + 1);
			// EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
			// EV_SET(&tt, ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			// kevent(kq, &tt, 1, NULL, 0, NULL);
			close(ev.ident);
		};

		void	sendData(struct kevent &ev) {

			// std::cout << "send" << std::endl;
			Request	&req = client[ev.ident];
			string	res = req.res.getStr();	

			/* send */
			size_t bits = send(ev.ident, res.c_str() + req.res.sendBits , res.length(), MSG_DONTWAIT);
			req.res.sendBits += bits;
			// std::cout << bits << std::endl;
			/* check if send all data */
			if (req.res.sendBits < res.length())
				return ;
			req.clean();
			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			client.erase(ev.ident);
			close(ev.ident);
			// EV_SET(&tt, ev.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			// kevent(kq, &tt, 1, NULL, 0, NULL);
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
					req.putCustomError(reqCode);
					return ;
				}
				req.content.initialize(buf, req.headers);
			} else
				req.content.parseByte(buf);
			
			if (!req.content.isFullyParsed)
			 	return ;

			int reqCode = req.parseRequest(req.content.raw);
			if (reqCode != 200) {
				req.putCustomError(reqCode);
			} else if (req.method == "POST") {
				req.postContent(req);				
			} else if (req.method == "DELETE") {
				if (!remove(("www" + req.path).c_str()))
					req.putCustomError(204);
				else
					req.putCustomError(500);
			} else if (is_CGI(req.path)) {
				if (exist("www" + req.path))
					execute_cgi(req);
				else
					req.putCustomError(404);
			} else if (is_autoindex_on(req.path, this->routes)) {
				string body = req.res.putAutoIndex(req.path, this->routes["/autoindex"].root);
				if (body == "")
					req.putCustomError(404);
				else
					req.putAutoIndexRes(body);
			}
			else {
				req.putResponse(this->routes);
			}
			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			// EV_SET(&tt, ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			// kevent(kq, &tt, 1, NULL, 0, NULL);
			//close(ev.ident);
		}

		void setTimeOut(struct kevent &ev) {
			Request &req = client[ev.ident];
			
			req.putCustomError(408);
			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			// EV_SET(&tt, ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			// kevent(kq, &tt, 1, NULL, 0, NULL);
		}

		void launch() {
			int evNb;

			init_server();
			std::cout << "WEBSERV launched." << std::endl;
			std::cout << this->port << std::endl;

			while (1) {
				evNb = kevent(kq, chlist.data(), chlist.size(), evlist, 1024, NULL);
				chlist.clear();
				if (evNb <= 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
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
					std::cout << "end of boucle" << evNb << std::endl;
				}
			}
			std::cout << "what?" << std::endl;
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


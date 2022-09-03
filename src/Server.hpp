#pragma once

#include "utility.hpp"
#include "Request.hpp"

#include <cerrno>

class Server {
	public:
		unsigned int				port;
		std::string					serverName;
		std::string					maxBodySize;
		std::map<std::string, Route> routes;
	
		int							sockfd;
		int							kq;
		std::vector<struct kevent>	chlist;
		std::vector<struct kevent>	evlist;
		std::map<uintptr_t, Request> client;

		Server() {};
		~Server() {
			if (this->kq == -1)
				close(this->kq);
		};
		Server & operator=(const Server & other) {
			this->port = other.port;
			this->serverName = other.serverName;
			this->maxBodySize = other.maxBodySize;
			this->routes = other.routes; 
			this->sockfd = other.sockfd;
			this->kq = other.kq;
			this->chlist = other.chlist;
			this->evlist = other.evlist;
			return (*this);
		};
		
		// Server Launch 

		void init_server() {
			struct sockaddr_in	sockaddr;

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
		};

		void acceptConnection() {
			int	newConnection = accept(sockfd, NULL, NULL);
			assert(newConnection != -1);
			fcntl(newConnection, F_SETFL, O_NONBLOCK);

			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);

			client[newConnection] = Request(newConnection);
			std::cout << "Connection accepted." << std::endl;
		};
		
		void	disconnect(int fd) {
			std::cout << "disconnect" << std::endl;
			close(fd);
		};

		void	sendData(struct kevent &ev) {
			Request &req = client[ev.ident];
			std::string res = req.res.getStr();	

			size_t bits = send(ev.ident, res.c_str() + req.res.sendBits , res.length(), MSG_DONTWAIT);
			req.res.sendBits += bits;
			if (req.res.sendBits < res.length())
				return ;
			req.clean();
			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		}

		void	recvData(struct kevent &ev) {
			char	buf[10000];
			int		ret;

			ret = recv(ev.ident, buf, 9999, 0);
			if (ret < 0)
				throw Server::WebservError();
			if (!ret) {
				/* Minguk a revoir ici */
				return ;
			}
			buf[ret] = '\0';
			Request &req = client[ev.ident];
			if (!req.content.expected) {
				std::cout << "empty" << std::endl;
			 	req.parseRequest(buf);
				req.content.initialize(buf, req.headers);
			} else
				req.content.parseByte(buf);

			/* Minguk Protection invalid request */

			/* Sasso here CGI */
			
			if (!req.content.isFullyParsed)
			 	return ;
			req.parseRequest(req.content.raw);
			req.putResponse(this->routes);
			chlist.resize(chlist.size() + 1);
			EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		}

		void launch() {
			int evNb;

			init_server();
			std::cout << "WEBSERV launched." << std::endl;
			std::cout << this->port << std::endl;

			while (1) {
				evlist.clear();
				evlist.resize(1);
				evNb = kevent(kq, chlist.data(), chlist.size(), evlist.data(), evlist.size(), NULL);
				chlist.clear();
				if (evNb < 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
					return ;
				for (int i = 0; i < evNb; i++) {
					struct kevent &ev = evlist[i];
					if (ev.flags & EV_EOF)
						disconnect(ev.ident);
					else if (static_cast<int>(ev.ident) == sockfd)
						acceptConnection();
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


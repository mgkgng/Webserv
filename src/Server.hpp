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
		struct sockaddr_in			sockaddr;
		int							addrlen;
		std::vector<struct kevent>	chlist;
		std::vector<struct kevent>	evlist;
		std::vector<Request> 		reqs;

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
			this->sockaddr = other.sockaddr;
			this->addrlen = other.addrlen;
			this->chlist = other.chlist;
			this->evlist = other.evlist;
			return (*this);
		};
		
		// Server Launch

		void init_addrinfo() {
			bzero(&sockaddr, sizeof(struct sockaddr_in));
			sockaddr.sin_family = AF_INET;
			sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			sockaddr.sin_port = htons(this->port);
			addrlen = sizeof(sockaddr);
		};

		void init_server() {
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			fcntl(sockfd, F_SETFL, O_NONBLOCK);
			assert(sockfd != -1);

			int	option_on = 1;
			assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option_on, sizeof(int)) == 0);
			assert(bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == 0);
			assert(listen(sockfd, 20) == 0);

			kq = kqueue();
			assert(kq != -1);

			chlist.resize(1);
			EV_SET(chlist.data(), sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
		};

		void acceptConnection() {
			int	newConnection = accept(sockfd, (struct sockaddr *) &sockaddr, (socklen_t *) &addrlen);
			assert(newConnection != -1);
			fcntl(newConnection, F_SETFL, O_NONBLOCK);

			chlist.resize(chlist.size() + 1);
			EV_SET(chlist.end().base() - 1, newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);
			this->req.push_back(Request(newConnection));
			std::cout << "Connection accepted." << std::endl;
		};
		
		void	disconnect(int fd) {
			std::cout << "disconnect" << std::endl;
			close(fd);
		};

		void	sendData(struct kevent &ev) {

			// send response
			std::string res = this->getRequest(ev.ident).res.getStr();	
			std::cout << "response to send" << std::endl;

			ssize_t totalBits = res.length();
			ssize_t readBits = send(ev.ident, res.c_str() /*+ currBits*/, totalBits /*- currBits*/, MSG_DONTWAIT);
			if (readBits == -1) {
				std::cout << "send error" << std::strerror(errno) << std::endl;
				return ;
			} else if (!readBits) {
				chlist.resize(chlist.size() + 1);
				EV_SET(chlist.end().base() - 1, sockfd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			}
			//currBits += readBits;
		}

		void	recvData(struct kevent &ev) {
			char	buf[10000];
			int		ret;

			ret = recv(ev.ident, buf, 9999, 0);
			if (ret < 0)
				throw WebservError();
			buf[ret] = '\0';
			Request *req = this->getRequest(ev.ident);

			if (ret > std::stoi(this->maxBodySize)) { // Request too big
				req->res.putResponse(413);
				chlist.resize(chlist.size() + 1);
				EV_SET(chlist.end().base() - 1, ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				return ;
			}
			// if (!req.method.length()) {
			// 	std::cout << "Invalid HTTP request" << std::endl;
			// 	return ;
			// }
			std::cout << "Data read:" << buf << std::endl;
			chlist.resize(chlist.size() + 1);
			EV_SET(chlist.end().base() - 1, ev.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);


			req->res.putResponse(req->path, req->headers, this->routes);

		}

		Request *getRequest(uintptr_t ev_ident) {
			for (std::vector<Request>::iterator it = this->reqs.begin(); it != reqs.end(); it++)
				if (it->ident == ev_ident)
					return (&(*it));
			return (NULL);
		}

		void launch() {
			int evNb;

			init_addrinfo();
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
					if (evlist.at(i).flags & EV_EOF)
						disconnect(evlist.at(i).ident);
					else if (static_cast<int>(evlist.at(i).ident) == sockfd) {
						std::cout << "accept here" << std::endl;
						acceptConnection();
 					}
					if (evlist.at(i).filter & EVFILT_READ) {
						std::cout << "recv data" << std::endl;
						recvData(evlist[i]);
					}
					if (evlist.at(i).filter & EVFILT_WRITE) {
						std::cout << "send data" << std::endl;
						sendData(evlist[i]);
					}
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


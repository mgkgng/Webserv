#pragma once

#include "utility.hpp"
#include "ReqRes.hpp"
#include "HandleCode.hpp"
#include "cgi.hpp"
#include <netdb.h>
#include <arpa/inet.h>

class Request;

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
		string			maxBodySize;
		string			root;
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

		void init_server();

		void acceptConnection(struct kevent &ev);
		
		void disconnect(struct kevent &ev);

		void sendData(struct kevent &ev);

		void findCodeHandler(int code, Request & req);

		std::string find_extension(std::string path);

		std::string get_file_from_path(std::string path, std::string route);

		std::string get_file_full_path(std::string requested_file, std::string root);

		void doResBasedOnReq(Request & req, Route & match);

		void	recvData(struct kevent &ev);

		void setTimeOut(struct kevent &ev);

		void launch();

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


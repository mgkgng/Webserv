/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/19 14:18:37 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* functions to use : socket, listen, bind, accept

#pragma once

#include "Webserv.hpp"
#include "sys/un.h"
#include <vector>

#define PORT "8080"
#define BACKLOG 20

using namespace std;

class Server {
	private:
		int						sockfd;
		int						kq;
		struct addrinfo 		*info;
		vector<struct kevent>	chlist; // all
		vector<struct kevent>	evlist; // selected 
		string					receivedData;
		struct timespec			timeout;
		
		void	init_info();
		void	init_addrinfo();
		void	init_sockaddr();
		void	init_server();
		void	terminate();

	public:
		Server();
		~Server();

		void	broadcastMsg(string s);
		void	broadcastErr(string s);
		void	acceptConnection();
		void	registerEvents();
		void	sendData(int sockfd, string s);
		void	recvData(int sockfd);
		void	launch(Server *server);
		void	someExampleCode();
		void	acceptConnection();
	
		class SystemCallError : public std::exception {
			private:
				string _msg;
			public:
				SystemCallError(string s) : _msg(s) {}
				~SystemCallError() {}
				
				const char *what() const throw() {
					return (_msg.c_str());
				}
		};
};

// kqueue should be closed at the end.
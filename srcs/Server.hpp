/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/18 21:50:12 by min-kang         ###   ########.fr       */
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
		struct addrinfo 		*info;
		vector<struct kevent>	chlist;
		vector<struct kevent>	evlist;
		
		void	init_addrinfo();
		void	init_sockaddr();
		void	init_server();
		void	terminate();

	public:
		Server();
		~Server();

		void	acceptConnection();
		void	loopConnection();
		void	sendData(int sockfd, string s);
		void	recvData(int sockfd);
		void	launch();
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
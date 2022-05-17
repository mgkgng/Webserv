/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/17 15:53:07 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* functions to use : socket, listen, bind, accept

#pragma once

#include "Webserv.hpp"
#include <vector>

#define PORT "8080"
#define BACKLOG 20

using namespace std;

class Server {
	private:
		int						sockfd;
		struct addrinfo 		*info;
		struct sockaddr			*sockaddr;
		vector<struct kevent>	chlist;
		vector<struct kevent>	evlist;
		
		void	init_addrinfo() {
			struct addrinfo hints;
			int				status;

			bzero(&hints, sizeof(struct addrinfo));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;

			if ((status = getaddrinfo(NULL, PORT, &hints, &info)) != 0) {
				cerr << "getaddrinfo error." << endl;
				exit(EXIT_FAILURE);
			}
		}

		void	init_sockaddr() {
			sockaddr->sa_data = ;
			sockaddr->sa_family = info->ai_family;
		}
 
		void	init_server() {
			if ((sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) < 0) {
				cerr << "socket error." << endl;
				exit(EXIT_FAILURE);
			}
			fcntl(sockfd, F_SETFL, O_NONBLOCK); // Non-blocking mode, but it might be considered as an old way
		
			if (bind(sockfd, info->ai_addr, info->ai_addrlen) < 0) {
				cerr << "bind error." << endl;
				exit(EXIT_FAILURE);
			}

			if (listen(sockfd, BACKLOG) < 0) {
				cerr << "listen error." << endl;
				exit(EXIT_FAILURE);
			}
		}

		void	terminate() {
			freeaddrinfo(info);
		}

	public:
		Server() {
			init_addrinfo();
			init_server();
		}
		~Server() {
			terminate();
		}

		void		acceptConnection() {
			int		newConnection;
			socklen_t	addrlen = sizeof(info->ai_family); // try to check it 
			if ((newConnection = accept(sockfd, (struct sockaddr *) &sockAddr, (socklen_t *) &addrlen)) < 0) {
				cerr << "accept error." << endl;
				exit(EXIT_FAILURE);
			}
			struct kevent	ev;
			EV_SET(&ev, newConnection, EVFILT_READ, EV_ADD, 0, 0, 0);
			chlist.push_back(ev);
		}

		void	multiplexing() {
			int kq;

			if ((kq = kqueue()) < 0) {
				cerr << "kqueue error." << endl;
				exit(EXIT_FAILURE);
			}

			
		}
		
		void	receiveData(int connection) {
			
		}

		void	sendData(int sockfd, string s) {
			send(connection, s.c_str(), s.size(), 0);
		}

		void	someExampleCode() {
			struct socketaddr_in sa;
			struct socketaddr_in6 sa6;

			inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr));
			inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr));
			
			char	ip4[INET_ADDRSTRLEN]; // space to hold the IPv4 string
			struct socketaddr_in sa;
			inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);

			char	ip6[INET6_ADDRSTRLEN];
			struct	sockaddr_in6 sa6;
			inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);

			inet_ntop(info->ai_family, info->ai_addr, info->)
		}
};

//* NOTE
//* In order to use poll, i'll use vector for struct pollfd
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/15 18:20:47 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* functions to use : socket, listen, bind, accept

#pragma once

#include "Webserv.hpp"
#include <vector>

using namespace std;

class Socket {
	private:
		int	sockfd;
		struct sockaddr_in sockAddr; // in represents IP Network address.
		// i can consider here using struct sockaddr_storage instead of sockaddr_in
		// in order to cover both IPv4 and IPv6 address. In that case, i might have to use
		// inet_pton function in order to translate IP address in numbers-and-dots notation
		const int PORT;
		vector<struct kevent> ev;

	public:
		Socket();

		void	create_socket(struct addrinfo *AddrInfo) {
			if ((sockfd = socket(AddrInfo->ai_family, AddrInfo->ai_socktype, AddrInfo->ai_protocol)) < 0)
				throw ...;
			// put it into the non-blocking mode
			fcntl(sockfd, F_SETFL, O_NONBLOCK); // but it might be considered as an old way
		}

		void	name_socket(struct *AddrInfo) {
			if (bind(sockfd, AddrInfo->ai_addr, AddrInfo->ai_addrlen) < 0)
				throw ...;
		}
		
		void	wait_connection() {
			if (listen(sockfd, 20) < 0)
				throw ...;
		}

		int		newConnection() {
			int	newConnection;
			socklen_t	addrlen = sizeof(sockAddr);
			if ((newConnection = accept(sockfd, (struct sockaddr *) &sockAddr, (socklen_t *) &addrlen)) < 0)
				throw ...;
			return (newConnection);
		}

		void	receiveData(int connection) {
			
		}

		void	sendData(int sockfd, string s) {
			send(connection, s.c_str(), s.size(), 0);
		}

		void	someExampleCode {
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
		}
};

//* NOTE
//* In order to use poll, i'll use vector for struct pollfd
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/14 20:46:11 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* functions to use : socket, listen, bind, accept

#pragma once

#include "Webserv.hpp"

using namespace std;

class Socket {
	private:
		int	server_fd;
		struct sockaddr_in sockAddr; // in represents IP Network address.
		// i can consider here using struct sockaddr_storage instead of sockaddr_in
		// in order to cover both IPv4 and IPv6 address. In that case, i might have to use
		// inet_pton function in order to translate IP address in numbers-and-dots notation
		const int PORT;

	public:
		Socket();

		void	create_socket(struct addrinfo *AddrInfo) {
			if ((server_fd = socket(AddrInfo->ai_family, AddrInfo->ai_socktype, AddrInfo->ai_protocol)) < 0)
				throw ...;
		}

		void	name_socket(struct *AddrInfo) {
			if (bind(server_fd, AddrInfo->ai_addr, AddrInfo->ai_addrlen) < 0)
				throw ...;
		}
		
		void	wait_connection() {
			if (listen(server_fd, 42) < 0)
				throw ...;
		}

		int		newConnection() {
			int	newConnection;
			socklen_t	addrlen = sizeof(sockAddr);
			if ((newConnection = accept(server_fd, (struct sockaddr *) &sockAddr, (socklen_t *) &addrlen)) < 0)
				throw ...;
			return (newConnection);
		}

		void	receiveData(int connection) {
			char	buf[9999];
			int fd = read(connection, buf, 9999);
			std::cout <<
		}

		void	sendData(int connection, string s) {
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
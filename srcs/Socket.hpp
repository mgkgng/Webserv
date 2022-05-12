/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/12 18:33:48 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* functions to use : socket, listen, bind, accept

#include <sys/socket.h>
#include <netinet/in.h>

class Socket {
	private:
		int	server_fd;
		struct sockaddr_in sockAddr;

	public:
		Socket();

		void	create_socket() {
			if ((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				throw exception;
		}

		void	name_socket() {
			sockAddr.
			bind(server_fd, sockAddr)
		}
};
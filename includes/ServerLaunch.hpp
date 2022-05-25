/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLaunch.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 20:08:08 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/25 15:29:36 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "libft.hpp"
#include "Client.hpp"

namespace Webserv {

class ServerLaunch {
	public:
		ServerLaunch();
		ServerLaunch(const ServerLaunch & server);
		~ServerLaunch();
		ServerLaunch & operator=(const ServerLaunch & server);
		
		static void	thread_launch(void *ptr[2]);
		void	launch(Server *server);
		void	start(std::vector<Server> & servers);
		
	private:			
		int								sockfd;
		int								kq;
		struct sockaddr_in				sockaddr;
		int								addrlen;
		std::vector<struct kevent>		chlist;
		std::vector<struct kevent>		evlist;
		std::vector<Client>				clients;
		bool							quit;

		void	init_addrinfo();
		void	init_server();
		void	acceptConnection();
		void	disconnect(int fd);
		void	start(std::vector<Server> & servers);
		void	sendData(int c_fd);
		void	recvData(struct kevent &ev);
		Client* getClient(int fd);
};
};
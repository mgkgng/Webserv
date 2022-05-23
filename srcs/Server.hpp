/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/23 14:08:55 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include "ConnectionData.hpp"
#include "Client.hpp"

#define PORT 8080
#define BACKLOG 20

using namespace std;

class Server {
	private:
		int						sockfd;
		int						kq;
		struct sockaddr_in		sockaddr;
		int						addrlen;
		vector<struct kevent>	chlist;
		vector<struct kevent>	evlist;
		vector<Client>			clients;
		bool					quit;
		
		void	init_addrinfo();
		void	init_server();
		void	acceptConnection();
		void	disconnect(int fd);
		void	registerEvents();
		void	sendData(int c_fd);
		void	recvData(struct kevent &ev);
		Client* getClient(int fd);

	public:
		Server();
		Server(Server const &);
		~Server();

		Server & operator=(Server const &);

		void	launch(Server &server);

};
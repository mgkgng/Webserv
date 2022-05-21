/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/21 22:17:13 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"
#include "ConnectionData.hpp"
#include "Client.hpp"

#define PORT "8080"
#define BACKLOG 20

using namespace std;

class Server {
	private:
		int						sockfd;
		int						kq;
		struct addrinfo 		*info;
		vector<struct kevent>	chlist;
		vector<struct kevent>	evlist;
		vector<Client>			clients;
		bool					quit;
		
		void	init_addrinfo() {
			struct addrinfo hints = {0};

			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			//** A REFAIRE

			assert(getaddrinfo(NULL, PORT, &hints, &info) == 0);
		}
		
		void	init_server() {
			sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
			assert(sockfd != -1);

			bool	option_on = true;
			assert(setsockopt(sockfd, IPPROTO_TCP, SO_REUSEADDR, &option_on, sizeof(bool)) == 0);

			assert(bind(sockfd, info->ai_addr, info->ai_addrlen) == 0);
			assert(listen(sockfd, BACKLOG) == 0);

			kq = kqueue();
			assert(kq != -1);
		}
		
		void	terminate() {
			close(kq);
			freeaddrinfo(info);
		}

	public:
		Server() : quit(false) {
			init_addrinfo();
			init_server();
		}
		~Server() {
			terminate();
		}

		void	acceptConnection() {
			int		newConnection = accept(sockfd, info->ai_addr, &info->ai_addrlen);
			assert(newConnection != -1);
			fcntl(newConnection, F_SETFL, O_NONBLOCK);

			chlist.resize(chlist.size() + 1);
			EV_SET(chlist.end().base() - 1, newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);

			Client	c = Client(newConnection);
			clients.push_back(c);
			cout << "Connection accpeted." << endl;
		}
	
		void	registerEvents() {
			int evNb, fd;

			evlist.clear();
			evlist.resize(1);
			evNb = kevent(kq, &chlist[0], chlist.size(), &evlist[0], evlist.size(), NULL);
			chlist.clear();
			if (evNb < 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
				return ;
			for (int i = 0; i < evNb; i++) {
				if (evlist[i].flags & EV_EOF) {
					cout << "disconnect" << endl;
					close(evlist[i].ident);
					deleteClient(evlist[i].ident);
				} else if (evlist[i].ident == sockfd) {
					acceptConnection();
				} else if (evlist[i].filter & EVFILT_READ) {
					recvData(evlist[i]);
				} else if (evlist[i].filter & EVFILT_WRITE) {
					sendData(evlist[i].ident); // EST-CE QUE POSSIBLE DE WRITE, C'EST UN EVENEMENT?
				}
			}
		}
		
		void	sendData(int fd) {
			Client *client = getClient(fd);
			// error check
			send(client->getIdent(), client->getMsg().c_str(), client->getMsg().size(), 0);
			chlist.resize(chlist.size() + 1);
			EV_SET(chlist.end().base() - 1, ev.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
			// should check if it works like this
		}

		void	recvData(struct kevent &ev) {
			Client	*client = getClient(ev.ident);
			// error check
			char	buf[10000];
			int		ret;

			ret = recv(ev.ident, buf, 9999, 0);
			if (ret < 0)
				return ;
			if (!ret) {
				chlist.resize(chlist.size() + 2);
				EV_SET(chlist.end().base() - 2, ev.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				EV_SET(chlist.end().base() - 1, ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				return ;
			}
			buf[ret] = '\0';
			client->putMsg(string(buf));
		}

		void	launch(Server &server) {
			cout << "WEBSERV launched." << endl;
			quit = false;
			while (!quit) {
				cout << "......waiting for connection....." << endl;
				registerEvents();
			}
		}

		Client* getClient(int fd) {
			for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
				if (it->isClient(fd))
					return (it.base());
			}
			return (NULL); // comment gerer ca?
		}

		void	deleteClient(int fd) {
			for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
				if (it->isClient(fd))
					clients.erase(it);
			}
		}

};
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/12 14:32:01 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/21 14:56:35 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

#include "ConnectionData.hpp"

#define PORT "8080"
#define BACKLOG 20
#define EV_SIZE 1024

using namespace std;

class Server {
	private:
		int						sockfd;
		int						kq;
		struct addrinfo 		*info;
		//vector<struct kevent>	chlist;
		struct kevent			evlist[EV_SIZE];
		string					receivedData;
		struct timespec			timeout;
		
		void	init_addrinfo() {
			struct addrinfo hints = {0};

			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;

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
		Server() {
			init_addrinfo();
			init_server();
		}
		~Server() {
			terminate();
		}

		void	acceptConnection() {
			int		newConnection = accept(sockfd, info->ai_addr, &info->ai_addrlen);
			assert (newConnection != -1);
			fcntl(newConnection, F_SETFL, O_NONBLOCK);

			EV_SET(&evSet, newConnection, EVFILT_READ, EV_ADD, 0, 0, NULL);
			assert(kevent(kq, &evSet, 1, NULL, 0, NULL) == 0);
			cout << "Connection accpeted." << endl;
	
			chlist.push_back(newEvent);
		}
	
		void	registerEvents() {
			int nev, fd;
			struct kevent nkev;

			nev = kevent(kq, NULL, 0, evlist, EV_SIZE, NULL);
			assert(nev != -1);
			for (int i = 0; i < nev; i++) {
				if (evlist[i].flags & EV_EOF) {
					cout << "disconnect" << endl;
					close(evlist[i].ident);
				} else if (evlist[i].ident == sockfd) {
					acceptConnection();
				} else if (evlist[i].filter & EVFILT_READ) {
					recvData(evlist[i]);
				} else if (evlist[i].filter == EVFILT_WRITE) {
					sendData("Connection is done.\n");
				}	
			}
		}
		
		void	sendData(string s) {
			send(sockfd, s.c_str(), s.size(), 0);
		}

		void	recvData(struct kevent &ev) {
			char	buf[10000];
			int		ret;


			ret = recv(sockfd, buf, 9999, 0);
			if (ret < 0)
				return ;
			if (!ret) {
				ConnectionData& cd = ;
				cd.RequestDone();
				EV_SET(&ev, sockfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				EV_SET(&ev, sockfd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				return ;
			}
			buf[ret] = '\0';
			if (!ev.udata) {
				ConnectionData cd;
				cd.putRequestData(string(buf));
				ev.udata = &cd; // a checker
			}
		}

		void	launch(Server &server) {
			cout << "WEBSERV launched." << endl;
			while (true) {
				cout << "......waiting for connection....." << endl;
				registerEvents();
			}
		}

};
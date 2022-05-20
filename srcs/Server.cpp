/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 12:37:21 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/20 19:27:52 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : chlist(1) {
	init_addrinfo();
	init_server();
}

Server::~Server() {
	terminate();
}

void	Server::init_info() {
	timeout.tv_sec = 5000;
	timeout.tv_sec = 0;
}

void	Server::init_addrinfo() {
	struct addrinfo hints = {0};
	int				status;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(NULL, PORT, &hints, &info)) != 0)
		throw SystemCallError("getaddrinfo error.");
}

void	Server::init_server() {

	if ((sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) < 0)
		throw SystemCallError("socket error.");

	bool	option_on = true;
	if ((setsockopt(sockfd, IPPROTO_TCP, SO_REUSEADDR, &option_on, sizeof(bool)) < 0))
		throw SystemCallError("setsockopt error.");
		
	if (bind(sockfd, info->ai_addr, info->ai_addrlen) < 0)
		throw SystemCallError("bind error.");

	if (listen(sockfd, BACKLOG) < 0)
		throw SystemCallError("listen error.");

	if ((kq = kqueue()) < 0)
		throw SystemCallError("kqueue error.");
}

void	Server::terminate() {
	close(kq);
	freeaddrinfo(info);
}

void	Server::broadcastMsg(string s) {
	cout << s << endl;
}

void	Server::broadcastErr(string s) {
	cerr << s << endl;
}

void	Server::acceptConnection() {
	int		newConnection;
	if ((newConnection = accept(sockfd, info->ai_addr, &info->ai_addrlen) < 0))
		throw SystemCallError("accept error.");
	fcntl(newConnection, F_SETFL, O_NONBLOCK);

	broadcastMsg("Connection accpeted.");
	struct kevent	newEvent;
	EV_SET(&newEvent, newConnection, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &newEvent, 1, NULL, 0, NULL) == -1)
		throw SystemCallError("kevent error.");
	chlist.push_back(newEvent);
}

void	Server::disconnect(int fd) {
	struct kevent newEvent;
	EV_SET(&newEvent, sockfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	if (kevent(kq, &newEvent, 1, NULL, 0, NULL) < 0)
		throw SystemCallError("kevent error.");
	
	broadcastMsg("disconnect");
}

void	Server::registerEvents() {
	int nev, fd;
	struct kevent nkev;

	nev = kevent(kq, &chlist[0], chlist.size(), evlist, EV_SIZE, NULL);
	if (nev <= 0)
		throw SystemCallError("kqueue error.");
	chlist.clear();
	for (int i = 0; i < nev; i++) {
		if (evlist[i].flags & EV_EOF) {
			try {
				disconnect(evlist[i].ident);
				continue;
			} catch (SystemCallError &e) {
				broadcastErr(e.what());
				return ;
			}
		} else if (evlist[i].ident == sockfd) {
			try {
				acceptConnection();
			} catch (SystemCallError &e) {
				broadcastErr(e.what());
				return ;
			}
		}
		if (evlist[i].filter == EVFILT_READ) {
			recvData(evlist[i]);
		} else if (evlist[i].filter == EVFILT_WRITE) {
			sendData("Connection is done.\n");
		}
	}
}

void	Server::launch(Server &server) {

	broadcastMsg("WEBSERV launched.");
	
	try {
		init_addrinfo();
		init_server();
	} catch (SystemCallError& e) {
		broadcastErr(e.what());
		return ;
	}

	while (true) {
		broadcastMsg("......waiting for connection.....");
		try {
			registerEvents();
		} catch (SystemCallError &e) {
			broadcastErr(e.what());
			return ;
		}
		
	}
}

void	Server::sendData(string s) {
	send(sockfd, s.c_str(), s.size(), 0);
}

void	Server::recvData(struct kevent ev) {
	char	buf[10000];
	int		ret;
	
	ret = recv(sockfd, buf, 9999, 0);
	if (ret < 0)
		return ;
	if (!ret) {
		EV_SET(&ev, sockfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		EV_SET(&ev, sockfd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		return ;
	}
	buf[ret] = '\0';
	
	
	string(buf);

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

// kqueue should be closed at the end.
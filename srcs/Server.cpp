/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 14:05:13 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/23 14:10:29 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : quit(false) {
	init_addrinfo();
	init_server();
}

Server::Server(Server const & other) {
	*this = other;
}

Server::~Server() {
	close(kq);
}

Server & operator=(Server const & rhs) {
	sockfd = rhs.sockfd;
	kq = rhs.kq;
	sockaddr = rhs.sockaddr;
	addrlen = rhs.addrlen;
	chlist = rhs.chlist;
	evlist = rhs.evlist;
	clients = rhs.clients;
	quit = rhs.quit;
	return (*this);
}

void	Server::init_addrinfo() {
	bzero(&sockaddr, sizeof(struct sockaddr_in));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(PORT);
	addrlen = sizeof(sockaddr);
}
		
void	Server::init_server() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	bool	option_on = true;
	assert(setsockopt(sockfd, IPPROTO_TCP, SO_REUSEADDR, &option_on, sizeof(bool)) == 0);

	assert(bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == 0);
	assert(listen(sockfd, BACKLOG) == 0);

	kq = kqueue();
	assert(kq != -1);
}

void	Server::acceptConnection() {
	int	newConnection = accept(sockfd, (struct sockaddr *) &sockaddr, (socklen_t *) &addrlen);
	assert(newConnection != -1);
	fcntl(newConnection, F_SETFL, O_NONBLOCK);

	chlist.resize(chlist.size() + 1);
	EV_SET(chlist.end().base() - 1, newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);

	Client	c = Client(newConnection);
	clients.push_back(c);
	cout << "Connection accpeted." << endl;
}
	
void	Server::disconnect(int fd) {
	cout << "disconnect" << endl;
	close(fd);
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->isClient(fd))
			clients.erase(it);
}

void	Server::sendData(int c_fd) {
	// should figure out how to know whether i'm going to send data to client or to cgi
	Client *client = getClient(c_fd);
	assert(client != NULL);
	send(client->getIdent(), client->getMsg().c_str(), client->getMsg().size(), 0);
	chlist.resize(chlist.size() + 1);
	EV_SET(chlist.end().base() - 1, c_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	// should check if it works like this
}

void	Server::recvData(struct kevent &ev) {
	char	buf[10000];
	int		ret;
	Client	*client = getClient(ev.ident);
	assert(client != NULL);

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
			
void	Server::registerEvents() {
	int evNb, fd;

	evlist.clear();
	evlist.resize(1);
	evNb = kevent(kq, &chlist[0], chlist.size(), &evlist[0], evlist.size(), NULL);
	chlist.clear();
	if (evNb < 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
		return ;
	for (int i = 0; i < evNb; i++) {
		if (evlist[i].flags & EV_EOF) 
			disconnect(evlist[i].ident);
		else if (evlist[i].ident == sockfd)
			acceptConnection();
		else if (evlist[i].filter & EVFILT_READ)
			recvData(evlist[i]);
		else if (evlist[i].filter & EVFILT_WRITE)
			sendData(evlist[i].ident); // EST-CE QUE POSSIBLE DE WRITE, C'EST UN EVENEMENT?
	}
}
		
void	Server::launch(Server &server) {
	cout << "WEBSERV launched." << endl;
	quit = false;
	while (!quit) {
		cout << "......waiting for connection....." << endl;
		registerEvents();
	}	
}

Client* getClient(int fd) {
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->isClient(fd))
			return (it.base());
	return (NULL);
}
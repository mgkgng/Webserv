/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLaunch.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 16:54:33 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/24 17:35:00 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

using namespace Webserv;

ServerLaunch::ServerLaunch() {

}

ServerLaunch::ServerLaunch(ServerLaunch const & other) {
	*this = other;
}

ServerLaunch::~ServerLaunch() {
	close(kq);
}

ServerLaunch &	ServerLaunch::operator=(ServerLaunch const & rhs) {
	this->sockfd = rhs.sockfd;
	this->kq = rhs.kq;
	this->sockaddr = rhs.sockaddr;
	this->addrlen = rhs.addrlen;
	this->chlist = rhs.chlist;
	this->evlist = rhs.evlist;
	this->clients = rhs.clients;
	this->quit = rhs.quit;
	return (*this);
}

void	ServerLaunch::init_addrinfo() {
	bzero(&sockaddr, sizeof(struct sockaddr_in));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(PORT);
	addrlen = sizeof(sockaddr);
}
		
void	ServerLaunch::init_server() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	int	option_on = 1;
	assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option_on, sizeof(int)) == 0);
	assert(bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == 0);
	assert(listen(sockfd, BACKLOG) == 0);

	kq = kqueue();
	assert(kq != -1);
}

void	ServerLaunch::acceptConnection() {
	int	newConnection = accept(sockfd, (struct sockaddr *) &sockaddr, (socklen_t *) &addrlen);
	assert(newConnection != -1);
	fcntl(newConnection, F_SETFL, O_NONBLOCK);

	chlist.resize(chlist.size() + 1);
	EV_SET(chlist.end().base() - 1, newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);

	Client	c = Client(newConnection);
	clients.push_back(c);
	std::cout << "Connection accpeted." << std::endl;
}
	
void	ServerLaunch::disconnect(int fd) {
	std::cout << "disconnect" << std::endl;
	close(fd);
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->isClient(fd))
			clients.erase(it);
}

void	ServerLaunch::sendData(int c_fd) {
	// should figure out how to know whether i'm going to send data to client or to cgi
	
	//* if back to client
	Client *client = getClient(c_fd);
	assert(client != NULL);
	send(client->getIdent(), client->getResponseStr().c_str(), client->getResponseStr().size(), 0);
	chlist.resize(chlist.size() + 1);
	EV_SET(chlist.end().base() - 1, c_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	// should check if it works like this

	//* if to CGI
}

void	ServerLaunch::recvData(struct kevent &ev) {
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
		client->putRequest();
		return ;
	}
	buf[ret] = '\0';
	client->putRequestStr(std::string(buf));
}
			
void	ServerLaunch::thread_launch(void *ptr[2]) {
	ServerLaunch	*launch;

	launch = reinterpret_cast<ServerLaunch*>(ptr[0]);
	launch->launch(reinterpret_cast<Server *>(ptr[1]));
}

void	ServerLaunch::launch(Server *server) {
	int evNb;

	init_addrinfo();
	init_server();
	std::cout << "WEBSERV launched." << std::endl;
	quit = false;

	while (!quit) {
		evlist.clear();
		evlist.resize(1);
		evNb = kevent(kq, &chlist[0], chlist.size(), &evlist[0], evlist.size(), NULL);
		chlist.clear();
		if (evNb < 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
			return ;
		for (int i = 0; i < evNb; i++) {
			if (evlist[i].flags & EV_EOF) 
				disconnect(evlist[i].ident);
			else if (*(reinterpret_cast<int *>(evlist[i].ident)) == sockfd)
				acceptConnection();
			else if (evlist[i].filter & EVFILT_READ)
				recvData(evlist[i]);
			else if (evlist[i].filter & EVFILT_WRITE)
				sendData(evlist[i].ident);
		}
	}
}
		
void	ServerLaunch::start(std::vector<Server> & servers) {
	std::vector<pthread_t> threads;
	std::vector<Server>::iterator it_servers = servers.begin();
	threads.resize(servers.size());
	for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++)
		pthread_create(&(*it), NULL, (void * (*)(void *)) &ServerLaunch::thread_launch, (void *[2]) {this, &(*it_servers++)});
	for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++)
		pthread_detach(*it);
}

Client* ServerLaunch::getClient(int fd) {
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->isClient(fd))
			return (it.base());
	return (NULL);
}
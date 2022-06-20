/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 16:54:33 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/20 14:19:45 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

using namespace Webserv;

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

	int	option_on = 1;
	assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option_on, sizeof(int)) == 0);
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
	std::cout << "Connection accpeted." << std::endl;
}
	
void	Server::disconnect(int fd) {
	std::cout << "disconnect" << std::endl;
	close(fd);
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->isClient(fd))
			clients.erase(it);
}

void	Server::sendData(int c_fd) {
	
	Client *client;
	client = getClient(c_fd);
	assert(client != NULL);

	//Request	*r = client->getRequest();

	//std::string statusCode = execute_cgi(r);
	
	//* now back to client
	send(client->getIdent(), client->getResponseStr().c_str(), client->getResponseStr().size(), 0);
	chlist.resize(chlist.size() + 1);
	EV_SET(chlist.end().base() - 1, c_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
}

void	Server::recvData(struct kevent &ev) {
	char	buf[10000];
	int		ret;
	Client	*client;
	
	client = getClient(ev.ident);
	assert(client != NULL);

	ret = recv(ev.ident, buf, 9999, 0);
	if (ret < 0)
		return ;
	if (!ret) {
		chlist.resize(chlist.size() + 2);
		EV_SET(chlist.end().base() - 2, ev.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		EV_SET(chlist.end().base() - 1, ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		client->putRequest();
		client->clearReponseStr();
		return ;
	}
	buf[ret] = '\0';
	client->putRequestStr(std::string(buf));
}
			
void	Server::thread_launch(void *ptr[2]) {
	Server	*launch;

	launch = reinterpret_cast<Server*>(ptr[0]);
	launch->launch(reinterpret_cast<Server *>(ptr[1]));
}

void	Server::launch(Server *server) {
	int evNb;

	server->init_addrinfo();
	server->init_server();
	std::cout << "WEBSERV launched." << std::endl;
	server->quit = false;

	while (!server->quit) {
		server->evlist.clear();
		server->evlist.resize(1);
		evNb = kevent(server->kq, &server->chlist[0], server->chlist.size(), 
			&server->evlist[0], server->evlist.size(), NULL);
		server->chlist.clear();
		if (evNb < 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
			return ;
		for (int i = 0; i < evNb; i++) {
			if (server->evlist[i].flags & EV_EOF) 
				server->disconnect(server->evlist[i].ident);
			else if (*(reinterpret_cast<int *>(server->evlist[i].ident)) == server->sockfd)
				server->acceptConnection();
			else if (server->evlist[i].filter & EVFILT_READ)
				server->recvData(server->evlist[i]);
			else if (server->evlist[i].filter & EVFILT_WRITE)
				server->sendData(server->evlist[i].ident);
		}
	}
}
		
void	Server::start(std::vector<Server> & servers) {
	if (servers.size() == 1)
		launch(&servers.at(0));
	else {
		std::vector<pthread_t> threads;
		std::vector<Server>::iterator it_servers = servers.begin();
		threads.resize(servers.size());
		for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++)
			pthread_create(&(*it), NULL, (void * (*)(void *)) &Server::thread_launch, (void *[2]) {this, &(*it_servers++)});
		for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++)
			pthread_detach(*it);
	}
}

Client* Server::getClient(int fd) {
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->isClient(fd))
			return (it.base());
	return (NULL);
}
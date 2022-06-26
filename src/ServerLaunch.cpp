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
	sockaddr.sin_port = htons(this->port);
	addrlen = sizeof(sockaddr);
}
		
void	Server::init_server() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	int	option_on = 1;
	assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option_on, sizeof(int)) == 0);
	bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
	assert(listen(sockfd, 20) == 0);

	kq = kqueue();
	assert(kq != -1);

	chlist.resize(1);
	EV_SET(chlist.data(), sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
}

void	Server::acceptConnection() {
	int	newConnection = accept(sockfd, (struct sockaddr *) &sockaddr, (socklen_t *) &addrlen);
	assert(newConnection != -1);
	fcntl(newConnection, F_SETFL, O_NONBLOCK);

	chlist.resize(chlist.size() + 1);
	EV_SET(chlist.end().base() - 1, newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);

	// Client	c = Client(newConnection);
	// clients.push_back(c);
	std::cout << "Connection accepted." << std::endl;
}
	
void	Server::disconnect(int fd) {
	std::cout << "disconnect" << std::endl;
	close(fd);
}

// void	Server::sendData(int c_fd) {	
// 	Client *client;
// 	client = getClient(c_fd);
// 	assert(client != NULL);

// 	//Request	*r = client->getRequest();
// 	//std::string statusCode = execute_cgi(r);
	
// 	//* now back to client
// 	send(client->getIdent(), client->getResponseStr().c_str(), client->getResponseStr().size(), 0);
// 	chlist.resize(chlist.size() + 1);
// 	EV_SET(chlist.end().base() - 1, c_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
// }

void	Server::recvData(struct kevent &ev) {
	char	buf[10000];
	int		ret;
	// Client	*client;
	
	// client = getClient(ev.ident);
	// assert(client != NULL);

	ret = recv(ev.ident, buf, 9999, 0);
	// std::cout << client->getIdent() << std::endl;
	if (ret < 0)
		return ;
	if (!ret) {
		chlist.resize(chlist.size() + 2);
		EV_SET(chlist.end().base() - 2, ev.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		EV_SET(chlist.end().base() - 1, ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		// client->putRequest();
		// client->clearReponseStr();
		return ;
	}
	buf[ret] = '\0';
	// std::cout << buf << std::endl;

	std::cout << "Client Sent data!" << std::endl;
	try {
		Request request = Request(buf, *this);
	} catch (Webserv::Request::ERROR400 & e) {

	} catch (Webserv::Request::ERROR404 & e) {
		std::cout << "FILE NOT FOUND: ERROR 404" << std::endl;
	} catch (Webserv::Request::ERROR405 & e) {
		std::cout << "DISALLOWED METHOD: ERROR 405" << std::endl;
	}	// client->putRequestStr(std::string(buf));
}

void	Webserv::thread_launch(void *ptr) {
	Server	*launch;

	launch = reinterpret_cast<Server*>(ptr);
	launch->launch();
}

void	Server::launch() {
	int evNb;

	init_addrinfo();
	init_server();
	std::cout << "WEBSERV launched." << std::endl;
	std::cout << this->port << std::endl;
	quit = false;

	while (!quit) {
		evlist.clear();
		evlist.resize(1);
		evNb = kevent(kq, chlist.data(), chlist.size(), evlist.data(), evlist.size(), NULL);
		chlist.clear();
		if (evNb < 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
			return ;
		for (int i = 0; i < evNb; i++) {
			if (evlist.at(i).flags & EV_EOF)
				disconnect(evlist[i].ident);
			else if (static_cast<int>(evlist.at(i).ident) == sockfd)
				acceptConnection();
			else if (evlist.at(i).filter & EVFILT_READ)
				recvData(evlist[i]);
			// else if (print(4) && evlist.at(i).filter & EVFILT_WRITE)
			// 	sendData(evlist[i].ident);
		}
	}
}
		
void	Webserv::start(std::vector<Server> & servers) {
	if (servers.size() == 1)
		servers[0].launch();
	else {
		std::vector<pthread_t> threads;
		std::vector<Server>::iterator it_servers = servers.begin();
		threads.resize(servers.size());
		for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++)
			pthread_create(&(*it), NULL, (void * (*)(void *)) &Webserv::thread_launch, &(*it_servers++));
		for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++)
			pthread_detach(*it);
	}
}

// Client* Server::getClient(int fd) {
// 	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
// 		if (it->isClient(fd))
// 			return (it.base());
// 	return (NULL);
// }
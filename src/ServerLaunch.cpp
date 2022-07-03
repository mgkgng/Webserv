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
	assert(bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == 0);
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

	std::cout << "Connection accepted." << std::endl;
}
	
void	Server::disconnect(int fd) {
	std::cout << "disconnect" << std::endl;
	close(fd);
}

void	Server::recvData(struct kevent &ev, std::vector<Webserv::Server> & servers) {
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
	std::cout << "Client Sent data!" << std::endl;
	try {
		//std::cout << buf << std::endl;
		Request request = Request(buf, servers);
		std::string	res = request.getHtml();
		std::cout << res << std::endl;
		//send(clientID, s.c_str(), s.size(), 0);
		//chlist.resize(chlist.size() + 1);
		//EV_SET(chlist.end().base() - 1, c_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

	} catch (Webserv::Request::ERROR400 & e) {
		std::cout << "INVAID REQUEST: ERROR 400" << std::endl;
	} catch (Webserv::Request::ERROR403 & e) {
		std::cout << "FORBIDDEN: ERROR 403" << std::endl;
	} catch (Webserv::Request::ERROR404 & e) {
		std::cout << "FILE NOT FOUND: ERROR 404" << std::endl;
	} catch (Webserv::Request::ERROR405 & e) {
		std::cout << "DISALLOWED METHOD: ERROR 405" << std::endl;
	} catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}	
}

void	Webserv::thread_launch(void *ptr) {
	std::vector<Server> *launch;

	launch = reinterpret_cast<std::vector<Server> *>(ptr);
	launch->begin()->launch((*launch));
}

void	Server::launch(std::vector<Server> servers) {
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
				recvData(evlist[i], servers);
		}
	}
}
		
void	Webserv::start(std::vector<Server> & servers) {
	std::vector<Server>::iterator it_servers = servers.begin();
	std::vector<Server> ports;
	for (; it_servers != servers.end(); it_servers++) {
		if (ports.empty()) {
			ports.push_back((*it_servers));
		} else if (ports[ports.size() - 1].getPort() != it_servers->getPort()) {
			ports.push_back((*it_servers));
		}
	}
	if (ports.size() == 1)
		servers[0].launch(servers);
	else {
		std::vector<pthread_t> threads;
		it_servers = servers.begin();
		std::vector<Server>::iterator it_ports = ports.begin();
		threads.resize(ports.size());
		for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++) {
			while (it_servers->getPort() < it_ports->getPort() && it_servers != servers.end()) {
				it_servers ++;
			}
			std::vector<Server>::iterator end = it_servers;
			while (end->getPort() == it_ports->getPort()) {
				end ++;
			}
			std::vector<Server> serverReadyToGo(it_servers, end);
			pthread_create(&(*it), NULL, (void * (*)(void *)) &Webserv::thread_launch, &(serverReadyToGo));
			it_ports++;
			usleep(300);
		}
		for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++)
			pthread_detach(*it);
		while (1);
	}
}
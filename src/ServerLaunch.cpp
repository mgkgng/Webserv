#include "Webserv.hpp"

Webserv::ServerLaunch::ServerLaunch() {

}

Webserv::ServerLaunch::ServerLaunch(const ServerLaunch & other) {
	*this = other;
}

Webserv::ServerLaunch::~ServerLaunch() {
	close(kq);
}

Webserv::ServerLaunch &	Webserv::ServerLaunch::operator=(const ServerLaunch & rhs) {
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

void	Webserv::ServerLaunch::init_addrinfo() {
	bzero(&sockaddr, sizeof(struct sockaddr_in));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(PORT);
	addrlen = sizeof(sockaddr);
}
		
void	Webserv::ServerLaunch::init_server() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	bool	option_on = true;
	assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option_on, sizeof(bool)) == 0);

	assert(bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == 0);
	assert(listen(sockfd, BACKLOG) == 0);

	kq = kqueue();
	assert(kq != -1);
}

void	Webserv::ServerLaunch::acceptConnection() {
	int	newConnection = accept(sockfd, (struct sockaddr *) &sockaddr, (socklen_t *) &addrlen);
	assert(newConnection != -1);
	fcntl(newConnection, F_SETFL, O_NONBLOCK);

	chlist.resize(chlist.size() + 1);
	EV_SET(chlist.end().base() - 1, newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);

	Client	c = Client(newConnection);
	clients.push_back(c);
	std::cout << "Connection accpeted." << std::endl;
}
	
void	Webserv::ServerLaunch::disconnect(int fd) {
	std::cout << "disconnect" << std::endl;
	close(fd);
	for (std::vector<Webserv::Client>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->isClient(fd))
			clients.erase(it);
}

void	Webserv::ServerLaunch::sendData(int c_fd) {
	// should figure out how to know whether i'm going to send data to client or to cgi
	Client *client = getClient(c_fd);
	assert(client != NULL);
	send(client->getIdent(), client->getMsg().c_str(), client->getMsg().size(), 0);
	chlist.resize(chlist.size() + 1);
	EV_SET(chlist.end().base() - 1, c_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	// should check if it works like this
}

void	Webserv::ServerLaunch::recvData(struct kevent &ev) {
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
	client->putMsg(std::string(buf));
}
			
void	Webserv::ServerLaunch::launch() {
	int evNb;

	init_addrinfo();
	init_server();
	std::cout << "WEBSERV launched." << std::endl;
	quit = false;
	while (!quit) {
		std::cout << "......waiting for connection....." << std::endl;

	}	
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
		
void	Webserv::ServerLaunch::start(std::vector<Server> & servers) {
	std::vector<std::thread> threads;
	threads.resize(servers.size());
	for (std::vector<std::thread>::iterator it = threads.begin(); it != threads.end(); it++) {

	}


}

Webserv::Client* Webserv::ServerLaunch::getClient(int fd) {
	for (std::vector<Webserv::Client>::iterator it = clients.begin(); it != clients.end(); it++)
		if (it->isClient(fd))
			return (it.base());
	return (NULL);
}
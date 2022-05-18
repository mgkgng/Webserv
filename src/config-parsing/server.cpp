#include <Webserv.hpp>


Webserv::Server::Server() {

}

Webserv::Server::Server(const Server & server) {
	this->servername = server.getServerName();
	this->host = server.getHost();
	this->port = server.getPort();
	this->isdefault = server.getIsDefault();
	this->routes = server.getRoutes();
	this->codehandlers = server.getHandleCode();
}

Webserv::Server::~Server() { }

Webserv::Server &	Webserv::Server::operator=(const Server & server) {
	this->servername = server.getServerName();
	this->host = server.getHost();
	this->port = server.getPort();
	this->isdefault = server.getIsDefault();
	this->routes = server.getRoutes();
	this->codehandlers = server.getHandleCode();
	return (*this);
}

std::string	Webserv::Server::getServerName() const {
	return (this->servername);
}

std::string	Webserv::Server::getHost() const {
	return (this->host);
}

unsigned int	Webserv::Server::getPort() const {
	return (this->port);
}

bool	Webserv::Server::getIsDefault() const {
	return (this->isdefault);
}

std::vector<Webserv::Route>	Webserv::Server::getRoutes() const {
	return (this->routes);
}

std::vector<Webserv::HandleCode>	Webserv::Server::getHandleCode() const {
	return (this->codehandlers);
}

void	Webserv::Server::addRoute(const Webserv::Route & route) {
	this->routes.push_back(route);
}

void	Webserv::Server::addCodeHandler(const Webserv::HandleCode & handlecode) {
	this->codehandlers.push_back(handlecode);
}

// std::vector<Webserv::Server>	Webserv::makeServersFromJSON(const JSON & json) {

// }

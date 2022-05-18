#include <webserver.hpp>

//default
webserv::Route::Route() { }

webserv::Route::Route(bool islistingdirectory, std::string index, std::string root, unsigned int clientmaxbodysize, std::vector<std::string> allowedHTTPmethods) {
	this->islistingdirectory = islistingdirectory;
	this->index = index;
	this->root = root;
	this->clientmaxbodysize = clientmaxbodysize;
	this->allowedHTTPmethods = allowedHTTPmethods;
}

webserv::Route::Route(const Route & route) {
	this->islistingdirectory = route.getListingDirectory();
	this->index = route.getIndex();
	this->root = route.getRoot();
	this->clientmaxbodysize = route.getClientMaxBodySize();
	this->allowedHTTPmethods = route.getAllowedHTTPMethods();
}

webserv::Route::~Route() { }

webserv::Route & webserv::Route::operator=(const Route & route) {
	this->islistingdirectory = route.getListingDirectory();
	this->index = route.getIndex();
	this->root = route.getRoot();
	this->clientmaxbodysize = route.getClientMaxBodySize();
	this->allowedHTTPmethods = route.getAllowedHTTPMethods();
	return (*this);
}

bool			webserv::Route::getListingDirectory() const {
	return (this->islistingdirectory);
}

std::string		webserv::Route::getIndex() const {
	return (this->index);
}

std::string		webserv::Route::getRoot() const {
	return (this->root);
}

std::vector<std::string> webserv::Route::getAllowedHTTPMethods() const {
	return (this->allowedHTTPmethods);
}

unsigned int	webserv::Route::getClientMaxBodySize() const {
	return (this->clientmaxbodysize);
}
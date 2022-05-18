#include <Webserv.hpp>

//default
Webserv::Route::Route() { }

Webserv::Route::Route(bool islistingdirectory, std::string index, std::string root, unsigned int clientmaxbodysize, std::vector<std::string> allowedHTTPmethods) {
	this->islistingdirectory = islistingdirectory;
	this->index = index;
	this->root = root;
	this->clientmaxbodysize = clientmaxbodysize;
	this->allowedHTTPmethods = allowedHTTPmethods;
}

Webserv::Route::Route(const Route & route) {
	this->islistingdirectory = route.getListingDirectory();
	this->index = route.getIndex();
	this->root = route.getRoot();
	this->clientmaxbodysize = route.getClientMaxBodySize();
	this->allowedHTTPmethods = route.getAllowedHTTPMethods();
}

Webserv::Route::~Route() { }

Webserv::Route & Webserv::Route::operator=(const Route & route) {
	this->islistingdirectory = route.getListingDirectory();
	this->index = route.getIndex();
	this->root = route.getRoot();
	this->clientmaxbodysize = route.getClientMaxBodySize();
	this->allowedHTTPmethods = route.getAllowedHTTPMethods();
	return (*this);
}

bool			Webserv::Route::getListingDirectory() const {
	return (this->islistingdirectory);
}

std::string		Webserv::Route::getIndex() const {
	return (this->index);
}

std::string		Webserv::Route::getRoot() const {
	return (this->root);
}

std::vector<std::string> Webserv::Route::getAllowedHTTPMethods() const {
	return (this->allowedHTTPmethods);
}

unsigned int	Webserv::Route::getClientMaxBodySize() const {
	return (this->clientmaxbodysize);
}
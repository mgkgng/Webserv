#include <Webserv.hpp>
#include <JSON.hpp>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

Webserv::Server::Server(const Server & server) {
	*this = server;
}

Webserv::Server::Server(
	std::string servername, std::string host, unsigned int port, bool isdefault, std::map<std::string, Webserv::Route> routes, std::map<std::string, Webserv::HandleCode> codes) {
	this->servername = servername;
	this->host = host;
	if (port > 65535) {
		throw PortOutsideOfRange();
	}
	this->port = port;
	this->isdefault = isdefault;
	this->routes = routes;
	this->codehandlers = codes;
	this->kq = -1;
}

Webserv::Server::~Server() {
	if (this->kq == -1)
		close(this->kq);
}

Webserv::Server &	Webserv::Server::operator=(const Server & server) {
	this->servername = server.getServerName();
	this->host = server.getHost();
	this->port = server.getPort();
	this->isdefault = server.getIsDefault();
	this->routes = server.getRoutes();
	this->codehandlers = server.getHandleCode();
	this->sockfd = server.sockfd;
	this->kq = server.kq;
	this->sockaddr = server.sockaddr;
	this->addrlen = server.addrlen;
	this->chlist = server.chlist;
	this->evlist = server.evlist;
	// this->clients = server.clients;
	this->quit = server.quit;
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

void	Webserv::Server::setIsDefault(bool d) {
	this->isdefault = d;
}

std::map<std::string, Webserv::Route>	Webserv::Server::getRoutes() const {
	return (this->routes);
}

std::map<std::string, Webserv::HandleCode>	Webserv::Server::getHandleCode() const {
	return (this->codehandlers);
}

Webserv::sbh_t	getInformation(const JSON & json, Webserv::sbh_t ret) {
	try {
		ret.index = json.getStrings().at("index");
	} catch (std::exception & e) { }
	try {
		ret.root = json.getStrings().at("root");
	} catch (std::exception & e) { }
	try {
		ret.allowedHTTPmethods = json.getStrings().at("allowed_methods");
	} catch (std::exception & e) { }
	try {
		ret.path = json.getStrings().at("path");
	} catch (std::exception & e) { }
	try {
		ret.islistingdirectory = json.getBooleans().at("autoindex");
	} catch (std::exception & e) { }
	try {
		ret.directoryfile = json.getStrings().at("directory_file");
	} catch (std::exception & e) { }
	try {
		ret.host = json.getStrings().at("host");
	} catch (std::exception & e) { }
	try {
		ret.port = json.getNumbers().at("port");
	} catch (std::exception & e) { }
	try {
		ret.servername = json.getStrings().at("server_name");
	} catch (std::exception & e) { }
	try {
		ret.code = json.getNumbers().at("code");
	} catch (std::exception & e) { }
	try {
		ret.redirect = json.getStrings().at("redirect");
	} catch (std::exception & e) { }
	try {
		ret.responsecode = json.getNumbers().at("response_code");
	} catch (std::exception & e) { }
	try {
		ret.clientmaxbodysize = json.getStrings().at("client_body_size");
	} catch (std::exception & e) { }
	try {
		ret.pythoncgiextension = json.getStrings().at("python_cgi_extension");
	} catch (std::exception & e) { }
	try {
		ret.phpcgiextextension = json.getStrings().at("php_cgi_extensions");
	} catch (std::exception & e) { }
	return (ret);
}

Webserv::sbh_t defaultInformation() {
	Webserv::sbh_t ret;
	ret.index = "index.html";
	ret.root = ".";
	ret.allowedHTTPmethods = "";
	ret.path = "/";
	ret.islistingdirectory = false;
	ret.directoryfile = "";
	ret.host = "localhost";
	ret.port = 42069;
	ret.servername = "default";
	ret.code = 0;
	ret.redirect = "/500";
	ret.responsecode = 0;
	ret.clientmaxbodysize = "5M";
	ret.pythoncgiextension = "";
	ret.phpcgiextextension = "";
	return (ret);
}

Webserv::Route	generateRoute(const JSON & json, Webserv::sbh_t sinfo) {
	if (json.getObjects().empty() != 1) {
		throw Webserv::InvalidJSONObjectInRoute();
	}
	sinfo = getInformation(json, sinfo);
	std::vector<std::string> words;

	size_t pos = 0;
	std::string sep = " ";
	while ((pos = sinfo.allowedHTTPmethods.find(sep)) != std::string::npos) {
		words.push_back(sinfo.allowedHTTPmethods.substr(0, pos));
		sinfo.allowedHTTPmethods.erase(0, pos + sep.length());
	}
	if (words.empty() != 1) {
		words.push_back(sinfo.allowedHTTPmethods);
	}
	Webserv::Route ret = Webserv::Route(sinfo.islistingdirectory, sinfo.directoryfile, sinfo.index, sinfo.root, sinfo.path, sinfo.clientmaxbodysize, words, sinfo.pythoncgiextension, sinfo.phpcgiextextension);
	return (ret);
}

Webserv::HandleCode	generateHandleCode(const JSON & json, Webserv::sbh_t sinfo, std::map<std::string, Webserv::Route> & routes) {
	if (json.getObjects().empty() != 1) {
		throw Webserv::InvalidJSONObjectInHandleCode();
	}
	sinfo = getInformation(json, sinfo);

	for (std::map<std::string, Webserv::Route>::iterator it = routes.begin(); it != routes.end(); it++) {
		if ((*it).second.getPath() == sinfo.redirect) {
			return (Webserv::HandleCode(sinfo.code, (*it).second, sinfo.responsecode));
		}
	}
	throw Webserv::InvalidJSONHandleCodeInvalidRoute(); // temp solution, should set up default error routes depending on the code.
}

std::vector<Webserv::Server>	makeServersFromJSONHelper(const JSON & json, Webserv::sbh_t sinfo, std::map<std::string, Webserv::Route> & routes, std::map<std::string, Webserv::HandleCode> & codes) {
	std::vector<Webserv::Server> ret;
	JSON::object_box temp = json.getObjects();

	sinfo = getInformation(json, sinfo);
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 5, "route") == 0) {
			if (routes.find((*it).first) != routes.end()) {
				routes.erase(routes.find((*it).first));
			}
			routes.insert(std::pair<std::string, Webserv::Route>((*it).first, generateRoute((*it).second, sinfo)));
		}
	}
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 4, "code") == 0) {
			if (codes.find((*it).first) != codes.end()) {
				codes.erase(codes.find((*it).first));
			}
			codes.insert(std::pair<std::string, Webserv::HandleCode>((*it).first, generateHandleCode((*it).second, sinfo, routes)));
		}
	}
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 6, "server") == 0) {
			std::vector<Webserv::Server>	temp = makeServersFromJSONHelper((*it).second, sinfo, routes, codes);
			ret.insert(ret.end(), temp.begin(), temp.end());
		} else if ((*it).first.compare(0, 5, "route") == 0) {
			;
		} else if ((*it).first.compare(0, 4, "code") == 0) {
			;
		} else {
			throw Webserv::InvalidJSONObjectIdentifier();
		}
	}
	for (std::vector<Webserv::Server>::iterator it = ret.begin(); it != ret.end(); it++) {
		if ((*it).getHost() == sinfo.host && (*it).getPort() == sinfo.port) {
			ret.push_back(Webserv::Server(sinfo.servername, sinfo.host, sinfo.port, false, routes, codes));
			return (ret);
		}
	} 
	ret.push_back(Webserv::Server(sinfo.servername, sinfo.host, sinfo.port, true, routes, codes));
	return (ret);
}

std::vector<Webserv::Server>	Webserv::makeServersFromJSON(const JSON & json) {
	std::vector<Webserv::Server> ret;
	std::map<std::string, Webserv::Route> routes;
	std::map<std::string,Webserv::HandleCode> codes;
	JSON::object_box temp = json.getObjects();

	Webserv::sbh_t sinfo = getInformation(json, defaultInformation());

	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 5, "route") == 0) {
			if (routes.find((*it).first) != routes.end()) {
				routes.erase(routes.find((*it).first));
			}
			routes.insert(std::pair<std::string, Webserv::Route>((*it).first, generateRoute((*it).second, sinfo)));
		}
	}
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 4, "code") == 0) {
			if (codes.find((*it).first) != codes.end()) {
				codes.erase(codes.find((*it).first));
			}
			codes.insert(std::pair<std::string, Webserv::HandleCode>((*it).first, generateHandleCode((*it).second, sinfo, routes)));
		}
	}
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 6, "server") == 0) {
			std::vector<Webserv::Server>	temp = makeServersFromJSONHelper((*it).second, sinfo, routes, codes);
			ret.insert(ret.end(), temp.begin(), temp.end());
		} else if ((*it).first.compare(0, 5, "route") == 0) {
			;
		} else if ((*it).first.compare(0, 4, "code") == 0) {
			;
		} else {
			throw Webserv::InvalidJSONObjectIdentifier();
		}
	}
	for (std::vector<Webserv::Server>::iterator it = ret.begin(); it != ret.end(); it++) {
		if ((*it).getIsDefault()) {
			for(std::vector<Webserv::Server>::iterator sit = it + 1; sit != ret.end(); sit++) {
				if ((*sit).getIsDefault() && (*sit).getHost().compare(0, (*sit).getHost().size(), (*it).getHost()) == 0 && (*sit).getPort() == (*it).getPort()) {
					(*sit).setIsDefault(false);
				}
			}
		}
	}
	return (ret);
}

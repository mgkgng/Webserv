#pragma once

#include "utility.hpp"
#include "statusCodes.hpp"
#include "Route.hpp"
#include "Server.hpp"

class Response {
	public:
		std::string protocolVer;
		unsigned int	statCode;
		std::string statMsg;
		std::map<std::string, std::string> headers;
		std::string	body;

		Response() {}

		Response(Response const & other) {
			*this = other;
		}

		~Response() {}

		Response & operator=(Response const & rhs) {
			this->protocolVer = rhs.protocolVer;
			this->statCode = rhs.statCode;
			this->statMsg = rhs.statMsg;
			this->headers = rhs.headers;
			return (*this);
		}

		void	putBody(Route &route) {

			std::cout << route << std::endl;
			std::ifstream f(route.root + "/" + route.index);
			std::stringstream buf;

			buf << f.rdbuf();
			this->body = buf.str();
			//this->headers["Content-Length"] = this->body.length();
			//this->headers["Content-Type"] = "text/html";
		}

		void putResponse(std::string path, std::map<std::string, Route> routes) {

			this->protocolVer = "HTTP/1.1";
			std::map<std::string, Route>::iterator it = routes.find(path);
			if (it == routes.end()) {
				this->statCode = NotFound;
				this->statMsg = statusCodeToString(NotFound);
				//res.putBody(NotFound);
			} else {
				this->statCode = Ok;
				this->statMsg = "OK";
				this->putBody(routes[path]);
			}
		}
		
		std::string	getStr() {
			std::string res;

			res += this->protocolVer + ' ' + std::to_string(this->statCode) + ' ' + this->statMsg + ' ' + "\r\n";
			for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
				res += it->first + ": " + it->second + "\r\n";
			res += '\n';
			res += this->body;
			return (res);
		}
};

std::ostream &operator<<(std::ostream &os, Response &res)
{
	os << res.protocolVer << ' ' << res.statCode << ' ' << res.statMsg << ' ' << "\r\n";
	for (std::map<std::string, std::string>::iterator it = res.headers.begin(); it != res.headers.end(); it++)
		os << it->first << ": " << it->second << "\r\n";
	os << std::endl;
	os << res.body;

	return (os);
}
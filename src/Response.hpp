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
			std::ifstream f(route.root + "/" + route.index);
			std::stringstream buf;

			buf << f.rdbuf();
			this->body = buf.str();
			this->headers["Content-Length"] = this->body.length();
			this->headers["Content-Type"] = "text/html";
		}

		static Response putResponse(std::string path, std::map<std::string, Route> routes) {
			Response res = Response();

			std::map<std::string, Route>::iterator it = routes.find(path);
			if (it == routes.end()) {
				res.statCode = NotFound;
				res.statMsg = statusCodeToString(NotFound);
				//res.putBody(NotFound);
			} else {
				res.statCode = Ok;
				res.statMsg = statusCodeToString(Ok);
				res.putBody(routes[path]);
			}
			return (res);
		}
		/*void	putBody(unsigned int errCode) {

		}*/
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
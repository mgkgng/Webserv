#pragma once

#include "utility.hpp"
#include "statusCodes.hpp"
#include "Route.hpp"
#include "Server.hpp"

class Response {
	public:
		std::string 	protocolVer;
		unsigned int	statCode;
		std::string 	statMsg;
		std::map<std::string, std::string> headers;
		std::string	body;
		bool		ready;

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

		std::string	getStr() {
			std::string res;

			res += this->protocolVer + ' ' + std::to_string(this->statCode) + ' ' + this->statMsg + ' ' + "\r\n";
			for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
				res += it->first + ": " + it->second + "\r\n";
			res += '\n';
			res += this->body;

			if (this->headers["Content-Type"] == "text/html")
				std::cout << "-==-==========" << std::endl << res << std::endl;
			return (res);
		}

		void clean() {
			this->protocolVer = "";
			this->statMsg = "";
			this->headers.clear();
			this->body = "";
			this->ready = false;
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
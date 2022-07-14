#pragma once

#include "Response.hpp"

class Request {
	public:
		std::string 						method;
		std::string							path;
		std::map<std::string, std::string>	attributes;
		std::string							protocolVer;
		std::map<std::string, std::string>	headers;
		
		std::string							body;
		std::string							file;
		Response							res;

		Request(std::string s) {
			std::vector<std::string> req = split(s, "\r\n");

			// head
			std::vector<std::string> head = split(req.at(0), " ");
			this->method = head.at(0);
			std::vector<std::string> pq = split(head.at(1), "?");
			this->path = pq.at(0);
			this->attributes.empty();
			if (pq.size() > 1) {
				std::vector<std::string> query = split(pq.at(1), "&");
				for (std::vector<std::string>::iterator it = query.begin(); it != query.end(); it++) {
					std::vector<std::string> kv = split(*it, "=");
					this->attributes.insert(std::pair<std::string, std::string>(kv.at(0), kv.at(1)));
				}
			}
			this->protocolVer = head.at(2);

			// headers
			std::vector<std::string>::iterator it;
			for (it = req.begin() + 1; it != req.end() && it->find(":") != std::string::npos; it++) {
				std::vector<std::string> kv = split(*it, ":");
				this->headers.insert(std::pair<std::string, std::string>(trim(kv.at(0), WHITESPACE), trim(kv.at(1), WHITESPACE)));
			}

			// Body
			while (it != req.end())
				this->body = *(it++) + "\r\n";

			// Error check
			//parseErrorCheck();

		}

		Request(Request const & other) {
			*this = other;
		}

		~Request() { }

		Request & operator=(Request const & rhs) {
			this->method = rhs.method;
			this->path = rhs.path;
			this->headers = rhs.headers;
			this->body = rhs.body;
			return (*this);
		}

		/*bool	parseErrorCheck() const {
			if (!this->method.length() || !this->path.length() || !this->protocolVer.length()
				|| (this->method != "GET" && this->method != "POST" && this->method != "DELETE")
				|| this->protocolVer != "HTTP/1.1")
				return (false);
			if (!this->headers.size())
				return (false);
			if (this->method == "GET" && this->body.size() > 0)
				;
			else if (this->method == "POST") { // TODO: Need a way to handle chunked requests
				return ;
			}
			return (true);
		}*/
};

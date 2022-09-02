#pragma once

#include "Response.hpp"

typedef unsigned int methode_type;
#define GET 0;
#define POST 1;
#define DELETE 2;

class Request {
	private:

	// Struct to help manage the parse and keep a cleaner Request
	struct Content {
		string raw;
		string boundary;
		int			expected;
		bool		isMultipart;
		bool		isChunked;
		bool		isFullyParsed;

		void initialize(const_string &data, const std::map<string, string> &headers) {
			std::map<string, string>::const_iterator iter;

			// Look for Content-Length to know how many byte we are waiting for
			if ((iter = headers.find("Content-Length")) != headers.end())
				expected = atoi(iter->second.c_str());

			// Look for Content-Type to manage the multipart / boundary
			// More information: https://stackoverflow.com/questions/3508338/what-is-the-boundary-in-multipart-form-data
			if ((iter = headers.find("Content-Type")) != headers.end()) {
				std::vector<string> line = split(iter->second, ";");
				if (line[0] == "multipart/form-data" && line.size() == 2) {
					isMultipart = true;
					trim(line[1], WHITESPACE);
					if (start_with(line[1], "boundary="))
						boundary = line[1].substr(9);
				}
			}

			// Check if the Request is chunked
			if ((iter = headers.find("Transfer-Encoding")) != headers.end()) {
				std::vector<string> fields = split(iter->second, ", ");
				if (includes(fields, string("chunked")))
					isChunked = true;
			}

			parseByte(data);
		}

		// If chunked, parse the chunk
		// Otherwise, apprend the data and check if we do not expect other byte
		// Technically, that helps to check if Content Length is actually the real one 
		void parseByte(const_string &data) {
			if (isChunked) {
				if (parseChunk(data)) isFullyParsed = true;
				return;
			}
			expected -= data.size();
			raw += data;
			if (!(expected > 0)) isFullyParsed = true;
		}

		// To parse chunks, we take lines 1 by 1 from the data stringstream
		// When there's no more chunkSize, the data is fullyParsed
		bool parseChunk(const_string &chunkedData) {
			std::stringstream	ss(chunkedData);
			string line;
			std::getline(ss, line);
			size_t chunkSize = strtol(line.c_str(), NULL, 16);
			size_t currentChunkSize = 0;

			while (std::getline(ss, line)) {
				if (!chunkSize) return true;
				currentChunkSize += line.length() + 1;
				if (chunkSize > currentChunkSize) {
					line = trim(line, "\r");
					line += "\n";
					raw += line;
				}
				if (currentChunkSize == chunkSize && std::getline(ss, line)) {
					chunkSize = strtol(line.c_str(), NULL, 16);
					currentChunkSize = 0;
				}
			}
			return false;
		}

		Content() : expected(0), isMultipart(false), isChunked(false), isFullyParsed(false) {}
		~Content() {}
	};

	public:
		std::string 						method;
		std::string							path;
		std::map<std::string, std::string>	attributes;
		std::string							protocolVer;
		std::map<std::string, std::string>	headers;
		std::string							body;
		std::string							file;
		Content				            	content;
		Response							res;
		uintptr_t							ident;

		std::string							rawContent;

		Request() {}
		Request(uintptr_t id) : ident(id) {}
		~Request() { }

		void putRequest(std::string s) {
			std::vector<std::string> getb = split(s, "\n\n");
			std::vector<std::string> req = split(s, "\r\n");

			// head 
			std::vector<std::string> head = split(req.at(0), " ");
			if (head.at(0) != "GET" && head.at(0) != "POST" && head.at(0) != "DELETE") {
				this->method = "";
				return ;
			}
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
		}

		Request & operator=(Request const & rhs) {
			this->method = rhs.method;
			this->path = rhs.path;
			this->headers = rhs.headers;
			this->body = rhs.body;
			return (*this);
		}

		void putResponse(std::map<std::string, Route> routes) {

			this->res.protocolVer = "HTTP/1.1";
			std::map<std::string, Route>::iterator it = routes.find(this->path);
			if (it != routes.end()) {
				this->res.statCode = Ok;
				this->res.statMsg = "OK";
				this->putResBody(routes[path]);
			} else if (exist("www" + path)) {
				this->res.statCode = Ok;
				this->res.statMsg = "OK";
				this->putResBody("www" + path);
			} else if (exist("www/cgi" + split(path, "?").at(0))) {
				this->res.statCode = Ok;
				this->res.statMsg = "OK";
				this->putResBody("www/cgi" + split(path, "?").at(0));
			} else {
				this->res.statCode = NotFound;
				this->res.statMsg = statusCodeToString(NotFound);
				this->putResBody("www/error_pages/error_404.html");
			}
			this->res.ready = true;
		}

		void	putResBody(Route &route) {
			std::cout << route << std::endl;
			std::ifstream f(route.root + "/" + route.index);
			std::stringstream buf;

			buf << f.rdbuf();
			res.body = buf.str();
			std::cout << buf.str() << std::endl;
			res.headers["Content-Length"] = std::to_string(res.body.length());
			res.headers["Content-Type"] = split(this->headers["Accept"], ",").at(0);
		}

		void	putResBody(std::string fName) {
			std::ifstream f(fName);
			std::stringstream buf;

			//std::cout << "fs" << std::endl;
			buf << f.rdbuf();
			res.body = buf.str();
			res.headers["Content-Length"] = std::to_string(res.body.length());
			res.headers["Content-Type"] = split(this->headers["Accept"], ",").at(0);
			//std::cout << this->headers["Content-Type"] << "sf" << std::endl;
		}

		void clean() {
			this->method = "";
			this->path = "";
			this->attributes.clear();
			this->protocolVer = "";
			this->headers.clear();
			this->body = "";
			this->file = "";
			this->res.clean();
		}

		void putContent(std::string s) {
			this->rawContent += s;
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

		void setIdent(uintptr_t id) {
			this->ident = id;
		}
};

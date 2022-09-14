#pragma once

#include "Response.hpp"

class Request {
	private:
	// Struct to help manage the parse and keep a cleaner Request
	struct Content {
		string 		raw, boundary;
		bool		isMultipart, isChunked, isFullyParsed;
		int			expected;

		void initialize(const_string &data, const std::map<string, string> &headers) {
			std::map<string, string>::const_iterator iter;

			// Look for Content-Length to know how many byte we are waiting for
			if (!expected && (iter = headers.find("Content-Length")) != headers.end())
				expected = atoi(iter->second.c_str());

			// Look for Content-Type to manage the multipart / boundary
			// More information: https://stackoverflow.com/questions/3508338/what-is-the-boundary-in-multipart-form-data
			if ((iter = headers.find("Content-Type")) != headers.end()) {
				std::vector<string> line = split(iter->second, ";");
				if (line[0] == "multipart/form-data" && line.size() == 2) {
					isMultipart = true;
					string tmp = trim(line[1], WHITESPACE);
					if (start_with(tmp, "boundary="))
						boundary = tmp.substr(9);
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

		void clean() {
			this->expected = 0;
			this->isMultipart = false;
			this->isChunked = false;
			this->isFullyParsed = false;
			this->raw = "";
			this->boundary = "";
		}

		Content() : isMultipart(false), isChunked(false), isFullyParsed(false), expected(0) {}
		~Content() {}
	};

	public:
		string 						method, path, protocolVer, body, file, root;
		std::map<string, string>	attributes, headers;
		Content						content;
		Response					res;
		uintptr_t					ident;

		Request() : method(""), path(""), protocolVer(""), body(""), file("") {
			this->content = Content();
		}
		explicit Request(uintptr_t id, string root="") : method(""), path(""), protocolVer(""), body(""), file(""), root(root), ident(id) { this->content = Content(); }
		~Request() {}

		int parseRequest(string s) {
			std::vector<string> req = split(s, "\r\n");
			std::vector<string> head = split(req.at(0), " ");
			if (head.size() != 3)
				return (400);
			this->method = head.at(0);
			if (method != "GET" && method != "POST" && method != "DELETE")
				return (501);
			this->protocolVer = head.at(2);
			if (protocolVer != "HTTP/v1.1")
				return (501);
			std::vector<string> pq = split(head.at(1), "?");
			this->path = pq.at(0);
			this->attributes.empty();
			if (pq.size() > 1) {
				std::vector<string> query = split(pq.at(1), "&");
				for (std::vector<string>::iterator it = query.begin(); it != query.end(); it++) {
					std::vector<string> kv = split(*it, "=");
					this->attributes.insert(std::pair<string, string>(kv.at(0), kv.at(1)));
				}
			}

			// headers
			std::vector<string>::iterator it;
			for (it = req.begin() + 1; it != req.end() && it->find(":") != string::npos; it++) {
				std::vector<string> kv = split(*it, ":");
				this->headers.insert(std::pair<string, string>(trim(kv.at(0), WHITESPACE), trim(kv.at(1), WHITESPACE)));
			}
			while (it != req.end()) // je vais tester
				this->body += *it++ + "\r\n";
<<<<<<< HEAD

=======
>>>>>>> origin/avant
			return (200);
		}

		void redirect(string re) {
			this->res.protocolVer = "HTTP/1.1";
			this->res.status = statusCodeToString(MovedPermanently);
			this->res.headers["Location"] = re;
			this->res.ready = true;
		}

		void putResponse(std::map<string, Route> routes) {
			this->res.protocolVer = "HTTP/1.1";
			std::map<string, Route>::iterator it = routes.find(this->path);
			if (it != routes.end()) {
				if (routes[this->path].redirect.length()) {
					redirect(routes[this->path].redirect);
					return ;
				}
				this->res.status = statusCodeToString(Ok);
				this->putResBody(routes[path]);
			} else if (exist(this->root + this->path)) {
				this->res.status = statusCodeToString(Ok);
				this->putResBody(this->root + path);
			} else
				putCustomError(404);
			this->res.ready = true;
		}

		void	putResBody(Route &route) {
			std::ifstream f(route.root + "/" + route.index);
			std::stringstream buf;

			buf << f.rdbuf();
			res.body = buf.str();
			res.headers["Content-Length"] = std::to_string(res.body.length());
			res.headers["Content-Type"] = mime(route.index);
		}

		void	putResBody(string fName) {
			std::ifstream f(fName);
			std::stringstream buf;

			buf << f.rdbuf();
			res.body = buf.str();
			res.headers["Content-Length"] = std::to_string(res.body.length());
			res.headers["Content-Type"] = mime(fName);
		}

		void	putAutoIndexRes(const_string &page) {
			res.protocolVer = "HTTP/1.1";
			res.status = statusCodeToString(Ok);
			res.body = page;
			res.headers["Content-Length"] = std::to_string(res.body.length());
			res.headers["Content-Type"] = "text/html";
			res.ready = true;
		}

		void	putCustomError(int code) {
			string	file = replace_all_occurrency(replace_all_occurrency(custom_error, "$NAME", statusCodeToString(code)), "$CODE", to_string(code));
			res.protocolVer = "HTTP/1.1";
			res.status = statusCodeToString(code);
			res.body = file;
			res.headers["Content-Length"] = std::to_string(res.body.length());
			res.headers["Content-Type"] = "text/html";
			res.ready = true;
		}

		void	postContent(Request &req, Server & server) {
			std::vector<string> multiportData = split(req.content.raw, req.content.boundary);
			std::vector<string> fileData = split(multiportData[2], "\r\n");
			string fileName = trim(split(fileData[0], ";")[2].substr(11), "\""); // i know it's not perfect but come on
			string fileContent;
			unsigned int fileSize = 0;
			for (size_t i = 2; i < fileData.size(); i++) {
				fileContent += fileData[i];
				fileSize += fileData[i].length();
				if (fileSize > 1024) { // Need the route
					server.findCodeHandler(413, req);
					return ;
				}
			}

		    std::ofstream out("www/cgi/upload/" + fileName); // Need the route
			out << fileContent;

			res.protocolVer = "HTTP/1.1";
			res.status = statusCodeToString(Ok);
			res.body = "file upload success";
			res.headers["Content-Length"] = std::to_string(res.body.length());
			res.headers["Content-Type"] = "text/html";
			res.ready = true;
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
			this->content.clean();
		}
};

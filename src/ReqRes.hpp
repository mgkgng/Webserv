#pragma once

#include <string>
#include <map>

#include "utility.hpp"

class Server;

class Response {
	public:
		string 						body, status, protocolVer;
		std::map<string, string>	headers;
		bool						ready;
		size_t						sendBits;
		int							cgi_fd;
		pid_t						cgi_pid;

		Response() : ready(false), sendBits(0), cgi_fd(0), cgi_pid(0) {}
		~Response() {}

		string	getStr();

		void clean();

		void replaceAutoIndexBody(string &body, const_string &url, char *date, struct stat &statistics, struct dirent &current_directory) {
			body = replace_all_occurrency(autoidx_body, "$NAME", current_directory.d_name);
			body = replace_all_occurrency(body, "$URL", url + "/" + current_directory.d_name);
			body = replace_all_occurrency(body, "$DATE", string(date));
			body = replace_all_occurrency(body, "$SIZE", file_size(statistics.st_size));
			body = replace_all_occurrency(body, "$ISDIR", statistics.st_mode & S_IFDIR ? "1" : "");
		}

		// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
		// Basically we look into our page, take the top part, then parse all the file in a while
		// For each file found the "body" part will be appended with the needed data
		// Then we append the bottom and the html page is finished
		string putAutoIndex(const_string & url, const_string & path, string & file);

		void readFromFd();
};



class Request {
	private:
	// Struct to help manage the parse and keep a cleaner Request
	struct Content {
		string 		raw, boundary;
		bool		isMultipart, isChunked, isFullyParsed;
		int			expected;

		void initialize(const_string &data, const std::map<string, string> &headers);

		// If chunked, parse the chunk
		// Otherwise, apprend the data and check if we do not expect other byte
		// Technically, that helps to check if Content Length is actually the real one 
		void parseByte(const_string &data);

		// To parse chunks, we take lines 1 by 1 from the data stringstream
		// When there's no more chunkSize, the data is fullyParsed
		bool parseChunk(const_string &chunkedData);

		void clean();

		Content() : isMultipart(false), isChunked(false), isFullyParsed(false), expected(0) {}
		~Content() {}
	};

	public:
		string 						method, path, protocolVer, body, file, root;
		std::map<string, string>	attributes, headers;
		Content						content;
		Response					res;
		uintptr_t					ident;
		bool						empty;

		Request() : method(""), path(""), protocolVer(""), body(""), file(""), empty(true) {
			this->content = Content();
		}
		explicit Request(uintptr_t id, string root="") : method(""), path(""), protocolVer(""), body(""), file(""), root(root), ident(id), empty(true) { this->content = Content(); }
		~Request() {}

		int parseRequest(string s);

		void redirect(string re);

		void putResponse(Route & route, const_string & file);

		void putResBody(Route & route, const_string & file);

		void putAutoIndexRes(const_string &page);

		bool getContent(string raw, Server & server);
		
		void putCustomError(int code);

		void postContent(Server & server, Route & route);

		void clean();
};

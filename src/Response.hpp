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

		size_t	totalBits;
		size_t	currBits;

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
			std::cout << buf.str() << std::endl;
			this->headers["Content-Length"] = std::to_string(this->body.length());
			this->headers["Content-Type"] = "text/html";
		}

		void	putBody(std::string fName, std::map<std::string, std::string> reqHeaders) {
			std::ifstream f(fName);
			std::stringstream buf;

			//std::cout << "fs" << std::endl;
			buf << f.rdbuf();
			this->body = buf.str();
			this->headers["Content-Length"] = std::to_string(this->body.length());
			this->headers["Content-Type"] = split(reqHeaders["Accept"], ",").at(0);
			//std::cout << this->headers["Content-Type"] << "sf" << std::endl;
		}

		void	putBody(std::string fName) {
			std::ifstream f(fName);
			std::stringstream buf;

			buf << f.rdbuf();
			this->body = buf.str();
			this->headers["Content-Length"] = std::to_string(this->body.length());
			this->headers["Content-Type"] = "text/html";		
		}

		void putResponse(std::string path, std::map<std::string, std::string> reqHeaders, std::map<std::string, Route> routes) {

			this->protocolVer = "HTTP/1.1";
			std::map<std::string, Route>::iterator it = routes.find(path);
			if (it != routes.end()) {
				this->statCode = Ok;
				this->statMsg = "OK";
				this->putBody(routes[path]);
			} else if (exist("www" + path)) {
				this->statCode = Ok;
				this->statMsg = "OK";
				this->putBody("www" + path, reqHeaders);
			} else if (exist("www/cgi" + split(path, "?").at(0))) {
				this->statCode = Ok;
				this->statMsg = "OK";
				this->putBody("www/cgi" + split(path, "?").at(0), reqHeaders);
			} else {
				this->statCode = NotFound;
				this->statMsg = statusCodeToString(NotFound);
				this->putBody("www/error_pages/error_404.html", reqHeaders);
			}
		}

		void putResponse(unsigned int errCode) {
			this->statCode = errCode;
			this->statMsg = statusCodeToString(errCode);
			this->putBody("www/error_pages/error_404.html");
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

		void replaceAutoIndexBody(string &body, const_string &url, char *date, struct stat &statistics, struct dirent &current_directory) {
			body = replace_all_occurrency(autoidx_body, "$NAME", current_directory.d_name);
			body = replace_all_occurrency(body, "$URL", url + current_directory.d_name),
			body = replace_all_occurrency(body, "$DATE", string(date)),
			body = replace_all_occurrency(body, "$SIZE", file_size(statistics.st_size)),
			body = replace_all_occurrency(body, "$ISDIR", statistics.st_mode & S_IFDIR ? "1" : "");
		}

		// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
		// Basically we look into our page, take the top part, then parse all the file in a while
		// For each file found the "body" part will be appended with the needed data
		// Then we append the bottom and the html page is finished
		void putAutoIndex(const std::map<int, string> &status, const_string &url, const_string &path) {
			DIR *directory;
			// https://stackoverflow.com/questions/12991334/members-of-dirent-structure
			struct dirent *current_directory;
			string tmp, autoidxpage = autoidx_head;

			if (!(directory = opendir(path.c_str())))
				return (void)(status); // To do: Response set error, technically
			while ((current_directory = readdir(directory))) {
				if (!strcmp(current_directory->d_name, ".") || (!strcmp(current_directory->d_name, "..") && url == "/"))
					continue;
				
				// https://pubs.opengroup.org/onlinepubs/007908799/xsh/sysstat.h.html
				struct stat statistics;
				char date[128];
				stat((path + "/" + current_directory->d_name).c_str(), &statistics);
				strftime(date, 128, "%d %h %Y", localtime(&statistics.st_ctime));
				replaceAutoIndexBody(tmp, url, date, statistics, *current_directory);
				autoidxpage += tmp;
			}
			closedir(directory);
			autoidxpage += autoidx_bottom;
			// Here should set:
			// Header: 200 as status, autoidx.size() as size, "text/html" as type
			// Body: autoidx as body
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
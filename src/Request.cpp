/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrathelo <student.42nice.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:36:24 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/30 12:15:14 by jrathelo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Webserv.hpp>
#include <algorithm>
#include <fstream>

#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>

using namespace Webserv;

struct sortByComplex { 
	inline bool operator() (const Webserv::Route & struct1, const Webserv::Route & struct2) {
        return (struct1.getPath().length() > struct2.getPath().length());
    }
}sortByComplex;

std::string find_extension(std::string path) {
	std::string temp = path;
	while (temp.find(".") != std::string::npos) {
		temp.erase(0, temp.find(".") + 1);
	}
	if (path.length() == temp.length()) {
		return "";
	}
	if (temp.find('/') != std::string::npos) {
		return "";
	}
	return temp;
}

std::string get_file_from_path(std::string path, std::string route) {
	std::string ret = path.substr(route.length());
	if ((*ret.rbegin()) == '/') {
		ret.pop_back();
	} 
	return ret;
}

std::string get_file_full_path(std::string requested_file, std::string root) {
	char buf [MAXPATHLEN];
	getcwd(buf, MAXPATHLEN);
	if (root.find('.') == 0 && root.find('/') == 1) {
		root.replace(0, 1, buf);
	} else if (root.find('/') != 0) {
		root.insert(0, 1, '/');
		root.insert(0, buf);
	}
	if (root.find('/', root.length() - 1) == std::string::npos && requested_file.find('/') != 0) {
		root.push_back('/');
	}
	return root + requested_file;
}

bool check_if_file_exists(const std::string name) {
    std::fstream f(name.c_str());
    bool ret = f.good();
	f.close();
	return ret;
}

bool check_if_file_is_dir(const std::string name) {
   struct stat statbuf;
   if (stat(name.c_str(), &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

void	do_request_depending_on_file_type(const std::string file, const Webserv::Route & it) {
	if (check_if_file_exists(file)) {
		std::cout << "REQUESTED INDEX FILE" << std::endl;
	} else if (check_if_file_is_dir(file)) {
		if (it.getListingDirectory()) {
			std::cout << "REQUESTED DIRECTORY" << std::endl;
		} else if (it.getDirectoryFile() != "") {
			if (check_if_file_exists(get_file_full_path(it.getDirectoryFile(), it.getRoot()))) {
				std::cout << "REQUESTED FILEDIR" << std::endl;
			} else {
				throw Webserv::Request::ERROR404();
			}
		} else {
			throw Webserv::Request::ERROR403();
		}
	} else {
		throw Webserv::Request::ERROR404();
	}
}

bool endsWith(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

Request::Request(std::string request, std::vector<Webserv::Server> & server) {
	this->parseRequest(request);

	std::vector<Webserv::Server>::iterator it_s = server.begin();
	for (std::vector<Webserv::Server>::iterator it_s = server.begin(); it_s != server.end(); it_s++) {
		try {
			std::string temp = this->headers.at("Host");
			if (this->headers.at("Host").find(":") != std::string::npos) {
				temp.erase(temp.find(":")); 
			}
			if ((*it_s->getHost().begin()) == '.') {
				if (!endsWith(temp, it_s->getHost())) {
					throw Webserv::Request::ERROR400();
				} else {
					break;
				}
			} else {
				if (temp != it_s->getHost()) {
					throw Webserv::Request::ERROR400();
				} else {
					break;
				}
			}
		} catch (std::out_of_range & e) {
			throw Webserv::Request::ERROR400();
		} catch (Webserv::Request::ERROR400 & e) {
			if (it_s + 1 == server.end()) {
				throw Webserv::Request::ERROR400();
			}
		}
	}

	std::map<std::string, Webserv::Route> route = it_s->getRoutes();
	std::vector<Webserv::Route> matches;
	for (std::map<std::string, Route>::iterator it = route.begin(); it != route.end(); it++ ) {
		if (this->path.substr(0, it->second.getPath().length()) == it->second.getPath()) {
			matches.push_back(it->second);
		}
	}

	// Sort in order from longest to shortest.
	std::sort(matches.begin(), matches.end(), sortByComplex);
	std::vector<Webserv::Route>::iterator it = matches.begin();
	std::vector<std::string> a = it->getAllowedHTTPMethods();
	if (std::find(a.begin(), a.end(), this->method) == a.end()) {
		throw ERROR405();
	} else if (this->method == "GET") {
		if (this->path.length() == it->getPath().length()) {
			std::string file = get_file_full_path(it->getIndex(), it->getRoot());
			do_request_depending_on_file_type(file, *it);
		} else {
			std::string extension = find_extension(this->path);
			std::string file = get_file_from_path(this->path, it->getPath());
			std::cout << file << std::endl;
			file = get_file_full_path(file, it->getRoot());
			std::cout << file << std::endl;
			if (extension == "") {
				do_request_depending_on_file_type(file, *it);
			} else if (extension == it->getPHPCGIExtension()) {
				do_request_depending_on_file_type(file, *it);
			} else if (extension == it->getPythonCGIExtension()) {
				do_request_depending_on_file_type(file, *it);
			} else {
				do_request_depending_on_file_type(file, *it);
			}
		}
	} else if (this->method == "POST") {

	} else if (this->method == "DELETE") {
		
	}
}


Request::Request(Request const & other) {
	*this = other;
}

Request::~Request() { }

Request & Request::operator=(Request const & rhs) {
	this->method = rhs.method;
	this->path = rhs.path;
	this->headers = rhs.headers;
	this->body = rhs.body;
	return (*this);
}

const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string & s) {
	return rtrim(ltrim(s));
}

void	Request::parseRequest(std::string request) {
	// Head
	this->method = request.substr(0, request.find(" "));
	request.erase(0, request.find(" ") + 1);
	this->path = request.substr(0, request.find(" "));
	request.erase(0, request.find(" ") + 1);
	if (this->path.find("?") != std::string::npos) {
		std::string temp = this->path;
		this->path = this->path.substr(0, this->path.find("?"));
		temp = temp.substr(temp.find("?")+1);
		while (temp.size() != 0) {
			std::string name = temp.substr(0, temp.find("="));
			temp.erase(0, temp.find("=") + 1);
			std::string content = temp.substr(0, temp.find("&"));
			if (temp.find("&") == std::string::npos) {
				temp.clear();
			} else {
				temp.erase(0, temp.find("&") + 1);
			}
			this->attributes.insert(
				std::pair<std::string, std::string>(
					name,
					content
			));	
		}
	} else {
		this->attributes.empty();
	}
	this->protocol_v = request.substr(0, request.find("\r\n"));
	request.erase(0, request.find("\r\n") + 2);

	if (this->method.find("\r\n") != std::string::npos) {
		throw ERROR400();
	}
	if (this->path.find("\r\n") != std::string::npos) {
		throw ERROR400();
	}
	if (this->protocol_v.find("\r\n") != std::string::npos) {
		throw ERROR400();
	}
	if (this->method != "GET" && this->method != "POST" && this->method != "DELETE") {
		throw ERROR400();
	}
	if (this->protocol_v != "HTTP/1.1") {
		throw ERROR400();
	}

	// Headers
	while (request.find("\r\n") != 0) {
		if (request.find (":") == std::string::npos) {
			throw ERROR400();
		}
		std::string name = request.substr(0, request.find(":"));
		request.erase(0, request.find(":") + 1);
		std::string content = request.substr(0, request.find("\r\n"));
		// Might keep this, might not. Nginix just doesn't respond.
		// if (content.find(":") != std::string::npos) {
		// 	throw ERROR400();
		// }
		request.erase(0, request.find("\r\n") + 2);
		this->headers.insert(
			std::pair<std::string, std::string>(
				trim(name),
				trim(content)
		));
	}

	request.erase(0, request.find("\r\n") + 2);

	// Body
	this->body = trim(request);
	if (this->method == "GET" && this->body.size() > 0) {
		throw ERROR400();
	}
}

std::string Request::getMethod() const {
	return method;
}

std::string Request::getBody() const {
	return body;
}

std::map<std::string, std::string> Request::getHeaders() const {
	return headers;
}
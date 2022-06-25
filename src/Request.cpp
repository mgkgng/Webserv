/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrathelo <student.42nice.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:36:24 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/25 18:28:47 by jrathelo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Webserv.hpp>
#include <algorithm>
#include <fstream>

// #include <unistd.h>
// #include <sys/param.h>

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
	return path.substr(route.length());
}

std::string get_file_full_path(std::string requested_file, std::string root) {
	// need a ++98 way of getting the path of the working dir
	// char buf [MAXPATHLEN];
	// getcwd(buf, MAXPATHLEN);
	// if (root.find('.') == 0 && root.find('/') == 1) {
	// 	root.replace(0, 1, buf);
	// } else if (root.find('/') != 0) {
	// 	root.insert(0, 1, '/');
	// 	root.insert(0, buf);
	// }
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

Request::Request(std::string request, Webserv::Server & server) {
	this->parseRequest(request);
	std::map<std::string, Webserv::Route> route = server.getRoutes();
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
		std::cout << "DISALLOWED METHOD: ERROR 405" << std::endl;
	} else if (this->method == "GET") {
		if (this->path.length() == it->getPath().length()) {
			std::string file = get_file_full_path(it->getIndex(), it->getRoot());
			if (check_if_file_exists(file)) {
				std::cout << "REQUESTED INDEX FILE" << std::endl;
			} else {
				std::cout << "ERROR 404: FILE NOT FOUND" << std::endl;
			}
		} else {
			std::string extension = find_extension(this->path);
			std::string file = get_file_from_path(this->path, it->getPath());
			file = get_file_full_path(file, it->getRoot());
			std::cout << check_if_file_exists(file) << std::endl;
			if (extension == "") {
				std::cout << "REQUESTED EXTENSONLESS FILE OR DIRECTORY" << std::endl;
				if (check_if_file_exists(file)) {
					std::cout << "REQUESTED INDEX FILE" << std::endl;
				} else {
					std::cout << "ERROR 404: FILE NOT FOUND" << std::endl;
				}
			} else if (extension == it->getPHPCGIExtension()) {
				std::cout << "REQUESTED PHP CGI FILE" <<  std::endl;
				if (check_if_file_exists(file)) {
					std::cout << "REQUESTED INDEX FILE" << std::endl;
				} else {
					std::cout << "ERROR 404: FILE NOT FOUND" << std::endl;
				}
			} else if (extension == it->getPythonCGIExtension()) {
				std::cout << "REQUESTED PYTHON CGI FILE" << std::endl;
				if (check_if_file_exists(file)) {
					std::cout << "REQUESTED INDEX FILE" << std::endl;
				} else {
					std::cout << "ERROR 404: FILE NOT FOUND" << std::endl;
				}
			} else {
				std::cout << "REQUESTED FILE" << std::endl;
				if (check_if_file_exists(file)) {
					std::cout << "REQUESTED INDEX FILE" << std::endl;
				} else {
					std::cout << "ERROR 404: FILE NOT FOUND" << std::endl;
				}
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

	// Headers
	while (request.find("\r\n") != 0) {
		std::string name = request.substr(0, request.find(":"));
		request.erase(0, request.find(":") + 1);
		std::string content = request.substr(0, request.find("\r\n"));
		request.erase(0, request.find("\r\n") + 2);
		this->headers.insert(
			std::pair<std::string, std::string>(
				name,
				content
		));
	}
	request.erase(0, request.find("\r\n") + 2);

	// Body
	this->body = request;
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
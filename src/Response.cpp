/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrathelo <student.42nice.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 16:51:56 by min-kang          #+#    #+#             */
/*   Updated: 2022/07/05 14:55:44 by jrathelo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Webserv.hpp>
#include <fstream>

#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

using namespace Webserv;

Response::Response() { }

Response::Response(std::string file) {
	//gen headers

	std::fstream f(file.c_str());
	std::ostringstream sstr;
	sstr << f.rdbuf();
	this->body = sstr.str();
}

Response::Response(DIR * directory, std::string route) {
	//gen headers

	//gen body
	struct dirent *diritem = readdir(directory);
	this->body = "<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n<meta charset=\"utf-8\" />\r\n</head>\r\n<body>\r\n<ul>\r\n";
	while (diritem != NULL) {
		this->body.append("<il>\r\n<a href=\"");
		this->body.append(route);
		if (!endsWith(route, "/")) {
			this->body.append("/");
		}
		this->body.append(diritem->d_name);
		this->body.append("\">");
		this->body.append(diritem->d_name);
		this->body.append("</a>\r\n</il>\r\n");
		diritem = readdir(directory);
	}
	closedir(directory);
	this->body.append("</ul>\r\n</body>\r\n</html>");
	std::cout << this->body << std::endl;
}

Response::Response(Response const & other) {
	*this = other;
}

Response::~Response() {
	
}

Response & Response::operator=(Response const & rhs) {
	this->protocol_v = rhs.protocol_v;
	this->status_code = rhs.status_code;
	this->status_message = rhs.status_message;
	this->headers = rhs.headers;
	return (*this);
}

std::string Response::makeResponseStr() {
	std::stringstream ss;
	
	ss << protocol_v << ' ' << status_code << ' ' << status_message << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		ss << it->first << ": " << it->second << std::endl;
	ss << std::endl;
	return (ss.str());
}

Response	Webserv::gen_response_directory(const Webserv::Request & request, const std::string directory, const Webserv::Route & route) {
	(void) request;
	if (route.getListingDirectory()) {
		if (check_if_file_is_dir(directory)) {
			return Response(opendir(directory.c_str()), request.getPath());
		} else {
			throw Request::ERROR403();
		}
	} else {
		if (route.getDirectoryFile() == "") {
			throw Request::ERROR404();
		}
		std::string dirfile =  get_file_full_path(route.getDirectoryFile(), route.getRoot());
		if (check_if_file_exists(dirfile)) {
			// return Response(dirfile);
		} else  {
			throw Request::ERROR404();
		}
	}
	throw Request::ERROR404();
}
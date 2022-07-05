/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrathelo <student.42nice.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 16:51:56 by min-kang          #+#    #+#             */
/*   Updated: 2022/07/05 11:12:41 by jrathelo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Webserv.hpp>
#include <Response.hpp>
#include <fstream>

#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>

using namespace Webserv;

Response::Response() { }

Response::Response(std::string file) {
	//gen_headers
	std::fstream f(file.c_str());
	std::ostringstream sstr;
	sstr << f.rdbuf();
	this->body = sstr.str();
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

Response	gen_response_directory(Webserv::Request & request, std::string directory, Webserv::Route & route) {
	if (route.getListingDirectory()) {
		if (check_if_file_is_dir(directory)) {

		} else {
			throw Request::ERROR404();
		}
	} else {
		if (route.getDirectoryFile() == "") {
			throw Request::ERROR404();
		}
		std::string dirfile =  get_file_full_path(route.getDirectoryFile(), route.getRoot());
		if (check_if_file_exists(dirfile)) {
			return Response(dirfile);
		} else  {
			throw Request::ERROR404();
		}
	}
}
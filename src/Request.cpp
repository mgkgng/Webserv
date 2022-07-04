/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:36:24 by min-kang          #+#    #+#             */
/*   Updated: 2022/07/04 13:32:19 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Webserv.hpp>
#include <algorithm>
#include <fstream>

#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>

using namespace Webserv;

Request::Request(std::string s) {
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
	this->protocol_v = head.at(2);

	// headers
	std::vector<std::string>::iterator it;
	for (it = req.begin() + 1; it != req.end() && it->find(":") != std::string::npos; it++) {
		std::vector<std::string> kv = split(*it, ":");
		this->headers.insert(std::pair<std::string, std::string>(trim(kv.at(0)), trim(kv.at(1))));
	}

	// Body
	for (it; it != req.end(); it++)
		this->body = *it + "\r\n";

	// Error check
	parseErrorCheck();
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

std::string	Request::getResponse() {

	
	this->res = Response(this->protocol_v, )

	std::stringstream ss;
	
	ss << protocol_v << ' ' << status_code << ' ' << status_message << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		ss << it->first << ": " << it->second << std::endl;
	ss << std::endl;
	return (ss.str());

	
	if (exist(rname))
		rname = "www/error_pages/error_404.html";
	std::ifstream f(rname);
	std::stringstream buf;
	buf << f.rdbuf();
	return (buf.str());
}

void	Request::parseErrorCheck() const {
	if (!this->method.length() || !this->path.length() || !this->protocol_v.length()
		|| (this->method != "GET" && this->method != "POST" && this->method != "DELETE")
		|| this->protocol_v != "HTTP/1.1")
		throw ERROR400();
	if (!this->headers.size())
		throw ERROR400();
	if (this->method == "GET" && this->body.size() > 0)
		throw ERROR400();
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
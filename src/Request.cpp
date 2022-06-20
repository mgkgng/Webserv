/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:36:24 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/20 13:12:20 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

using namespace Webserv;

Request::Request() {

}

Request::Request(std::string request) {
	parseRequest(request);
}

Request::Request(Request const & other) {
	*this = other;
}

Request::~Request() {
	
}

Request & Request::operator=(Request const & rhs) {
	this->method = rhs.method;
	this->path = rhs.path;
	this->headers = rhs.headers;
	this->body = rhs.body;
	return (*this);
}

void	Request::parseRequest(std::string request) {
	std::istringstream iss;
	std::string line;

	// first line parsing
	getline(iss, line);
	std::vector<std::string> start_line = ft_split(const_cast<char *>(line.c_str()), " \n\r");
	method = start_line.at(0);
	path = start_line.at(1);
	protocol_v = start_line.at(2);
	
	// header parsing
	std::vector<std::string> header_line;
	while (getline(iss, line) && line != "\n\r") { // a voir
		header_line = ft_split(const_cast<char *>(line.c_str()), ":");
		headers.insert(std::pair<std::string, std::string>(header_line.at(0), ft_trim(header_line.at(1), " \n\r")));
	}

	// body parsing
	while (getline(iss, line))
		body += line;
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
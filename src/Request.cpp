/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:36:24 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/24 16:54:27 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

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
	return (*this);
}

void	Request::parseRequest(std::string request) {
	std::vector<std::string> lines = ft_split(const_cast<char *>(request.c_str()), "\n");
	std::vector<std::string> start_line = ft_split(const_cast<char *>(lines.at(0).c_str()), " ");
	method = start_line.at(0);
	path = start_line.at(1);
	protocol_v = start_line.at(2);
	
	std::vector<std::string> line;
	for (int i = 1; i < lines.size(); i++) {
		line = ft_split(const_cast<char *>(lines.at(i).c_str()), ":");
		headers.insert(std::pair<std::string, std::string>(line.at(0), ft_strtrim(line.at(1), " ")));
	}
}
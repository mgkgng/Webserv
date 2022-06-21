/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 16:51:56 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/20 19:47:38 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

using namespace Webserv;

Response::Response() {
	
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

std::string Response::makeResponseStr(Request const & request) {
	(void) request;
	std::stringstream ss;
	ss << protocol_v << ' ' << status_code << ' ' << status_message << std::endl;
	//for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		//ss << it->first() << ": " << it->second() << std::endl;
	ss << std::endl;
	//ss << html comes after
	return (ss.str());
}
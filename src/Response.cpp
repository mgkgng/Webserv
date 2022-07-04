/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 16:51:56 by min-kang          #+#    #+#             */
/*   Updated: 2022/07/04 13:31:23 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

using namespace Webserv;

Response::Response() {
	
}

Response::Response(std::string ptc, std::string code, std::string msg, std::map<std::string, std::string> headers) {
	this->protocol_v = ptc;
	this->status_code = code;
	this->status_message = msg;
	this->headers = headers;
}

Response::Response(Response const & other) {
	this->protocol_v = other.protocol_v;
	this->status_code = other.status_code;
	this->status_message = other.status_message;
	this->headers = other.headers;
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

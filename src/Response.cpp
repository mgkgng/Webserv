/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 16:51:56 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/24 16:57:43 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

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


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 14:53:51 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/24 16:59:12 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

using namespace Webserv;

Client::Client() {
	
}

Client::Client(Client const & other) {
	*this = other; 
}

Client::Client(int fd) : ident(fd) {
	
}

Client::~Client() {
	
}

Client & Client::operator=(Client const & rhs) {
	this->ident = rhs.ident;
	this->requestStr = rhs.requestStr;
	this->request = rhs.request;
	return (*this);
}
			
bool	Client::isClient(int fd) { 
	return (fd == ident) ? true : false; 
}

int		Client::getIdent() const {
	return ident;
}

std::string	Client::getRequestStr() const {
	return requestStr;
}

Request Client::getRequest() const {
	return request;
}
			
Response Client::getResponse() const {
	return response;
}	

void	Client::putRequest() {
	request.parseRequest(requestStr);
}

void	Client::putRequestStr(std::string s) {
	if (!requestStr.length())
		requestStr = s;
	else
		requestStr.append(s);
}
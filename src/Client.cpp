/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 14:53:51 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/20 19:48:54 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

using namespace Webserv;

Client::Client() {
	
}

Client::Client(Client const & other) {
	*this = other; 
}

Client::Client(int fd) : ident(fd), request(NULL), response(NULL) {
	
}

Client::~Client() {
	delete request;
	delete response;
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

std::string Client::getResponseStr() const {
	return responseStr;
}

Request* Client::getRequest() const {
	return request;
}
			
Response* Client::getResponse() const {
	return response;
}	

void	Client::putRequest() {
	request = new Request;
	request->parseRequest(requestStr);
}

void	Client::putRequestStr(std::string s) {
	if (!requestStr.length())
		requestStr = s;
	else
		requestStr.append(s);
}

void	Client::clearRequestStr() {
	requestStr.clear();
}

void	Client::clearReponseStr() {
	responseStr.clear();
}

void	Client::destroyRequest() {
	delete request;
	request = NULL;
}

void	Client::destroyResponse() {
	delete response;
	response = NULL;
}
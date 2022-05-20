/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionData.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 19:33:21 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/20 19:54:28 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

using namespace std;

class ConnectionData {
	private:
		string	requestData;
		string	responseData;
		bool	isRequest;

	public:
		ConnectionData() : isRequest(true) {}
		ConnectionData(ConnectionData const & other) {
			*this = other;
		}
		~ConnectionData() {}
		
		ConnectionData& operator=(ConnectionData const & rhs) {
			this->requestData = rhs.requestData;
			this->responseData = rhs.responseData;
			this->isRequest = rhs.isRequest;
			return (*this);
		}
		
		string	getRequestData() {return requestData;}
		string	putRequestData(string s) {requestData.append(s);}
		string	getResponseData() {return responseData;}
		string	putResponseData(string s) {responseData.append(s);}
		
		bool	getIsRequest() {return isRequest;}
		void	RequestDone() {isRequest = false;}
};
 
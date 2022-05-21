/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 21:09:45 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/21 21:47:56 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

using namespace std;

class Client {
	private:
		int		ident;
		string 	requestMsg;

	public:
		Client();
		Client(Client const & other) {
			*this = other;
		}
		~Client();

		Client & operator=(Client const & right) {
			this->ident = right.ident;
			this->requestMsg = right.requestMsg;
			return (*this);
		}
		
		bool	isClient(int fd) { return (fd == ident) ? true : false; }

		int		getIdent() {
			return ident;
		}
		
		string	getMsg() {
			return requestMsg;
		}
		
		void	putMsg(string s) {
			if (!requestMsg.length())
				requestMsg = s;
			else
				requestMsg.append(s);
		}
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 22:40:38 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/25 17:28:01 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "libft.hpp"

namespace Webserv {

class Request {
	private:
		std::string 						method;
		std::string							path;
		std::string							protocol_v;
		std::string							body;
		std::map<std::string, std::string>	headers;
	public:
		Request();
		Request(std::string);
		Request(Request const &);
		~Request();

		Request & operator=(Request const & rhs);
		
		void	parseRequest(std::string);
		
		std::string	getMethod() const;
		std::string	getBody() const;
		std::map<std::string, std::string> getHeaders() const;
};
};
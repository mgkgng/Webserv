/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 22:40:38 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/24 22:47:53 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

namespace Webserv {

class Request {
	private:
		std::string 						method;
		std::string							path;
		std::map<std::string, std::string>	headers;
	public:
		Request();
		Request(std::string);
		Request(Request const &);
		~Request();

		Request & operator=(Request const & rhs);
		
		void	parseRequest(std::string);
};
};
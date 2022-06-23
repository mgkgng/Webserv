/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrathelo <student.42nice.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 22:40:38 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/22 17:55:41 by jrathelo         ###   ########.fr       */
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

			std::map<std::string, std::string>	headers;
			
			std::string							body;
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 20:25:11 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/24 20:25:45 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

namespace Webserv {

class Response {
	private:
		std::string protocol_v;
		std::string status_code;
		std::string status_message;
		std::map<std::string, std::string> headers;
	public:
		Response();
		Response(Response const &);
		~Response();
		Response & operator=(Response const &);
		std::string makeResponseStr() const;
};
};
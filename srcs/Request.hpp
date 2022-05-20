/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 19:21:59 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/20 19:23:41 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"

using namespace std;

class Request {
	private:
		string content;

	public:
		Request();
		~Request();

		string getContent() {return content;}
		void	putContent(string s);
};
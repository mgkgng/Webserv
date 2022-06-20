/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:42:44 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/20 13:13:20 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// C++ libraries
#include <iostream>
#include <istream>
#include <string>
#include <exception>
#include <cassert>
#include <cerrno>
#include <vector>
#include <thread>
#include <utility>
#include <sstream>
#include <map>
#include <iterator>

// C libraries
#include <sys/socket.h> 
#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <netdb.h>
#include <pthread.h>

std::vector<std::string> ft_split(char *str, char *sep) {
	char *substr;
	std::vector<std::string> res;

	substr = strtok(str, sep);
	while (substr) {
		res.push_back(std::string(substr));
		substr = strtok(str, sep);
	}
	return (res);
}

std::string	ft_trim(std::string s, std::string set) {
	for (std::string::iterator c = s.begin(); c != s.end() && set.find(*c, 0) != std::string::npos; c++)
		s.erase(c);
	for (std::string::iterator c = s.end() - 1; c != s.begin() && set.find(*c, 0) != std::string::npos; c--)
		s.erase(c);
	return (s);
}
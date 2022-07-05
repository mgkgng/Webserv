/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrathelo <student.42nice.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:42:44 by min-kang          #+#    #+#             */
/*   Updated: 2022/07/05 13:00:37 by jrathelo         ###   ########.fr       */
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

std::vector<std::string> ft_split(std::string str, std::string sep);
std::string	ft_trim(std::string s, std::string set);
std::string get_file_full_path(std::string requested_file, std::string root);
bool check_if_file_exists(const std::string name);
bool check_if_file_is_dir(const std::string name);
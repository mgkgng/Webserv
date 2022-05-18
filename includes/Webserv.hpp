/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 17:08:04 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/17 17:39:00 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <istream>
#include <string>
#include <exception>

#include <sys/socket.h> // socket, listen, bind, accept, send, connect, getsockname, setsockopt
#include <sys/event.h> // kqueue, kevent. poll() and select() are deprecated.
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h> // inet_addr
// inet_addr function seems to be deprecated. We can use inet_pton function instead.
// pton stands for "presentation to network" 

#include <arpa/inet.h> // htons, htonl, ntohs, ntohl
#include <fcntl.h> // fcntl
#include <netdb.h> // getaddrinfo

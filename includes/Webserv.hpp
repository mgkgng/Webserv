/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 17:08:04 by min-kang          #+#    #+#             */
/*   Updated: 2022/05/15 18:13:42 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <istream>
#include <string>

#include <sys/socket.h> // socket, listen, bind, accept, send, connect, getsockname, setsockopt
#include <sys/event.h> // kqueue, kevent. poll() and select() are deprecated.
#include <sys/types.h>
#include <sys/select.h> // select
#include <netinet/in.h> // inet_addr
// inet_addr function seems to be deprecated. We can use inet_pton function instead.
// pton stands for "presentation to network" 

#include <arpa/inet.h> // htons, htonl, ntohs, ntohl
#include <fcntl.h> // fcntl
#include <netdb.h> // getaddrinfo

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: min-kang <minguk.gaang@gmail.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 17:08:04 by min-kang          #+#    #+#             */
/*   Updated: 2022/07/11 19:20:29 by min-kang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "utility.hpp"
#include "statusCodes.hpp"
#include <JSON.hpp>

#include "Server.hpp"

std::vector<Server>		makeServersFromJSON(const JSON & json);

void thread_launch(void *ptr) {
	Server *launch;

	launch = reinterpret_cast<Server *>(ptr);
	launch->launch();
}

void start(std::vector<Server> & servers) {
	int	serverNb = servers.size();
	
	if (serverNb == 1)
		servers[0].launch();
	else {
		std::vector<pthread_t> threads(serverNb);

		for (int i = 0; i < serverNb; i++)
			pthread_create(&threads.at(i), NULL, (void * (*)(void *)) &Webserv::thread_launch, &servers.at(i));
		for (int i = 0; i < serverNb; i++)
			pthread_detach(threads.at(i));
	}
	while (1);
}
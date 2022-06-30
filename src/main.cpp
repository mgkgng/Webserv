#include <JSON.hpp>
#include <Webserv.hpp>

#include <iostream>
#include <algorithm>

// Not to be confused with sword logic
bool sortLogic(Webserv::Server i1, Webserv::Server i2)
{
	if (i1.getPort() != i2.getPort()) {
    	return i1.getPort() < i2.getPort();
	} else if (i1.getHost() != i2.getHost()) {
		return i1.getHost() < i2.getHost();
	} else {
		return i1.getIsDefault();
	}
}

int main(int argc, char ** argv) {
	if (argc == 1) {
		try {
			JSON test = JSON("./config/example.json"); 
			std::vector<Webserv::Server> servers = Webserv::makeServersFromJSON(test);
			std::sort(servers.begin(), servers.end(), sortLogic);
			std::vector<Webserv::Server>::iterator it = servers.begin();
			for (; it != servers.end(); it++) {
				std::cout << it->getHost() << ":" << it->getPort() << std::endl;
			}
			Webserv::start(servers);
		} catch (std::exception & e) {
			std::cerr << "Error" << std::endl;
			std::cerr << e.what() << std::endl;
		}
	} else if (argc == 2) {
		try {
			JSON test = JSON(argv[1]);
			std::vector<Webserv::Server> servers = Webserv::makeServersFromJSON(test);
			Webserv::start(servers);
		} catch (std::exception & e) {
			std::cerr << e.what() << std::endl;
		}
	} else {
		std::cerr << "To many arguments!" << std::endl;
	}
}


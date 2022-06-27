#include <JSON.hpp>
#include <Webserv.hpp>

#include <iostream>
#include <algorithm>

int main(int argc, char ** argv) {
	if (argc == 1) {
		try {
			JSON test = JSON("./config/example.json"); 
			std::vector<Webserv::Server> servers = Webserv::makeServersFromJSON(test);
			Webserv::start(servers);
		} catch (std::exception & e) {
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


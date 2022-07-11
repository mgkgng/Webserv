//#include "Webserv.hpp"
//#include "Server.hpp"
#include "ConfigParsing.hpp"

#include <iostream>
int main() {
	std::cout << "bonjour" << std::endl;
}
/*
int main(int argc, char **argv) {
	if (argc > 2)
		return (1);
	try {
		std::string config = (argv[1]) ? argv[1] : "./config/config.json";
		//std::vector<ServerInfo> infos = getServerInfo(JSON(config));
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}*/
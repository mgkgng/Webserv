#include <string>
#include <iostream>

#include "Server.hpp"
#include "Config.hpp"
#include "JSON.hpp"
#include "utility.hpp"

int main(int argc, char **argv) {
	if (argc > 2)
		return (1);
	std::string configFile = (argv[1]) ? argv[1] : "./config/config.json";
	try {
		JSON serverConfig = JSON(configFile);
		Config serverSetting = Config(configFile);
		Server::start(serverSetting.servers);
	} catch (std::exception & e) {
		std::cout << e.what() << std::endl;
		return (2);
	}
}
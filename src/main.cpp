#include "Webserv.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include <fstream>

int main(int argc, char **argv) {
	if (argc > 2)
		return (1);
	std::string configFile = (argv[1]) ? argv[1] : "./config/default.config";

	Config serverSetting = Config(configFile);
	Server::start(serverSetting.servers);
}
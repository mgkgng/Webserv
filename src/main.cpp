#include "utility.hpp"
#include "statusCodes.hpp"
#include "Server.hpp"
#include "ConfigDebug.hpp"

int main(int argc, char **argv) {
	if (argc > 2)
		return (1);
	std::string configFile = (argv[1]) ? argv[1] : "./config/default.config";

	Config serverSetting = Config(configFile);
	// for (std::map<string, Route>::iterator it = serverSetting.servers[0].routes.begin(); it != serverSetting.servers[0].routes.end(); it++)
	// 	std::cout << it->second << std::endl;
	Server::start(serverSetting.servers);
}
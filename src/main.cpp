#include "Webserv.hpp"
#include "Server.hpp"

int main(int argc, char **argv) {
	if (argc > 2)
		return (1);
	std::string config = (argv[1]) ? argv[1] : "./config/config.json";
	std::vector<ServerInfo> infos = getServerInfo(JSON(config));


	/*std::vector<Server> servers;
	for (std::vector<ServerInfo>::iterator it = infos.begin(); it != infos.end(); it++)
		servers.push_back(Server(*it));
	Server::start(servers);*/
}
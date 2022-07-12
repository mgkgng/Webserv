#include "Webserv.hpp"
#include "Server.hpp"

class Route {
	std::string					index;
	std::string					root;
	int							bodySizeLimit;
	std::vector<std::string>	methods;
}

class Config {
	public:

		std::string	port;
		std::string serverName;
		std::map<std::string, Route> routes;

		Config(std::ifstream fs) {
			std::string configFile = getConfigFile(fs);
			
			std::vector<std::string> serverConfig = split(configFile, "@");
			for (std::vector<std::string>::iterator it = serverConfig.begin(); it != serverConfig.end(); it++) {
				parseServer(*it);
			}
		}

		std::string	getConfigFile(std::ifstream fs) {
			std::string res;

			for (std::string line; getline(fs, line); )
				res += line;
			return (res);
		}
		void	parseServer(std::string serverConfig) {
			std::vector<std::string> lines = split(serverConfig, "\n");
			for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
				
			}
		}
		void	parseLocation() {}

		static std::vector<Server> createServer()

}

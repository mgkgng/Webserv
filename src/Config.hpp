#include "Webserv.hpp"
#include "Server.hpp"
#include <string>


class Route {
	public:
		std::string					index;
		std::string					root;
		int							bodySizeLimit;
		std::vector<std::string>	methods;
		bool						autoindex;


		Route();
};

class Config {
	private:
		std::string createDots(int nb) {
			std::string res;
			for (int i = 0; i < nb; i++)
				res += '.';
			return (res);
		}
	public:
		std::vector<Server> servers;

		Config(std::ifstream fs) {
			std::string configFile = getConfigFile(fs);
			
			std::vector<std::string> serverConfig = split(configFile, "@");
			for (std::vector<std::string>::iterator it = serverConfig.begin(); it != serverConfig.end(); it++) {
				this->servers.push_back(parseServer(*it));
			}
		}

		std::string	getConfigFile(std::ifstream fs) {
			std::string res;

			for (std::string line; getline(fs, line); )
				res += line;
			return (res);
		}

		Server	&parseServer(std::string serverConfig) {
			Server	res = new Server();

			std::vector<std::string> lines = split(serverConfig, "\n");
			for (std::vector<std::string>::iterator line = lines.begin(); line != lines.end(); line++) {
				std::vector<std::string> info = split(*line, ":");
				if (info.at(0) == "listen")
					res.port = trim(info.at(1), WHITESPACE);
				else if (info.at(0) == "servername")
					res.serverName = trim(info.at(1), WHITESPACE);
				else if (info.at(0) == "maxbodysize")
					res.maxBodySize = trim(info.at(1), WHITESPACE);
				else if (info.at(0) == "location")
					res.routes[trim(info.at(1), WHITESPACE)] = parseLocation(line, lines, 1);
				else
					std::cerr << "Error: Wrong config file format." << std::endl;
			}
		}
		Route	&parseLocation(std::vector<std::string>::iterator line, std::vector<std::string> lines, int dotNb) {
			Route	res = new Route();

			std::string dots = createDots(dotNb);
			while (line != lines.end() && strncmp((*line).c_str(), dots.c_str(), dotNb)) {
				std::vector<string> info = split((*line).erase(0, dotNb), ":");
				if (info.at(0) == "method")
					res.methods.push_back(trim(info.at(1), WHITESPACE));
				else if (info.at(0) == "root")
					res.root = trim(info.at(1), WHITESPACE);
				else if (info.at(0), "index")
					res.index = trim(info.at(1), WHITESPACE);
				else if (info.at(0), "autoindex" && trim(info.at(1), WHITESPACE) == "on")
					res.autoindex = true;
				/*else if (info.at(0) == "location")
					res.routes = parseLocation(line, lines, dotNb + 1);*/
				else
					std::cerr << "Error: Wrong config file format." << std::endl;
			}
			line--;
		}

		static std::vector<Server> createServer()

}

#pragma once

#include "utility.hpp"
#include "Server.hpp"
#include "Route.hpp"

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

		Config(std::string configFile) {
			std::string config = getConfigFile(configFile);
			
			std::vector<std::string> serverConfig = split(config, "@");
			for (std::vector<std::string>::iterator it = serverConfig.begin(); it != serverConfig.end(); it++) {
				this->servers.push_back(parseServer(*it));
			}
		}

		std::string	getConfigFile(std::string configFile) {
			std::fstream fs(configFile); // should do some error check
			std::string res;

			for (std::string line; getline(fs, line); )
				res += line;
			return (res);
		}

		Server	parseServer(std::string serverConfig) {
			Server	res = Server();

			std::vector<std::string> lines = split(serverConfig, "\n");
			for (std::vector<std::string>::iterator line = lines.begin(); line != lines.end(); line++) {
				std::vector<std::string> info = split(*line, ":");
				if (info.at(0) == "port")
					res.port = std::stoul(trim(info.at(1), WHITESPACE));
				else if (info.at(0) == "servername")
					res.serverName = trim(info.at(1), WHITESPACE);
				else if (info.at(0) == "maxbodysize")
					res.maxBodySize = trim(info.at(1), WHITESPACE);
				else if (info.at(0) == "route")
					res.routes[trim(info.at(1), WHITESPACE)] = parseRoute(line, lines, 1);
				else
					throw Config::InvalidConfig();
			}
			return (res);
		}

		Route	parseRoute(std::vector<std::string>::iterator &line, std::vector<std::string> lines, int dotNb) {
			Route	res = Route();

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
				/*else if (info.at(0) == "route")
					res.routes = parseRoute(line, lines, dotNb + 1);*/
				else
					throw Config::InvalidConfig();
			}
			line--;
			return (res);
		}

		class InvalidConfig: public std::exception { const char * what () const throw () { return "Error: Invalid configuration file format."; } };
};

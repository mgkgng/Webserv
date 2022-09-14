#pragma once

#include "utility.hpp"
#include "Server.hpp"
#include "Route.hpp"
#include "HandleCode.hpp"
#include "JSON.hpp"

class Config {
	private:
		typedef struct sbh_s {
			//server
			std::string		servername;
			std::string		host;
			unsigned int	port;

			//code
			unsigned int	code;
			std::string		redirect;
			unsigned int	responsecode;
				
			// Route
			bool			islistingdirectory;
			std::string		directoryfile;
			bool			isuploadable;
			std::string		uploadroot;
			std::string		index;
			std::string		root;
			std::string		path;
			std::string		clientmaxbodysize;
			std::string		allowedHTTPmethods;
			std::string		pythoncgiextension;
			std::string		perlcgiextextension;
		}	sbh_t;

		sbh_t	getInformation(const JSON & json, sbh_t ret);

		sbh_t defaultInformation();

		Route	generateRoute(const JSON & json, sbh_t sinfo);

		HandleCode	generateHandleCode(const JSON & json, sbh_t sinfo, std::map<std::string, Route> & routes);
	
		std::vector<Server>	makeServersFromJSONHelper(const JSON & json, sbh_t sinfo, std::map<std::string, Route> & routes, std::map<std::string, HandleCode> & codes);
	
	public:
		std::vector<Server> servers;

		Config(const JSON & json);

		class InvalidConfig: public std::exception { const char * what () const throw () { return "Error: Invalid configuration file format."; } };
};

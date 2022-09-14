#include "Config.hpp"
#include "JSON.hpp"
#include "Route.hpp"
#include "Server.hpp"
#include "HandleCode.hpp"

#include <map>
#include <vector>

Config::sbh_t Config::getInformation(const JSON & json, sbh_t ret) {
	try {
		ret.index = json.getStrings().at("index");
	} catch (std::exception & e) { }
	try {
		ret.root = json.getStrings().at("root");
	} catch (std::exception & e) { }
	try {
		ret.allowedHTTPmethods = json.getStrings().at("method");
	} catch (std::exception & e) { }
	try {
		ret.path = json.getStrings().at("path");
	} catch (std::exception & e) { }
	try {
		ret.islistingdirectory = json.getBooleans().at("autoindex");
	} catch (std::exception & e) { }
	try {
		ret.directoryfile = json.getStrings().at("directory_file");
	} catch (std::exception & e) { }
	try {
		ret.isuploadable= json.getBooleans().at("uploadable");
	} catch (std::exception & e) { }
	try {
		ret.uploadroot = json.getStrings().at("upload_root");
	} catch (std::exception & e) { }
	try {
		ret.host = json.getStrings().at("host");
	} catch (std::exception & e) { }
	try {
		ret.port = json.getNumbers().at("port");
	} catch (std::exception & e) { }
	try {
		ret.servername = json.getStrings().at("server_name");
	} catch (std::exception & e) { }
	try {
		ret.code = json.getNumbers().at("code");
	} catch (std::exception & e) { }
	try {
		ret.redirect = json.getStrings().at("redirect");
	} catch (std::exception & e) { }
	try {
		ret.responsecode = json.getNumbers().at("response_code");
	} catch (std::exception & e) { }
	try {
		ret.clientmaxbodysize = json.getStrings().at("client_body_size");
	} catch (std::exception & e) { }
	try {
		ret.pythoncgiextension = json.getStrings().at("python_cgi_extension");
	} catch (std::exception & e) { }
	try {
		ret.perlcgiextextension = json.getStrings().at("perl_cgi_extension");
	} catch (std::exception & e) { }
	return (ret);
}

Config::sbh_t Config::defaultInformation() {
	sbh_t ret;
	ret.index = "";
	ret.root = ".";
	ret.allowedHTTPmethods = "";
	ret.path = "/";
	ret.islistingdirectory = false;
	ret.directoryfile = "";
	ret.isuploadable = false;
	ret.uploadroot = "";
	ret.host = "localhost";
	ret.port = 42069;
	ret.servername = "default";
	ret.code = 0;
	ret.redirect = "";
	ret.responsecode = 0;
	ret.clientmaxbodysize = "5M";
	ret.pythoncgiextension = "";
	ret.perlcgiextextension = "";
	return (ret);
}

Route	Config::generateRoute(const JSON & json, sbh_t sinfo) {
	if (json.getObjects().empty() != 1) {
		throw Config::InvalidConfig();
	}
	sinfo = getInformation(json, sinfo);
	std::vector<std::string> words;

	size_t pos = 0;
	std::string sep = " ";
	while ((pos = sinfo.allowedHTTPmethods.find(sep)) != std::string::npos) {
		words.push_back(sinfo.allowedHTTPmethods.substr(0, pos));
		sinfo.allowedHTTPmethods.erase(0, pos + sep.length());
	}
	if (words.empty() != 1) {
		words.push_back(sinfo.allowedHTTPmethods);
	}
	Route ret = Route(sinfo.islistingdirectory, sinfo.directoryfile, sinfo.isuploadable, sinfo.uploadroot, sinfo.index, sinfo.root, sinfo.path, sinfo.redirect, std::stoul(sinfo.clientmaxbodysize), words, sinfo.pythoncgiextension, sinfo.perlcgiextextension);
	return (ret);
}

HandleCode	Config::generateHandleCode(const JSON & json, sbh_t sinfo, std::map<std::string, Route> & routes) {
	if (json.getObjects().empty() != 1) {
		throw Config::InvalidConfig();
	}
	sinfo = getInformation(json, sinfo);
	for (std::map<std::string, Route>::iterator it = routes.begin(); it != routes.end(); it++) {
		if ((*it).second.path == sinfo.redirect) {
			return (HandleCode(sinfo.code, sinfo.responsecode, (*it).second));
		}
	}
	throw Config::InvalidConfig();
}

std::vector<Server>	Config::makeServersFromJSONHelper(const JSON & json, sbh_t sinfo, std::map<std::string, Route> & routes, std::map<std::string, HandleCode> & codes) {
	std::vector<Server> ret;
	JSON::object_box temp = json.getObjects();

	sinfo = getInformation(json, sinfo);
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 5, "route") == 0) {
			if (routes.find((*it).first) != routes.end()) {
				routes.erase(routes.find((*it).first));
			}
			routes.insert(std::pair<std::string, Route>((*it).first, generateRoute((*it).second, sinfo)));
		}
	}
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 4, "code") == 0) {
			if (codes.find((*it).first) != codes.end()) {
				codes.erase(codes.find((*it).first));
			}
			codes.insert(std::pair<std::string, HandleCode>((*it).first, generateHandleCode((*it).second, sinfo, routes)));
		}
	}
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 6, "server") == 0) {
			std::vector<Server>	temp = makeServersFromJSONHelper((*it).second, sinfo, routes, codes);
			ret.insert(ret.end(), temp.begin(), temp.end());
		} else if ((*it).first.compare(0, 5, "route") == 0) {
			;
		} else if ((*it).first.compare(0, 4, "code") == 0) {
			;
		} else {
			throw Config::InvalidConfig();
		}
	}
	for (std::vector<Server>::iterator it = ret.begin(); it != ret.end(); it++) {
		if ((*it).host == sinfo.host && (*it).port == sinfo.port) {
			ret.push_back(Server(sinfo.servername, sinfo.host, sinfo.port, false, routes, codes));
			return (ret);
		}
	} 
	ret.push_back(Server(sinfo.servername, sinfo.host, sinfo.port, true, routes, codes));
	return (ret);
}

Config::Config(const JSON & json) {
	std::map<std::string, Route> routes;
	std::map<std::string, HandleCode> codes;
	JSON::object_box temp = json.getObjects();

	sbh_t sinfo = getInformation(json, defaultInformation());

	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 5, "route") == 0) {
			if (routes.find((*it).first) != routes.end()) {
				routes.erase(routes.find((*it).first));
			}
			routes.insert(std::pair<std::string, Route>((*it).first, generateRoute((*it).second, sinfo)));
		}
	}
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 4, "code") == 0) {
			if (codes.find((*it).first) != codes.end()) {
				codes.erase(codes.find((*it).first));
			}
			codes.insert(std::pair<std::string, HandleCode>((*it).first, generateHandleCode((*it).second, sinfo, routes)));
		}
	}
	for (JSON::object_box::iterator it = temp.begin(); it != temp.end(); it++) {
		if ((*it).first.compare(0, 6, "server") == 0) {
			std::vector<Server>	temp = makeServersFromJSONHelper((*it).second, sinfo, routes, codes);
			this->servers.insert(this->servers.end(), temp.begin(), temp.end());
		} else if ((*it).first.compare(0, 5, "route") == 0) {
			;
		} else if ((*it).first.compare(0, 4, "code") == 0) {
			;
		} else {
			throw Config::InvalidConfig();
		}
	}
	for (std::vector<Server>::iterator it = this->servers.begin(); it != this->servers.end(); it++) {
		if ((*it).isdefault) {
			for(std::vector<Server>::iterator sit = it + 1; sit != this->servers.end(); sit++) {
				if ((*sit).isdefault && (*sit).host.compare(0, (*sit).host.size(), (*it).host) == 0 && (*sit).port == (*it).port) {
					(*sit).isdefault = false;
				}
			}
		}
	}
}
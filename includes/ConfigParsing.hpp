#pragma once

class Route {
	public:
		Route();
		Route(
			bool islistingdirectory, 
			std::string directoryfile,
			bool uploadable,
			std::string uploadRoot,
			std::string index, 
			std::string root, 
			std::string path, 
			std::string clientmaxbodysize, 
			std::vector<std::string> allowedHTTPmethods, 
			std::string pythoncgiextension,
			std::string phpcgiextension
		);
		Route(const Route & route);
		~Route();
		Route & operator=(const Route & route);
			bool			getListingDirectory() const;
		std::string 	getDirectoryFile() const;
		bool			getUploadable() const;
		std::string 	getUploadRoot() const;
		std::string		getIndex() const;
		std::string		getRoot() const;
		std::string		getPath() const;
		std::string		getPythonCGIExtension() const;
		std::string		getPHPCGIExtension() const;
		std::vector<std::string> getAllowedHTTPMethods() const;
		std::string		getClientMaxBodySize() const;
	private:
		// is this routes directory listing its contents to the client
		bool			islistingdirectory;
			// the default page that is shown if islistingdirectory is false
		std::string		directoryfile;
			// can the route upload files
		bool			isuploadable;
			// location where uploaded files are stored
		std::string 	uploadroot;
			// index of the route, first file found is shown to the user
		std::string		index;
			// root directory to search for files and the like
		std::string		root;
			// path where the files are exposed on the uri
		std::string		path;
			// what is the maximum size body that the client is alowed to send to the server
		std::string		clientmaxbodysize;
		
		// Allowed http methods for the route
		std::vector<std::string>	allowedHTTPmethods;
			// Python Extension to look for
		std::string		pythoncgiextension;
			// PHP Extension to look for
		std::string		phpcgiextension;
		
		// Common errors
		struct InvalidHTTPMethod: public std::exception { const char * what () const throw () { return "Invalid HTTP method"; } };
};

class HandleCode {
	public:
		HandleCode(int code, Route route, int responsecode);
		HandleCode(const HandleCode & handlecode);
		~HandleCode();
		HandleCode & operator=(const HandleCode & handlecode);
		unsigned int	getCode() const;
		Route 			getRoute() const;
		unsigned int	getResponseCode() const;
	private:
		// The HTTP code that is defined
		unsigned int			code;
		// code that is sent to the user
		unsigned int			responsecode;
		// The Route to redirect the if the code is encountered
		Route					route;
		// Common errors
		struct InvalidHTTPCode: public std::exception { const char * what () const throw () { return "Invalid HTTP code"; } };
};

class JSON {
	public:
		// shorter versions of names for data storage
		typedef std::map<std::string, std::string>					string_box;
		typedef std::map<std::string, long double>					number_box;
		typedef std::map<std::string, bool>							boolean_box;
		typedef std::vector<std::pair<std::string, JSON> >			object_box;

		JSON();
		JSON(std::string path);
		JSON(const JSON & json);
		~JSON();
		JSON & operator=(const JSON & json);
		const string_box getStrings() const;
		const number_box getNumbers() const;
		const boolean_box getBooleans() const;
		const object_box getObjects() const;
		const std::vector<std::string> getNulls() const;
		const std::vector<std::string> getKeys() const;

	protected:
		// Recursive Constructor (Should never be called outside of string constructor)
		JSON(std::ifstream & file);

		// Data
		string_box					strings;
		number_box					numbers;
		boolean_box					booleans;
		object_box					objects;
		std::vector<std::string>	nulls;
		
		// All keys
		std::vector<std::string>	keys;

		// Functions used to parse file into data
		void		skipwhitespace(std::ifstream & file);
		std::string	parsestring(std::ifstream & file);
		long double	parsenumber(std::ifstream & file, char c);
		int			isbooleantrue(std::ifstream & file);
		int			isbooleanfalse(std::ifstream & file);
		int			isnull(std::ifstream & file);

		// Errors related to IO
		struct InvalidPath: public std::exception { const char * what () const throw () { return "No file was found from the given path"; } };
		struct ErrorReadingFile: public std::exception { const char * what () const throw () { return "A fatal error was encountered while reading the file"; } };
		struct InvalidExtension: public std::exception {const char * what () const throw () { return "Invalid extension, only .json files are allowed!"; } };

		// Errors related to Incorectly Formated JSON
		struct InvalidJSON: public std::exception { const char * what () const throw () { return "An error with the JSON was found"; } };
		struct InvalidJSONWrongRootType: public std::exception { const char * what () const throw () { return "Expected \"{\" but got something else"; } };
		struct InvalidJSONWrongObjectEnd: public std::exception { const char * what () const throw () { return "Expected \"}\" but got something else"; } };
		struct InvalidJSONWrongKeyType: public std::exception { const char * what () const throw () { return "Expected STRING but got something else"; } };
		struct InvalidJSONWrongValueType: public std::exception { const char * what () const throw () { return "Expected STRING, OBJECT, TRUE, FALSE, NULL or NUMBER but got something else"; } };
		struct InvalidJSONWrongSepteratorCharacter: public std::exception { const char * what () const throw () { return "Expected \":\" but got something else"; } };
		struct InvalidJSONMulitipleRootsFound: public std::exception { const char * what () const throw () { return "Multiple root founds, store multiple VALUEs in an OBJECT"; } };
		struct InvalidJSONUnclosedQuotations: public std::exception { const char * what () const throw () { return "An unclosed quote was discovered"; } };
		struct InvalidJSONDuplicateKeyNames: public std::exception { const char * what () const throw () { return "Two Different KEYs share the same name!"; } };
};

class ServerInfo {
	private:
		std::string						servername;
		std::string						host;
		unsigned int					port;
		bool							isdefault;
		// routes and error redirections associated with the server 
		std::map<std::string, Route>			routes;
		std::map<std::string, HandleCode>		codehandlers;

	public:
		ServerInfo() {};
		ServerInfo(std::string servername, std::string host, unsigned int port, bool isdefault, std::map<std::string, Route> routes, std::map<std::string, HandleCode> codes) {
			this->servername = servername;
			this->host = host;
			if (port > 65535) {
				throw PortOutsideOfRange();
			}
			this->port = port;
			this->isdefault = isdefault;
			this->routes = routes;
			this->codehandlers = codes;
		};

		ServerInfo &operator=(const ServerInfo & serverInfo) {
			this->servername = serverInfo.servername;
			this->host = serverInfo.host;
			this->port = serverInfo.port;
			this->isdefault = serverInfo.isdefault;
			this->routes = serverInfo.routes;
			this->codehandlers = serverInfo.codehandlers;
		};
		~ServerInfo() {};

		std::string	getServerName() const { return (this->servername); }
		std::string	getHost() const { return (this->host); }
		unsigned int	getPort() const { return (this->port); }
		bool	getIsDefault() const { return (this->isdefault); }
		void	setIsDefault(bool d) { this->isdefault = d; }
		std::map<std::string, Route> getRoutes() const { return (this->routes); }
		std::map<std::string, HandleCode> getHandleCode() const { return (this->codehandlers); }

		struct PortOutsideOfRange: public std::exception { const char * what () const throw () { return "Port Outside of Range, please chose a value inbetween 0 to 65535"; } };
};

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
	std::string		phpcgiextextension;
}	sbh_t;

struct InvalidJSONObjectIdentifier: public std::exception { const char * what () const throw () { return "JSON objects should start with route, code or server"; } };
struct InvalidJSONObjectInRoute: public std::exception { const char * what () const throw () { return "Don't define JSON objects inside a route"; } };
struct InvalidJSONObjectInHandleCode: public std::exception { const char * what () const throw () { return "Don't define JSON objects inside a code"; } };
struct InvalidJSONHandleCodeInvalidRoute: public std::exception { const char * what () const throw () { return "Code needs to have a valid route to redirect towards"; };

sbh_t	getInformation(const JSON & json, sbh_t ret) {
	try {
		ret.index = json.getStrings().at("index");
	} catch (std::exception & e) { }
	try {
		ret.root = json.getStrings().at("root");
	} catch (std::exception & e) { }
	try {
		ret.allowedHTTPmethods = json.getStrings().at("allowed_methods");
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
		ret.directoryfile = json.getStrings().at("upload_root");
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
		ret.phpcgiextextension = json.getStrings().at("php_cgi_extensions");
	} catch (std::exception & e) { }
	return (ret);
}

sbh_t defaultInformation() {
	sbh_t ret;
	ret.index = "index.html";
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
	ret.redirect = "/500";
	ret.responsecode = 0;
	ret.clientmaxbodysize = "5M";
	ret.pythoncgiextension = "";
	ret.phpcgiextextension = "";
	return (ret);
}

Route	generateRoute(const JSON & json, sbh_t sinfo) {
	if (json.getObjects().empty() != 1) {
		throw InvalidJSONObjectInRoute();
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
	Route ret = Route(sinfo.islistingdirectory, sinfo.directoryfile, sinfo.isuploadable, sinfo.uploadroot, sinfo.index, sinfo.root, sinfo.path, sinfo.clientmaxbodysize, words, sinfo.pythoncgiextension, sinfo.phpcgiextextension);
	return (ret);
}

HandleCode	generateHandleCode(const JSON & json, sbh_t sinfo, std::map<std::string, Route> & routes) {
	if (json.getObjects().empty() != 1) {
		throw InvalidJSONObjectInHandleCode();
	}
	sinfo = getInformation(json, sinfo);

	for (std::map<std::string, Route>::iterator it = routes.begin(); it != routes.end(); it++) {
		if ((*it).second.getPath() == sinfo.redirect) {
			return (HandleCode(sinfo.code, (*it).second, sinfo.responsecode));
		}
	}
	throw InvalidJSONHandleCodeInvalidRoute(); // temp solution, should set up default error routes depending on the code.
}

std::vector<ServerInfo>	makeServersFromJSONHelper(const JSON & json, sbh_t sinfo, std::map<std::string, Route> & routes, std::map<std::string, HandleCode> & codes) {
	std::vector<ServerInfo> ret;
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
			std::vector<ServerInfo>	temp = makeServersFromJSONHelper((*it).second, sinfo, routes, codes);
			ret.insert(ret.end(), temp.begin(), temp.end());
		} else if ((*it).first.compare(0, 5, "route") == 0) {
			;
		} else if ((*it).first.compare(0, 4, "code") == 0) {
			;
		} else {
			throw InvalidJSONObjectIdentifier();
		}
	}
	for (std::vector<ServerInfo>::iterator it = ret.begin(); it != ret.end(); it++) {
		if ((*it).getHost() == sinfo.host && (*it).getPort() == sinfo.port) {
			ret.push_back(ServerInfo(sinfo.servername, sinfo.host, sinfo.port, false, routes, codes));
			return (ret);
		}
	} 
	ret.push_back(ServerInfo(sinfo.servername, sinfo.host, sinfo.port, true, routes, codes));
	return (ret);
}

std::vector<ServerInfo>	getServerInfo(const JSON & json) {
	std::vector<ServerInfo> ret;
	std::map<std::string, Route> routes;
	std::map<std::string,HandleCode> codes;
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
			std::vector<ServerInfo>	temp = makeServersFromJSONHelper((*it).second, sinfo, routes, codes);
			ret.insert(ret.end(), temp.begin(), temp.end());
		} else if ((*it).first.compare(0, 5, "route") == 0) {
			;
		} else if ((*it).first.compare(0, 4, "code") == 0) {
			;
		} else {
			throw InvalidJSONObjectIdentifier();
		}
	}
	for (std::vector<ServerInfo>::iterator it = ret.begin(); it != ret.end(); it++) {
		if ((*it).getIsDefault()) {
			for(std::vector<ServerInfo>::iterator sit = it + 1; sit != ret.end(); sit++) {
				if ((*sit).getIsDefault() && (*sit).getHost().compare(0, (*sit).getHost().size(), (*it).getHost()) == 0 && (*sit).getPort() == (*it).getPort()) {
					(*sit).setIsDefault(false);
				}
			}
		}
	}
	return (ret);
}

#pragma once

#include <fstream>

class Route {
	public:
		Route() {}

		Route(bool islistingdirectory, std::string directoryfile, bool isuploadable, std::string uploadroot, std::string index, std::string root, std::string path, std::string clientmaxbodysize, std::vector<std::string> allowedHTTPmethods, std::string pythoncgiextension, std::string phpcgiextension) {
			this->islistingdirectory = islistingdirectory;
			this->directoryfile = directoryfile;
			this->isuploadable = isuploadable;
			this->uploadroot = uploadroot;
			this->index = index;
			this->root = root;
			this->path = path;
			this->clientmaxbodysize = clientmaxbodysize;
			this->allowedHTTPmethods = allowedHTTPmethods;
			this->phpcgiextension = phpcgiextension;
			this->pythoncgiextension = pythoncgiextension;
		}

		Route(const Route & route) {
			this->islistingdirectory = route.islistingdirectory;
			this->directoryfile = route.directoryfile;
			this->index = route.index;
			this->root = route.root;
			this->path = route.path;
			this->clientmaxbodysize = route.clientmaxbodysize;
			this->allowedHTTPmethods = route.allowedHTTPmethods;
			this->phpcgiextension = route.phpcgiextension;
			this->pythoncgiextension = route.pythoncgiextension;
		}

		~Route() {}

		Route & operator=(const Route & route) {
			this->islistingdirectory = route.islistingdirectory;
			this->directoryfile = route.directoryfile;
			this->index = route.index;
			this->root = route.root;
			this->path = route.path;
			this->clientmaxbodysize = route.clientmaxbodysize;
			this->allowedHTTPmethods = route.allowedHTTPmethods;
			this->phpcgiextension = route.phpcgiextension;
			this->pythoncgiextension = route.pythoncgiextension;
			return (*this);
		}

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

std::ostream &operator<<(std::ostream &os, const Route &route)
{
	os << "listing directory: " << route.islistingdirectory << std::endl;
	os << "directory file: " << route.directoryfile << std::endl;
	os << "index: " << route.index << std::endl;
	os << "root: " << route.root << std::endl;
	os << "client max body size: " << route.clientmaxbodysize << std::endl;
	//std::cout << it->second.getAllowedHTTPMethods() << std::endl;
	os << "php cgi: " << route.phpcgiextension << std::endl;
	os << "python cgi: " << route.pythoncgiextension << std::endl;

	return os;
}

class HandleCode {
	public:
		// The HTTP code that is defined
		unsigned int			code;
		// code that is sent to the user
		unsigned int			responsecode;
		// The Route to redirect the if the code is encountered
		Route					route;
		// Common errors
		struct InvalidHTTPCode: public std::exception { const char * what () const throw () { return "Invalid HTTP code"; } };
		
		HandleCode(const HandleCode & handlecode) {
			this->code = handlecode.code;
			this->route = handlecode.route;
			this->responsecode = handlecode.responsecode;
		}

		HandleCode(int code, Route route, int responsecode) {
			//* apparently here we should handle invalid http codes
			/*std::vector<const unsigned int>::iterator valid = std::find(validHTTPCodes.begin(), Webserv::validHTTPCodes.end(), code);
			if (valid == validHTTPCodes.end()) {
				throw InvalidHTTPCode();
			}
			valid = std::find(validHTTPCodes.begin(), validHTTPCodes.end(), responsecode);
			if (valid == validHTTPCodes.end() && responsecode != 0) {
				throw InvalidHTTPCode();
			}*/
			this->code = code;
			this->route = route;
			this->responsecode = responsecode;
		}

		~HandleCode() { }

		HandleCode &  operator=(const HandleCode & handlecode) {
			this->code = handlecode.code;
			this->route = handlecode.route;
			this->responsecode = handlecode.responsecode;
			return (*this);
		}
};

std::ostream &operator<<(std::ostream &os, HandleCode &h)
{
	os << "code: " << h.code << std::endl;
	os << "route: " << h.route << std::endl;
	os << "response code: " << h.responsecode << std::endl;

	return os;
}

class JSON {
	public:
		typedef std::map<std::string, std::string>					string_box;
		typedef std::map<std::string, long double>					number_box;
		typedef std::map<std::string, bool>							boolean_box;
		typedef std::vector<std::pair<std::string, JSON> >			object_box;

		// shorter versions of names for data storage
		JSON(): strings(), numbers(), booleans(), objects() { }

		std::string find_extension_parser(std::string path) {
			std::string temp = path;
			while (temp.find(".") != std::string::npos) {
				temp.erase(0, temp.find(".") + 1);
			}
			if (path.length() == temp.length()) {
				return "";
			}
			if (temp.find('/') != std::string::npos) {
				return "";
			}
			return temp;
		}

		JSON(std::string path) {
			std::ifstream	file;
			char			c;

			if (access(path.c_str(), F_OK | R_OK) == -1) {
				throw InvalidPath();
			} else if (find_extension_parser(path) != "json") {
				throw InvalidExtension();
			}
			file.open(path);
			skipwhitespace(file);
			if (!file.eof()) {
				file.get(c);
				if (c == '{') {
					bool notdone = true;
					while (!file.eof()) {
						skipwhitespace(file);
						file.get(c);
						if (c == '}') {
							if (notdone == true) {
								file.close();
								throw InvalidJSONWrongKeyType();
							}
							break ;
						} else if (c != '"') {
							file.close();
							throw InvalidJSONWrongKeyType();
						} else if (notdone == false) {
							file.close();
							throw InvalidJSONWrongObjectEnd();
						}
						std::string str = parsestring(file);
						if (std::find(this->keys.begin(), this->keys.end(), str) != keys.end()) {
							file.close();
							throw InvalidJSONDuplicateKeyNames();
						}
						file.get(c);
						skipwhitespace(file);
						file.get(c);
						if (c != ':') {
							file.close();
							throw InvalidJSONWrongSepteratorCharacter();
						}
						skipwhitespace(file);
						file.get(c);
						switch (c) {
							case '"':
								this->strings.insert(std::pair<std::string, std::string>(str, parsestring(file)));
								file.get(c);
								break;
							case '{':
								this->objects.push_back(std::pair<std::string, JSON>(str, JSON(file)));
								break;
							case 't':
								if (isbooleantrue(file) == 1) {
									this->booleans.insert(std::pair<std::string, bool>(str, true));
								}
								break;
							case 'f':
								if (isbooleanfalse(file) == 1) {
									this->booleans.insert(std::pair<std::string, bool>(str, false));
								}
								break;
							case 'n':
								if (isnull(file) == 1) {
									this->nulls.push_back(str);
								}
								break;
							default:
								if (c == '-' || isdigit(c)) {
									this->numbers.insert(std::pair<std::string, long double>(str, parsenumber(file, c)));
								} else {
									file.close();
									throw InvalidJSONWrongValueType();
								}
								break;
						}
						keys.push_back(str);
						skipwhitespace(file);
						file.get(c);
						if (c == ',') { notdone = true; } else {notdone = false;}
					}
					if (!file.eof()) {
						file.close();
						throw InvalidJSONMulitipleRootsFound();
					}
				} else {
					throw InvalidJSONWrongRootType();
				}
			}
			file.close();
		}

		JSON(const JSON & json) {
			this->strings = json.getStrings();
			this->objects = json.getObjects();
			this->booleans = json.getBooleans();
			this->keys = json.getKeys();
			this->numbers = json.getNumbers();
			this->nulls = json.getNulls();
		}

		JSON & operator=(const JSON & json) {
			this->strings = json.getStrings();
			this->objects = json.getObjects();
			this->booleans = json.getBooleans();
			this->keys = json.getKeys();
			this->numbers = json.getNumbers();
			this->nulls = json.getNulls();
			return (*this);
		}

		~JSON() { }

		const string_box getStrings() const {
			return this->strings;
		}

		const boolean_box getBooleans() const {
			return this->booleans;
		}

		const number_box getNumbers() const {
			return this->numbers;
		}

		const object_box getObjects() const {
			return this->objects;
		}

		const std::vector<std::string> getNulls() const {
			return this->nulls;
		}

		const std::vector<std::string> getKeys() const {
			return this->keys;
		}

	protected:
		// Recursive Constructor (Should never be called outside of string constructor)
		JSON(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			skipwhitespace(file);
			char c = file.peek();
			bool notdone = true;
			if (c == '}') {
				notdone = false;
				file.get(c);
			}
			while (c != '}' && !file.eof()) {
				skipwhitespace(file);
				file.get(c);
				if (c == '}') {
					if (notdone == true) {
						file.close();
						throw InvalidJSONWrongKeyType();
					}
					break ;
				} else if (c != '"') {
					file.close();
					throw InvalidJSONWrongKeyType();
				} else if (notdone == false) {
					file.close();
					throw InvalidJSONWrongObjectEnd();
				}
				std::string str = parsestring(file);
				if (std::find(this->keys.begin(), this->keys.end(), str) != keys.end()) {
					file.close();
					throw InvalidJSONDuplicateKeyNames();
				}
				file.get(c);
				skipwhitespace(file);
				file.get(c);
				if (c != ':') {
					file.close();
					throw InvalidJSONWrongSepteratorCharacter();
				}
				skipwhitespace(file);
				file.get(c);
				switch (c) {
					case '"':
						this->strings.insert(std::pair<std::string, std::string>(str, parsestring(file)));
						file.get(c);
						break;
					case '{':
						this->objects.push_back(std::pair<std::string, JSON>(str, JSON(file)));
						break;
					case 't':
						if (isbooleantrue(file) == 1) {
							this->booleans.insert(std::pair<std::string, bool>(str, true));
						}
						break;
					case 'f':
						if (isbooleanfalse(file) == 1) {
							this->booleans.insert(std::pair<std::string, bool>(str, false));
						}
						break;
					case 'n':
						if (isnull(file) == 1) {
							this->nulls.push_back(str);
						}
						break;
					default:
						if (c == '-' || isdigit(c)) {
							this->numbers.insert(std::pair<std::string, long double>(str, parsenumber(file, c)));
						} else {
							file.close();
							throw InvalidJSONWrongValueType();
						}
					}
				keys.push_back(str);
				skipwhitespace(file);
				file.get(c);
				if (c == ',') { notdone = true; } else {notdone = false;}
			}
			if (file.eof()) {
				file.close();
				throw InvalidJSONWrongObjectEnd();
			}
		}
		// Data
		string_box					strings;
		number_box					numbers;
		boolean_box					booleans;
		object_box					objects;
		std::vector<std::string>	nulls;
		
		// All keys
		std::vector<std::string>	keys;

		// Functions used to parse file into data
		void skipwhitespace(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			char c;
			c = file.peek();
			while (!file.eof() && isspace(c)) {
				file.get(c);
				c = file.peek();
			}
		}

		std::string parsestring(std::ifstream & file) {
			char c;
			std::string str = "";

			while(file.peek() != '"') {
				file.get(c);
				if (file.eof()) {
					file.close();
					throw InvalidJSONUnclosedQuotations();
				}
				if (c == '\\') {
					file.get(c);
					switch (c) {
						case '"':
							str.push_back('"');
							break;
						case '\\':
							str.push_back('"');
							break;
						case '/':
							str.push_back('"');
							break;
						case 'n':
							str.push_back('"');
							break;
						case 't':
							str.push_back('"');
							break;
						case 'b':
							str.push_back('"');
							break;
						case 'f':
							str.push_back('"');
							break;
						case 'r':
							str.push_back('"');
							break;
					}
				} else if (c == '\n') {
					file.close();
					throw InvalidJSON();
				} else {
					str.push_back(c);
				}
			}
			return str;
		}

		long double parsenumber(std::ifstream & file, char c) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			std::string temp;
			bool hasdot = false;
			temp.push_back(c);
			if (c == '0' && isdigit(file.peek())) {
				file.close();
				throw ;
			}
			c = file.peek();
			while ((isdigit(c) || c == '.') && !file.eof()) {
				temp.push_back(c);
				if (c == '.' && hasdot == false) {
					hasdot = true;
					file.get(c);
					c = file.peek();
					if (isdigit(c) && file.eof()) {
						file.close();
						throw ;
					}
				} else if (c == '.' && hasdot) {
					file.close();
					throw ;
				} else {
					file.get(c);
					c = file.peek();
				}
			}
			if (tolower(c) == 'e') {
				temp.push_back(c);
				file.get(c);
				c = file.peek();
				if (c == '+' || c == '-') {
					temp.push_back(c);
					file.get(c);
					c = file.peek();
					while (isdigit(c) && !file.eof()) {
						temp.push_back(c);
						file.get(c);
						c = file.peek();
					}
				} else {
					file.close();
					throw ;
				}
			}
			if (!isspace(c) && file.eof()) {
				file.close();
				throw ;
			}
			return (strtold(temp.c_str(), NULL));
		}

		int isbooleanfalse(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			char c = file.peek();
			if (tolower(c) == 'a') {
				file.get(c);
				c = file.peek();
				if (tolower(c) == 'l') {
					file.get(c);
					c = file.peek();
					if (tolower(c) == 's') {
						file.get(c);
						c = file.peek();
						if (tolower(c) == 'e') {
							file.get(c);
							c = file.peek();
							if (isspace(c) || c == ',' || c == '}') {
								return (1);
							}
						}
					}
				}
			}
			file.close();
			throw InvalidJSONWrongValueType();
		}

		int isbooleantrue(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			char c = file.peek();
			if (c == 'r') {
				file.get(c);
				c = file.peek();
				if (c == 'u') {
					file.get(c);
					c = file.peek();
					if (c == 'e') {
						file.get(c);
						c = file.peek();
						if (isspace(c) || c == ',' || c == '}') {
							return (1);
						}
					}
				}
			}
			file.close();
			throw InvalidJSONWrongValueType();
		}

		int isnull(std::ifstream & file) {
			if (!file.is_open() || file.bad()) {
				if(file.bad()) {
					file.close();
				}
				throw ErrorReadingFile();
			}
			char c = file.peek();
			if (c == 'u') {
				file.get(c);
				c = file.peek();
				if (c == 'l') {
					file.get(c);
					c = file.peek();
					if (c == 'l') {
						file.get(c);
						c = file.peek();
						if (isspace(c) || c == ',' || c == '}') {
							return (1);
						}
					}
				}
			}
			file.close();
			throw InvalidJSONWrongValueType();
		}
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
	public:
		std::string						servername;
		std::string						host;
		unsigned int					port;
		bool							isdefault;
		// routes and error redirections associated with the server 
		std::map<std::string, Route>			routes;
		std::map<std::string, HandleCode>		codehandlers;

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
			return (*this);
		};
		~ServerInfo() {};

		struct PortOutsideOfRange: public std::exception { const char * what () const throw () { return "Port Outside of Range, please chose a value inbetween 0 to 65535"; } };
};

std::ostream &operator<<(std::ostream &os, ServerInfo &info)
{
	os << "Servername: " << info.servername << std::endl;
	os << "Host: " << info.host << std::endl;
	os << "Port: " << info.port << std::endl;
	os << "Isdefault: " << info.isdefault << std::endl;
	os << "Route: " << std::endl;
	for (std::map<std::string, Route>::iterator it = info.routes.begin(); it != info.routes.end(); it++) {
		std::cout << "===================" << std::endl;
		std::cout << "route name" << it->first << std::endl;
		std::cout << "route info:" << std::endl << it->second << std::endl;
		std::cout << "=======================" << std::endl;
	}
	//std::cout << it->second.getAllowedHTTPMethods() << std::endl;
	os << "Code Handlers: " << info.codehandlers << std::endl;
	for (std::map<std::string, HandleCode>::iterator it = info.codehandlers.begin(); it != info.codehandlers.end(); it++) {
		std::cout << "===================" << std::endl;
		std::cout << "code handler name" << it->first << std::endl;
		std::cout << "code handler info:" << std::endl << it->second << std::endl;
		std::cout << "=======================" << std::endl;
	}

	return os;
}

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
struct InvalidJSONHandleCodeInvalidRoute: public std::exception { const char * what () const throw () { return "Code needs to have a valid route to redirect towards"; } };

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
		if ((*it).second.path == sinfo.redirect) {
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
		if ((*it).host == sinfo.host && (*it).port == sinfo.port) {
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
		if ((*it).isdefault) {
			for(std::vector<ServerInfo>::iterator sit = it + 1; sit != ret.end(); sit++) {
				if ((*sit).isdefault && (*sit).host.compare(0, (*sit).host.size(), (*it).host) == 0 && (*sit).port == (*it).port) {
					(*sit).isdefault = false;
				}
			}
		}
	}
	return (ret);
}

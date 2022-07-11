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
struct InvalidJSONHandleCodeInvalidRoute: public std::exception { const char * what () const throw () { return "Code needs to have a valid route to redirect towards"; }
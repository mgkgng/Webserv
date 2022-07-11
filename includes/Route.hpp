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

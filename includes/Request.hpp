class Request {
	private:
		std::string 						method;
		std::string							path;
		std::map<std::string, std::string>	attributes;
		std::map<std::string, std::string>	post_attributes;
		std::string							protocol_v;
		std::map<std::string, std::string>	headers;
		
		std::string							body;
		std::string							file;
		Response							res;
	public:
		Request();
		Request(std::string);
		Request(Request const &);
		~Request();
		Request & operator=(Request const & rhs);
		
		void	parseRequest(std::string);
		void	parseErrorCheck() const;	
		void	getResponse() const;

		std::string	getResponse();
		std::string	getMethod() const;
		std::string	getBody() const;

		std::map<std::string, std::string> getHeaders() const;

		void	do_request_depending_on_file_type(const std::string file, const Webserv::Route & it);
		struct ERROR400: public std::exception { const char * what () const throw () { return "Error 400"; } };
		struct ERROR403: public std::exception { const char * what () const throw () { return "Error 403"; } };
		struct ERROR404: public std::exception { const char * what () const throw () { return "Error 404"; } };
		struct ERROR405: public std::exception { const char * what () const throw () { return "Error 405"; } };
};

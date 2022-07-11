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
};

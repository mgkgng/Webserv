class Response {
	private:
		std::string protocol_v;
		std::string status_code;
		std::string status_message;
		std::map<std::string, std::string> headers;
	public:
		Response();
		Response(std::string, std::string, std::string, std::map<std::string, std::string>);
		Response(Response const &);
		~Response();
		Response & operator=(Response const &);
		
		std::string getProtocol() const;
		std::string	getStatusCode() const;
		std::string	getStatusMsg() const;
};
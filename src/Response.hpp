class Response {
	private:
		std::string protocol_v;
		std::string status_code;
		std::string status_message;
		std::map<std::string, std::string> headers;
	public:
		Response() {}

		Response(std::string ptc, std::string code, std::string msg, std::map<std::string, std::string> headers) {
			this->protocol_v = ptc;
			this->status_code = code;
			this->status_message = msg;
			this->headers = headers;
		}

		Response(Response const & other) {
			this->protocol_v = other.protocol_v;
			this->status_code = other.status_code;
			this->status_message = other.status_message;
			this->headers = other.headers;
			*this = other;
		}

		~Response() {}

		Response & operator=(Response const & rhs) {
			this->protocol_v = rhs.protocol_v;
			this->status_code = rhs.status_code;
			this->status_message = rhs.status_message;
			this->headers = rhs.headers;
			return (*this);
		}

		std::string getProtocol() const {
			return (this->protocol_v);
		}

		std::string getStatusCode() const {
			return (this->status_code);
		}

		std::string getStatusMsg() const {
			return (this->status_message);
		}
};
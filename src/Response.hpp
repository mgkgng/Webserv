class Response {
	public:
		std::string protocolVer;
		unsigned int	statCode;
		std::string statMsg;
		std::map<std::string, std::string> headers;
		std::string	body;

		Response() {}

		Response(std::string ptc, std::string code, std::string msg, std::map<std::string, std::string> headers) {
			this->protocolVer = ptc;
			this->statCode = code;
			this->statMsg = msg;
			this->headers = headers;
		}

		Response(Response const & other) {
			*this = other;
		}

		~Response() {}

		Response & operator=(Response const & rhs) {
			this->protocolVer = rhs.protocolVer;
			this->statCode = rhs.statCode;
			this->statMsg = rhs.statMsg;
			this->headers = rhs.headers;
			return (*this);
		}

		void	putBody(Route &route) {
			std::ifstream f(route.root + "/" + route.index);
			std::stringstream buf;

			buf << f.rdbuf();
			this->body = buf.str();
			this->headers["Content-length"] = this->body.length();
		}

		void	putBody(unsigned int errCode) {

		}
};

std::ostream &operator<<(std::ostream &os, Response const &res)
{
	os << res.protocolVer << ' ' << res.statCode << ' ' << res.statMsg << ' ' << "\r\n";
	os << "Content-Size: " << 
	os << std::endl;

	os << res.body;

	return os;
}
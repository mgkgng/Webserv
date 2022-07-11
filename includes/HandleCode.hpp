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
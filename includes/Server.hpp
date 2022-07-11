class Server {
	public:
		Server();
		Server(std::string servername, std::string host, unsigned int port, bool isdefault, std::map<std::string, Route> routes, std::map<std::string, HandleCode> codes);
		~Server();
		Server & operator=(const Server & server);
		
		// Getters
		std::string							getServerName() const;
		std::string							getHost() const;
		unsigned int						getPort() const;
		bool								getIsDefault() const;
		std::map<std::string, Route>		getRoutes() const;
		std::map<std::string, HandleCode>	getHandleCode() const;

		// Server Launch 
		void				launch();
		// Manage client requests
		void				init_addrinfo();
		void				init_server();
		void				acceptConnection();
		void				disconnect(int fd);
		void				sendData(int c_fd);
		void				recvData(struct kevent &ev);

		void				setIsDefault(bool b);
	private:
		// Information about the server, such as its name, it's host and port, and if it's the default server for the port or not
		std::string						servername;
		std::string						host;
		unsigned int					port;
		bool							isdefault;
		// routes and error redirections associated with the server 
		std::map<std::string, Route>			routes;
		std::map<std::string, HandleCode>		codehandlers;
		// In order to launch the server
		int								sockfd;
		int								kq;
		struct sockaddr_in				sockaddr;
		int								addrlen;
		std::vector<struct kevent>		chlist;
		std::vector<struct kevent>		evlist;
		bool							quit;
};
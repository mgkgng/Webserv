/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrathelo <student.42nice.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 17:08:04 by min-kang          #+#    #+#             */
/*   Updated: 2022/06/24 15:30:16 by jrathelo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "libft.hpp"
#include <JSON.hpp>
#include <Client.hpp>
#include <Response.hpp>

#define PORT 8080
#define BACKLOG 20

namespace Webserv {
	class Route {
		public:
			Route();
			Route(
				bool islistingdirectory, 
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

	class Server {
		public:
			Server();
			Server(std::string servername, std::string host, unsigned int port, bool isdefault, std::map<std::string, Route> routes, std::map<std::string, HandleCode> codes);
			Server(const Server & server);
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
			// Client* 			getClient(int fd);

			void				setIsDefault(bool b);
		private:
			// Information about the server, such as its name, it's host and port, and if it's the default server for the port or not
			std::string							servername;
			std::string							host;
			unsigned int						port;
			bool								isdefault;

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
			// std::vector<Client>				clients;
			bool							quit;
			
			// Common errors
			struct PortOutsideOfRange: public std::exception { const char * what () const throw () { return "Port Outside of Range, please chose a value inbetween 0 to 65535"; } };

	};

	class Request {
		private:
			std::string 						method;
			std::string							path;
			std::map<std::string, std::string>	attributes;
			std::string							protocol_v;

			std::map<std::string, std::string>	headers;
			
			std::string							body;

			std::string							file;
		public:
			Request();
			Request(std::string, Server & server);
			Request(Request const &);
			~Request();

			Request & operator=(Request const & rhs);
			
			void	parseRequest(std::string);
			
			std::string	getMethod() const;
			std::string	getBody() const;
			std::map<std::string, std::string> getHeaders() const;
	};

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
		std::string		index;
		std::string		root;
		std::string		path;
		std::string		clientmaxbodysize;
		std::string		allowedHTTPmethods;
		std::string		pythoncgiextension;
		std::string		phpcgiextextension;
	}	sbh_t;
	
	std::vector<Server>		makeServersFromJSON(const JSON & json);
	void					start(std::vector<Server> & servers);
	//static void			thread_launch(void *ptr[2]);
	void					thread_launch(void *ptr);

	struct InvalidJSONObjectIdentifier: public std::exception { const char * what () const throw () { return "JSON objects should start with route, code or server"; } };
	struct InvalidJSONObjectInRoute: public std::exception { const char * what () const throw () { return "Don't define JSON objects inside a route"; } };
	struct InvalidJSONObjectInHandleCode: public std::exception { const char * what () const throw () { return "Don't define JSON objects inside a code"; } };
	struct InvalidJSONHandleCodeInvalidRoute: public std::exception { const char * what () const throw () { return "Code needs to have a valid route to redirect towards"; } };

	static const unsigned int arr[] = {
		100, // Continue
		101, // Switching Protocols
		102, // Processing
		103, // Early Hints
		200, // OK
		201, // Created
		202, // Accepted
		203, // Non-Authorative Information
		204, // No Content
		205, // Reset Content
		206, // Partial Content
		207, // (WebDav) Multi-Status
		208, // (WebDav) Already Reported
		226, // (HTTP Delta encoding) IM used
		300, // Multiple Choices
		301, // Moved Permanenitly
		302, // Found
		303, // See Other
		304, // Not Modified
		305, // Use Proxy
		307, // Temporary Redirect
		308, // Permanent Redirect
		400, // Bad Request
		401, // Unauthorized
		402, // Payment Required 
		403, // Forbidden
		404, // Not Found
		405, // Method not Allowed
		406, // Not Exceptable
		407, // Proxy Authenitcation Required
		408, // Request Timeout
		409, // Conflict
		410, // Gone
		411, // Length Required
		412, // Precondition Failed
		413, // Payload to Large
		414, // URI to long
		415, // Unsuported Media Type
		416, // Range not Satisfiable 
		417, // Execption Failed
		418, // I'm a teapot
		421, // Misdirected Request
		422, // (WebDav) Unprocessable Entity
		423, // (WebDav) Locked
		424, // (WebDav) Failed Dependency
		425, // Too Early
		426, // Upgrade Required
		428, // Precondition Required
		429, // Too many Requests
		431, // Request Header To Large
		451, // Unavaliable for Legal Reasons
		500, // Internal Server Error
		501, // Not Implemented
		502, // Bad Gateway
		503, // Service Unavaliable
		504, // Gateway Timeout
		505, // HTTP version not supported
		506, // Varient also Negates
		507, // (WebDav) Insufficient Storage
		508, // (WebDav) Loop detected
		510, // Not Extended
		511  // Network Authentication Required
	};
	static const std::vector<unsigned int> validHTTPCodes (arr, arr + sizeof(arr) / sizeof(arr[0]) );
};

#endif
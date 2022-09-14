#include "Server.hpp"
#include "ReqRes.hpp"

#include <netdb.h>
#include <arpa/inet.h>
#include <string>

struct sortByComplex { 
	bool operator() (const Route & struct1, const Route & struct2) {
    	return (struct1.path.length() > struct2.path.length());
	}
}	sortByComplex;


void Server::init_server() {
	struct hostent *hp;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);

	sockaddr.sin_family = AF_INET;

	hp = gethostbyname(this->host.c_str());
	assert(hp != NULL);
	sockaddr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *) hp->h_addr_list[0])));
	sockaddr.sin_port = htons(this->port);

	int	option_on = 1;
	assert(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option_on, sizeof(int)) == 0);
	assert(bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == 0);
	assert(listen(sockfd, 20) == 0);

	kq = kqueue();
	assert(kq != -1);

	chlist.resize(1);
	EV_SET(&*(chlist.end() - 1), sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
};

void Server::acceptConnection(struct kevent &ev) {
	while (ev.data--) {
		socklen_t socklen = sizeof(sockaddr);
		int	newConnection = accept(sockfd, (struct sockaddr *) &sockaddr, &socklen);
		assert(newConnection != -1);
		fcntl(newConnection, F_SETFL, O_NONBLOCK);

		chlist.resize(chlist.size() + 2);
		EV_SET(&*(chlist.end() - 2), newConnection, EVFILT_READ, EV_ADD, 0,0, NULL);
		EV_SET(&*(chlist.end() - 1), newConnection, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, 6000, NULL); // TIMEOUT 

		client[newConnection] = Request(newConnection, this->root);
	}
};

void Server::disconnect(struct kevent &ev) {
	client.erase(ev.ident);
	close(ev.ident);
};

void Server::sendData(struct kevent &ev) {
	Request	&req = client[ev.ident];
	string	res = req.res.getStr();	

	/* send */
	size_t bits = send(ev.ident, res.c_str() + req.res.sendBits , res.length(), MSG_DONTWAIT);
	req.res.sendBits += bits;
	/* check if send all data */
	if (req.res.sendBits < res.length())
		return ;
	req.clean();
	chlist.resize(chlist.size() + 1);
	EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	client.erase(ev.ident);
	close(ev.ident);
}

void Server::findCodeHandler(int code, Request & req) {
	codes_t::iterator it = this->codes.begin();
	for (; it != this->codes.end(); it++) {
		if (it->second.code == code) {
			req.redirect(it->second.route.path);
			return;
		}
	}
	req.putCustomError(code);
}

std::string Server::find_extension(std::string path) {
	std::string temp = path;
	while (temp.find(".") != std::string::npos) {
		temp.erase(0, temp.find(".") + 1);
	}
	if (path.length() == temp.length()) {
		return "";
	}
	if (temp.find('/') != std::string::npos) {
		return "";
	}
	return temp;
}

std::string Server::get_file_from_path(std::string path, std::string route) {
	if (end_with(path, "/"))
		path = trim(path, "/");
	return path + route;
}

std::string Server::get_file_full_path(std::string requested_file, std::string root) {
	char buf [MAXPATHLEN];
	getcwd(buf, MAXPATHLEN);
	if (root.find('.') == 0 && root.find('/') == 1) {
		root.replace(0, 1, buf);
	} else if (root.find('/') != 0) {
		root.insert(0, 1, '/');
		root.insert(0, buf);
	}
	if (!end_with(root, "/")) {
		root.push_back('/');
	}
	if (start_with(requested_file, "/"))
		requested_file = requested_file.substr(1);
	return root + requested_file;
}

void Server::doResBasedOnReq(Request & req, Route & match) {
	if (req.path.length() == match.path.length()) {
		std::string file = get_file_full_path(match.index, match.root);
		std::string extension = find_extension(match.index);
		if (check_if_file_exists(file)) {
			if (extension == "") {
				if (req.method == "GET") {
					req.putResponse(match, file);
				} else if (req.method == "POST") {
					req.postContent(*this, match);
				} else if (req.method == "DELETE") {
					if (!remove((file).c_str())) {
						findCodeHandler(204, req);
					} else {
						findCodeHandler(500, req);
					}
				}
			} else if (extension == match.perlcgiExtension) {
				execute_cgi(req, true);
			} else if (extension == match.pythoncgiExtension) {
				execute_cgi(req, false);
			} else {
				if (req.method == "GET") {
					req.putResponse(match, file);
				} else if (req.method == "POST") {
					req.postContent(*this, match);
				} else if (req.method == "DELETE") {
					if (!remove((file).c_str())) {
						findCodeHandler(204, req);
					} else {
						findCodeHandler(500, req);
					}
				}
			}
		} else if (check_if_file_is_dir(file) && match.autoindex) {
			string body = req.res.putAutoIndex(req.path, match.root, file);
			if (body == "")
				findCodeHandler(404, req);
			else
				req.putAutoIndexRes(body);
		} else {
			findCodeHandler(404, req);
		}
	} else {
		std::string extension = find_extension(req.path);
		std::string temp = req.path.substr(match.path.length());
		std::string file = get_file_full_path(temp, match.root);
		if (extension == "") {
			if (check_if_file_exists(file)) {
				if (req.method == "GET") {
					req.putResponse(match, file);
				} else if (req.method == "POST") {
					req.postContent(*this, match);
				} else if (req.method == "DELETE") {
					if (!remove((file).c_str())) {
						findCodeHandler(204, req);
					} else {
						findCodeHandler(500, req);
					}
				}
			} else if (check_if_file_is_dir(file) && match.autoindex) {
				string body = req.res.putAutoIndex(req.path, match.root, file);
				if (body == "")
					findCodeHandler(404, req);
				else
					req.putAutoIndexRes(body);
			} else {
				findCodeHandler(404, req);
			}
		} else if (extension == match.perlcgiExtension) {
			if (check_if_file_exists(file)) {
				execute_cgi(req, true);
			} else if (check_if_file_is_dir(file) && match.autoindex) {
				string body = req.res.putAutoIndex(req.path, match.root, file);
				if (body == "")
					findCodeHandler(404, req);
				else
					req.putAutoIndexRes(body);
			} else {
				findCodeHandler(404, req);
			}
		} else if (extension == match.pythoncgiExtension) {
			if (check_if_file_exists(file)) {
				execute_cgi(req, false);
			} else if (check_if_file_is_dir(file) && match.autoindex) {
				string body = req.res.putAutoIndex(req.path, match.root, file);
				if (body == "")
					findCodeHandler(404, req);
				else
					req.putAutoIndexRes(body);
			} else {
				findCodeHandler(404, req);
			}
		} else {
			if (exist(file)) {
				if (req.method == "GET") {
					req.putResponse(match, file);
				} else if (req.method == "POST") {
					req.postContent(*this, match);
				} else if (req.method == "DELETE") {
					if (!remove((file).c_str())) {
						findCodeHandler(204, req);
					} else {
						findCodeHandler(500, req);
					}
				}
			} else if (check_if_file_is_dir(file) && match.autoindex) {
				string body = req.res.putAutoIndex(req.path, match.root, file);
				if (body == "")
					findCodeHandler(404, req);
				else
					req.putAutoIndexRes(body);
			} else {
				findCodeHandler(404, req);
			}
		}
	}
}

void Server::recvData(struct kevent &ev) {
	char	buf[10000];
	int		ret;
	/* recv */
	ret = recv(ev.ident, buf, 9999, 0);
	if (ret <= 0)
		return ;
	buf[ret] = '\0';
	Request &req = client[ev.ident];

	bool res = req.getContent(buf, *this);
	if (!res) {
		chlist.resize(chlist.size() + 1);
		EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		return ;
	}

	std::vector<Route> matches;
	for (routes_t::iterator it = routes.begin(); it != routes.end(); it++ ) {
		if (req.path.substr(0, it->second.path.length()) == it->second.path) {
			std::vector<Route>::iterator mit = matches.begin();
			for (; mit != matches.end(); mit++ ) {
				if (mit->path == it->second.path) {
					break ;
				}
			}
			if (mit != matches.end())
				continue;
			matches.push_back(it->second);
		}
	}

	std::sort(matches.begin(), matches.end(), sortByComplex);
	Route matchingRoute = *(matches.begin());

	if (req.content.raw.size() > matchingRoute.bodySizeLimit) 
		findCodeHandler(413, req);

	if (!req.content.isFullyParsed)
	 	return ;

	std::vector<std::string> a = matchingRoute.methods;
	if (matchingRoute.redirect.length()) {
		req.redirect(matchingRoute.redirect);
	} else if (std::find(a.begin(), a.end(), req.method) == a.end()) {
		findCodeHandler(405, req);
	} else if (req.method == "GET" || req.method == "POST" || req.method == "DELETE") {
		doResBasedOnReq(req, matchingRoute);
	} else
		findCodeHandler(501, req);
	chlist.resize(chlist.size() + 1);
	EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
}

void Server::setTimeOut(struct kevent &ev) {
	Request &req = client[ev.ident];

	findCodeHandler(408, req);
	chlist.resize(chlist.size() + 1);
	EV_SET(&*(chlist.end() - 1), ev.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
}

void Server::launch() {
	int evNb;

	init_server();
	std::cout << "WEBSERV launched." << std::endl;
	std::cout << this->host << ":" << this->port << std::endl;

	while (1) {
		evNb = kevent(kq, chlist.data(), chlist.size(), evlist, 1024, NULL);
		chlist.clear();
		if (evNb < 0 && errno == EINTR) // protection from CTRL + C (UNIX signal handling)
			return;
		for (int i = 0; i < evNb; i++) {
			struct kevent &ev = evlist[i];
			if (ev.flags & EV_EOF)
				disconnect(ev);
			else if (static_cast<int>(ev.ident) == sockfd)
				acceptConnection(ev);
			else if (ev.flags & EV_CLEAR)
			 	setTimeOut(ev);
			else if (ev.filter & EVFILT_READ && !client[ev.ident].res.ready)
				recvData(ev);
			else if (ev.filter & EVFILT_WRITE)
				sendData(ev);
		}
	}
}

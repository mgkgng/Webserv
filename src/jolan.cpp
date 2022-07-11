struct sortByComplex { 
	inline bool operator() (const Webserv::Route & struct1, const Webserv::Route & struct2) {
        return (struct1.getPath().length() > struct2.getPath().length());
    }
}sortByComplex;

std::string find_extension(std::string path) {
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

std::string get_file_from_path(std::string path, std::string route) {
	std::string ret = path.substr(route.length());
	if ((*ret.rbegin()) == '/') {
		ret.pop_back();
	} 
	return ret;
}

std::string get_file_full_path(std::string requested_file, std::string root) {
	char buf [MAXPATHLEN];
	getcwd(buf, MAXPATHLEN);
	if (root.find('.') == 0 && root.find('/') == 1) {
		root.replace(0, 1, buf);
	} else if (root.find('/') != 0) {
		root.insert(0, 1, '/');
		root.insert(0, buf);
	}
	if (root.find('/', root.length() - 1) == std::string::npos && requested_file.find('/') != 0) {
		root.push_back('/');
	}
	return root + requested_file;
}

bool check_if_file_exists(const std::string name) {
    std::fstream f(name.c_str());
    bool ret = f.good();
	f.close();
	return ret;
}

bool check_if_file_is_dir(const std::string name) {
   struct stat statbuf;
   if (stat(name.c_str(), &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

void	do_request_depending_on_file_type(const std::string file, const Webserv::Route & it) {
	if (check_if_file_exists(file)) {
		std::cout << "REQUESTED INDEX FILE" << std::endl;
	} else if (check_if_file_is_dir(file)) {
		if (it.getListingDirectory()) {
			std::cout << "REQUESTED DIRECTORY" << std::endl;
		} else if (it.getDirectoryFile() != "") {
			if (check_if_file_exists(get_file_full_path(it.getDirectoryFile(), it.getRoot()))) {
				std::cout << "REQUESTED FILEDIR" << std::endl;
			} else {
				throw Webserv::Request::ERROR404();
			}
		} else {
			throw Webserv::Request::ERROR403();
		}
	} else {
		throw Webserv::Request::ERROR404();
	}
}

bool endsWith(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool check_if_host_match(std::string s, std::string th) {
	if (s.find(":") != std::string::npos) {
		s.erase(s.find(":")); 
	}
	return s == th;
}

{
	/*for (; it_s != server.end(); it_s++) {
		try {
			std::string temp = this->headers.at("Host");
			if (this->headers.at("Host").find(":") != std::string::npos) {
				temp.erase(temp.find(":")); 
			}
			if ((*it_s->getHost().begin()) == '.') {
				if (!endsWith(temp, it_s->getHost()) && !endsWith(temp, "localhost")) {
					printf("888\n");
					throw Webserv::Request::ERROR400();
				} else {
					break;
				}
			} else {
				if (temp != it_s->getHost()) {
					printf("999\n");
					throw Webserv::Request::ERROR400();
				} else {
					break;
				}
			}
		} catch (std::out_of_range & e) {
			throw Webserv::Request::ERROR400();
		} catch (Webserv::Request::ERROR400 & e) {
			if (it_s + 1 == server.end()) {
				throw Webserv::Request::ERROR400();
			}
		}
	}
	
	std::string	target_host = it_s->getHost();
	std::vector<Webserv::Route> matches;

	for (; check_if_host_match(it_s->getHost(), target_host) && it_s != server.end(); it_s++) {
		std::map<std::string, Webserv::Route> route = it_s->getRoutes();
		for (std::map<std::string, Route>::iterator it = route.begin(); it != route.end(); it++ ) {
			if (this->path.substr(0, it->second.getPath().length()) == it->second.getPath()) {
				std::vector<Route>::iterator mit = matches.begin();
				for (; mit != matches.end(); mit++ ) {
					if (mit->getPath() == it->second.getPath()) {
						break ;
					}
				}
				if (mit != matches.end()) {
					continue;
				}
				matches.push_back(it->second);
			}
		}
	}

	// Sort in order from longest to shortest.
	std::sort(matches.begin(), matches.end(), sortByComplex);
	std::vector<Webserv::Route>::iterator it = matches.begin();
	std::vector<std::string> a = it->getAllowedHTTPMethods();
	if (std::find(a.begin(), a.end(), this->method) == a.end()) {
		throw ERROR405();
	} else if (this->method == "GET") {
		if (this->path.length() == it->getPath().length()) {
			std::string file = get_file_full_path(it->getIndex(), it->getRoot());
			printf("hello\n");
			do_request_depending_on_file_type(file, *it);
		} else {
			std::string extension = find_extension(this->path);
			std::string file = get_file_from_path(this->path, it->getPath());
			file = get_file_full_path(file, it->getRoot());
			std::cout << "full path: " << file << std::endl;
			std::cout << file << std::endl;
			if (extension == "") {
				do_request_depending_on_file_type(file, *it);
			} else if (extension == it->getPHPCGIExtension()) {
				do_request_depending_on_file_type(file, *it);
			} else if (extension == it->getPythonCGIExtension()) {
				do_request_depending_on_file_type(file, *it);
			} else {
				do_request_depending_on_file_type(file, *it);
			}
		}
	} else if (this->method == "POST") {

	} else if (this->method == "DELETE") {
		
	}*/
}

/*
	// Sort in order from longest to shortest.
	std::sort(matches.begin(), matches.end(), sortByComplex);
	std::vector<Webserv::Route>::iterator it = matches.begin();
	std::vector<std::string> a = it->getAllowedHTTPMethods();
	if (std::find(a.begin(), a.end(), this->method) == a.end()) {
		throw ERROR405();
	} else if (this->method == "GET") {
		if (this->path.length() == it->getPath().length()) {
			std::string file = get_file_full_path(it->getIndex(), it->getRoot());
			this->do_request_depending_on_file_type(file, *it);
		} else {
			std::string extension = find_extension(this->path);
			std::string file = get_file_from_path(this->path, it->getPath());
			std::cout << file << std::endl;
			file = get_file_full_path(file, it->getRoot());
			std::cout << file << std::endl;
			if (extension == "") {
				this->do_request_depending_on_file_type(file, *it);
			} else if (extension == it->getPHPCGIExtension()) {
				this->do_request_depending_on_file_type(file, *it);
			} else if (extension == it->getPythonCGIExtension()) {
				this->do_request_depending_on_file_type(file, *it);
			} else {
				this->do_request_depending_on_file_type(file, *it);
			}
		}
	} else if (this->method == "POST") {
		if (this->path.length() == it->getPath().length()) {
			std::string file = get_file_full_path(it->getIndex(), it->getRoot());
			this->do_request_depending_on_file_type(file, *it);
		} else {
			std::string extension = find_extension(this->path);
			std::string file = get_file_from_path(this->path, it->getPath());
			std::cout << file << std::endl;
			file = get_file_full_path(file, it->getRoot());
			std::cout << file << std::endl;
			if (extension == "") {
				this->do_request_depending_on_file_type(file, *it);
			} else if (extension == it->getPHPCGIExtension()) {
				this->do_request_depending_on_file_type(file, *it);
			} else if (extension == it->getPythonCGIExtension()) {
				this->do_request_depending_on_file_type(file, *it);
			} else {
				this->do_request_depending_on_file_type(file, *it);
			}
		}
	} else if (this->method == "DELETE") {
		
	}
}

void	Request::do_request_depending_on_file_type(const std::string file, const Webserv::Route & it) {
	std::cout << "GOT " << this->method << " REQUEST" << std::endl;
	if (check_if_file_exists(file)) {
		std::cout << "REQUESTED INDEX FILE" << std::endl;
	} else if (check_if_file_is_dir(file)) {
		if (it.getListingDirectory()) {
			std::cout << "REQUESTED DIRECTORY" << std::endl;
		} else if (it.getDirectoryFile() != "") {
			if (check_if_file_exists(get_file_full_path(it.getDirectoryFile(), it.getRoot()))) {
				std::cout << "REQUESTED FILEDIR" << std::endl;
			} else {
				throw Webserv::Request::ERROR404();
			}
		} else {
			throw Webserv::Request::ERROR403();
		}
	} else {
		throw Webserv::Request::ERROR404();
	}
}*/
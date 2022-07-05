#include <libft.hpp>
#include <Webserv.hpp>

#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

std::vector<std::string> ft_split(std::string str, std::string sep) {
	char *substr;
	std::vector<std::string> res;

	substr = strtok(const_cast<char *>(str.c_str()), sep.c_str());
	while (substr) {
		res.push_back(std::string(substr));
		substr = strtok(const_cast<char *>(str.c_str()), sep.c_str());
	}
	return (res);
}

std::string	ft_trim(std::string s, std::string set) {
	for (std::string::iterator c = s.begin(); c != s.end() && set.find(*c, 0) != std::string::npos; c++)
		s.erase(c);
	for (std::string::iterator c = s.end() - 1; c != s.begin() && set.find(*c, 0) != std::string::npos; c--)
		s.erase(c);
	return (s);
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

bool endsWith(std::string const &str, std::string const &suffix) {
	if (str.length() < suffix.length()) {
		return false;
	}
	return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

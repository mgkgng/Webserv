#include "libft.hpp"

bool exist(const std::string& fname) {
  struct stat buffer;
  return (stat(fname.c_str(), &buffer) == 0);
}

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
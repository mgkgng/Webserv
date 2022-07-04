#include "libft.hpp"

bool exist(const std::string& fname) {
  struct stat buffer;
  return (stat(fname.c_str(), &buffer) == 0);
}


std::vector<std::string> split(std::string s, std::string sep) {
	size_t start = 0, end, sep_len = sep.length();
	std::string ss;
	std::vector<std::string> res;

	while ((end = s.find(sep, start)) != std::string::npos) {
		ss = s.substr(start, end - start);
		start = end + sep_len;
		res.push_back(ss);
	}
	res.push_back(s.substr(start));
	return (res);
}

std::string	ft_trim(std::string s, std::string set) {
	for (std::string::iterator c = s.begin(); c != s.end() && set.find(*c, 0) != std::string::npos; c++)
		s.erase(c);
	for (std::string::iterator c = s.end() - 1; c != s.begin() && set.find(*c, 0) != std::string::npos; c--)
		s.erase(c);
	return (s);
}

const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string &s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string & s) {
	return rtrim(ltrim(s));
}
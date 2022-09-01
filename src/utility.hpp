#pragma once

// C++ libraries
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <exception>
#include <cassert>
#include <cerrno>
#include <vector>
#include <thread>
#include <utility>
#include <sstream>
#include <map>
#include <iterator>
#include <cstdarg>
#include <algorithm>

// C libraries
#include <sys/socket.h> 
#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <netdb.h>
#include <pthread.h>
#include <string.h>

#define WHITESPACE " \n\r\t\f\v"

using std::string;
typedef const std::string const_string;

bool exist(const std::string& fname) {
  struct stat buffer;
  return (stat(fname.c_str(), &buffer) == 0);
}

std::vector<std::string> split(std::string s, std::string sep) {

	size_t start = 0, end, sep_len = sep.length();
	std::vector<std::string> res;

    while ((end = s.find(sep, start)) != std::string::npos) {
        if (start != end)
            res.push_back(s.substr(start, end - start));
        start = end + sep_len;
    }
    if (start < s.length())
	    res.push_back(s.substr(start));
	return (res);
}

std::string	trim(std::string s, std::string set) {
	for (std::string::iterator c = s.begin(); c != s.end() && set.find(*c, 0) != std::string::npos; c++)
		s.erase(c);
	for (std::string::iterator c = s.end() - 1; c != s.begin() && set.find(*c, 0) != std::string::npos; c--)
		s.erase(c);
	return (s);
}

bool start_with(std::string s1, std::string s2) {
    return (!std::strncmp(s1.c_str(), s2.c_str(), s2.length()) ? true : false); 
}

bool        end_with(const_string &str, const_string &end)
{
    return end.size() > str.size() ? false : (str.substr(str.size() - end.size()) == end);
}

// https://www.delftstack.com/howto/cpp/get-current-directory-cpp/
string current_working_directory() {
    char dir[PATH_MAX];
    if (getwd(dir) != NULL)
        return string(dir);
    return "";
}

// Should be called in server, with a cgi (map set in route/config) and uri
// If true call the execute_CGI
string      is_CGI(const std::map<string, string> &cgi, const_string &file)
{
    for (std::map<string, string>::const_iterator iter = cgi.begin(); iter != cgi.end(); iter++)
        if (end_with(file, iter->first))
            return iter->second;
    return NULL;
}

string     to_lower_string(string str)
{
    for (string::iterator it = str.begin(); it != str.end(); it++)
        *it = tolower(*it);
    return (str);
}

string     to_upper_string(string str)
{
    for (string::iterator it = str.begin(); it != str.end(); it++)
        *it = toupper(*it);
    return (str);
}

// http://www.cplusplus.com/articles/D9j2Nwbp/
template<typename T>
string to_string(const T &status_code)
{  
	std::ostringstream in_string;
	in_string << status_code;
	return (in_string.str());
}

string replace_all_occurrency(string s, const_string &to_replace, const_string &replace)
{
    size_t  current = 0;
    while (true)
    {
        size_t  found = s.find(to_replace, current);
        if (found == string::npos) break;
        s.replace(found, to_replace.size(), replace);
        current = found + replace.size();
    }
    return (s);
}

template<typename Elem, typename Type>
bool    is_found_in(Elem elem, size_t n, Type first ...)
{
        va_list         args;
        va_start(args, first);

        while (n--)
        {
            if (elem == first) {
                va_end(args);
                return (true);
            }
            first = va_arg(args, Type);
        }
        va_end(args);
        return (false);
}

template<typename T>
inline bool includes(std::vector<T> vec, T elem) {
    return (std::find(vec.begin(), vec.end(), elem) != vec.end()) ? true : false;
}
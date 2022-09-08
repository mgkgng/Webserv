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
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>

#include "Route.hpp"
#include "mime.hpp"

typedef std::map<std::string, Route>	routes_t;

#define WHITESPACE " \n\r\t\f\v"
#define DEFAULT_ERROR_FILE "www/error.html"
#define AUTOINDEX_TEMPLATE_FILE "www/autoindex.html"
#define READFILE_BUF 2048

using std::string;
typedef const std::string const_string;

bool exist(const string& fname) {
  struct stat buffer;
  return (stat(fname.c_str(), &buffer) == 0);
}

std::vector<string> split(string s, string sep) {

	size_t start = 0, end, sep_len = sep.length();
	std::vector<string> res;

    while ((end = s.find(sep, start)) != string::npos) {
        if (start != end)
            res.push_back(s.substr(start, end - start));
        start = end + sep_len;
    }
    if (start < s.length())
	    res.push_back(s.substr(start));
	return (res);
}

string	trim(string s, string set) {
	for (string::iterator c = s.begin(); c != s.end() && set.find(*c, 0) != string::npos; c++)
		s.erase(c);
	for (string::iterator c = s.end() - 1; c != s.begin() && set.find(*c, 0) != string::npos; c--)
		s.erase(c);
	return (s);
}

inline bool start_with(const_string &str, const_string &start) {
    return (!std::strncmp(str.c_str(), start.c_str(), start.length()) ? true : false); 
}

inline bool end_with(const_string &str, const_string &end) {
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
inline bool      is_CGI(const_string &path)
{
    return (start_with(path, "/cgi/php/") || start_with(path, "/cgi/python")) ? true : false;
}

inline bool      is_autoindex_on(const_string &path, routes_t routes) {
    return (routes[path].autoindex);
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

// AUTOINDEX
// Basically, the entire template page is turned into a string
// That way, it can be easily manipulated when needed (more info: response)
string	file_to_string(const_string &filename){
	std::ifstream f;
	string s;
	char buf[READFILE_BUF + 1];
	f.open(filename.c_str());

	while (f) {
		f.read(buf, READFILE_BUF);
        // gcount return the last char read, so put a \0 at the end
		buf[f.gcount()] = '\0';
		s += buf;
	}
	return (s);
}

string	file_size(size_t size) {
	std::ostringstream	ss;
	float fsize = size;
	int i = 0;
	const_string	units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	while (fsize > 1024 && ++i) fsize /= 1024;
	ss << roundf(fsize * 10) / 10 << units[i];
	return (ss.str());
}

// Stock the string page, size_t to split in three (head, body, bottom)
const_string autoidx = file_to_string(AUTOINDEX_TEMPLATE_FILE);
const size_t autoidx_begin = autoidx.find("{{") == string::npos ? autoidx.size() - 2 : autoidx.find("{{");
const size_t autoidx_end = autoidx.find("}}") == string::npos ? autoidx.size() - 2 : autoidx.find("}}");
const_string autoidx_head = autoidx.substr(0, autoidx_begin);
const_string autoidx_body = autoidx.substr(autoidx_begin + 2, autoidx_end - autoidx_begin - 2);
const_string autoidx_bottom = autoidx.substr(autoidx_end + 2);
const_string custom_error = file_to_string(DEFAULT_ERROR_FILE);
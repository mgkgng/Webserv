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

bool exist(const string& fname);

std::vector<string> split(string s, string sep);

string	trim(string s, string set);

bool start_with(const_string &str, const_string &start);

bool end_with(const_string &str, const_string &end);

// https://www.delftstack.com/howto/cpp/get-current-directory-cpp/
string current_working_directory();

string to_lower_string(string str);

string to_upper_string(string str);

string replace_all_occurrency(string s, const_string &to_replace, const_string &replace);

// AUTOINDEX
// Basically, the entire template page is turned into a string
// That way, it can be easily manipulated when needed (more info: response)
string	file_to_string(const_string &filename);

string	file_size(size_t size);

// Stock the string page, size_t to split in three (head, body, bottom)
const_string autoidx = file_to_string(AUTOINDEX_TEMPLATE_FILE);
const size_t autoidx_begin = autoidx.find("{{") == string::npos ? autoidx.size() - 2 : autoidx.find("{{");
const size_t autoidx_end = autoidx.find("}}") == string::npos ? autoidx.size() - 2 : autoidx.find("}}");
const_string autoidx_head = autoidx.substr(0, autoidx_begin);
const_string autoidx_body = autoidx.substr(autoidx_begin + 2, autoidx_end - autoidx_begin - 2);
const_string autoidx_bottom = autoidx.substr(autoidx_end + 2);
const_string custom_error = file_to_string(DEFAULT_ERROR_FILE);

std::string ltrim(const std::string &s);
 
std::string rtrim(const std::string &s);

std::string trim(const std::string & s);

bool check_if_file_exists(const std::string name);

bool check_if_file_is_dir(const std::string name);

std::string statusCodeToString(int statCode);

#include "utility.hpp"
#include "statusCodes.hpp"

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

bool start_with(const_string &str, const_string &start) {
    return (!std::strncmp(str.c_str(), start.c_str(), start.length()) ? true : false); 
}

bool end_with(const_string &str, const_string &end) {
    return end.size() > str.size() ? false : (str.substr(str.size() - end.size()) == end);
}

string current_working_directory() {
    char dir[PATH_MAX];
    if (getwd(dir) != NULL)
        return string(dir);
    return "";
}

string to_lower_string(string str) {
    for (string::iterator it = str.begin(); it != str.end(); it++)
        *it = tolower(*it);
    return (str);
}

string to_upper_string(string str) {
    for (string::iterator it = str.begin(); it != str.end(); it++)
        *it = toupper(*it);
    return (str);
}

string replace_all_occurrency(string s, const_string &to_replace, const_string &replace) {
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

string	file_to_string(const_string &filename) {
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

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string & s) {
	return rtrim(ltrim(s));
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

std::string statusCodeToString(int statCode) {
	return (std::to_string(statCode) + " " + statusCode.getError(statCode));
}

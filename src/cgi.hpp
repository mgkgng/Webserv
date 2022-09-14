#pragma once

#include "ReqRes.hpp"
#include "utility.hpp"

class Request;

struct CGI_Environment {
    std::vector<char *> variable;
	
    char **env_to_execve() const;
    void name_formatter(string &name);
    void is_special_case(string &name);
    void add_variable(string name, string val);
};

// Call that after is_CGI, look at utility is_CGI
void execute_cgi(Request &req, bool type);
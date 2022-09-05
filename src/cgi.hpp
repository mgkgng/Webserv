#pragma once

#include "utility.hpp"
#include "Request.hpp"


struct CGI_Environment{

    std::vector<char *> variable;

    char **env_to_execve() const 
    {
        char **env_execveable = new char *[variable.size() + 1];
        size_t i;
        for (i = 0; i < variable.size(); i++)
            env_execveable[i] = variable[i];
        env_execveable[i] = '\0';
        return env_execveable;
    }

    void name_formatter(string &name)
    {
        name = to_upper_string(name);
        name = replace_all_occurrency(name, "-", "_");
    }

    void is_special_case(string &name)
    {
        if (is_found_in(name, 5, "ACCEPT", "ACCEPT_LANGUAGE", "USER_AGENT", "COOKIE", "REFERER"))
			name = "HTTP_" + name;
		else if (is_found_in(name, 1, "HOST"))
			name = "REMOTE_" + name;
    }

    void add_variable(string name, string val)
    {
        name_formatter(name);
        is_special_case(name);
        string tmp = name + '=' + val;
        variable.push_back(strdup(tmp.c_str()));
    }
};

// Call that after is_CGI, look at utility is_CGI
void execute_cgi(const std::map<int, string> &status_code, Request &request, const_string &uri, const_string &cgi, const_string &path)
{
    int fd_request[2], fd_response[2];
    pipe(fd_response);

    int pid = fork();
    if (pid)
    {
        close(fd_response[1]);
        // https://man7.org/linux/man-pages/man2/fcntl.2.html
        // Fcntl: manipulate file descriptor
        // F_SETFL: Set the file status flags to the value specified by arg.
        // O_NONBLOCK: Nonblock.
        // Actually, the subject impose to use fcntl like that
        fcntl(fd_response[0], F_SETFL, O_NONBLOCK);
        // "Something" to be changed, obv. Ideally, we set headers and stuffs
        // response.set_whatweset("HTTP/1.1 200 OK\r\n", fd_response[0], &status_code, pid, true); // ---> TODO
    }
    else 
    {
        pipe(fd_request);
        // Apparently, according to Mathias, the easiest way to manage is just to write content and parse/compose in req/res
        write(fd_request[1], request.content.raw.c_str(), request.content.raw.length());
        close(fd_request[1]), close(fd_response[0]);

        CGI_Environment environment;

        for (std::map<string, string>::const_iterator iter = request.headers.begin(); iter != request.headers.end(); iter++)
            environment.add_variable(iter->first, iter->second);

        // We set the env variable needed; must update with the actual source
        environment.add_variable("REQUEST_METHOD", request.method); // -> Member of Request
		// environment.add_variable("PATH_TRANSLATED", uri); // -> Argument from function call
		// environment.add_variable("SCRIPT_NAME", request.url); // -> Member of Request
		// environment.add_variable("QUERY_STRING", request.query); // -> Member of Request
		// environment.add_variable("PATH_INFO", path); // -> Argument from function call

        dup2(fd_request[0], 0), close(fd_request[0]);
        dup2(fd_response[1], 1), close(fd_response[1]);

        char *execve_av[] = {
            const_cast<char *>(cgi.c_str()), 
            const_cast<char *>(uri.c_str()), 
            NULL
        };

        // https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db
        execve(cgi.c_str(), execve_av, environment.env_to_execve());
        close(0), close(1);
        exit(1);
    }
}
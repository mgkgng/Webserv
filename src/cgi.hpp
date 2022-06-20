#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <map>

using std::string;
typedef const std::string const_string;

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
        if (is_found_in(key, ?, "ENV THAT REQUIRE HTTP_"))
			key = "HTTP_" + key;
		else if (is_found_in(key, 1, "HOST"))
			key = "REMOTE_" + key;
    }

    void add_variable(string name, string val)
    {
        name_formatter(name);
        is_special_case(name);
        string tmp = name + '=' + val;
        variable.push_back(strdup(tmp.c_str()));
    }
};

// In the prototype, everything is theorical
void execute_cgi(const std::map<int, string> &status_code, Request &request, const_string &uri, const_string &cgi, const_string &path)
{
    int req_fd[2], res_fd[2];
    pipe(res_fd);

    int pid = fork();
    if (pid)
    {
        close(res_fd[1]);
        // https://man7.org/linux/man-pages/man2/fcntl.2.html
        // Fcntl: manipulate file descriptor
        // F_SETFL: Set the file status flags to the value specified by arg.
        // O_NONBLOCK: Nonblock.
        // Actually, the subject impose to use fcntl like that
        fcntl(res_fd[0], F_SETFL, O_NONBLOCK);
        // "Something" to be changed, obv. Ideally, we set headers and stuffs
        // response.set_whatweset("HTTP/1.1 200 OK\r\n", res_fd[0], &status_code, pid, true);
    }
    else 
    {
        pipe(req_fd);
        // Still no ide what we write
        write(req_fd[1], what_we_write, len_of_what_we_write);
        close(req_fd[1]), close(res_fd[0]);

        CGI_Environment environment;

        // We set the env variable needed; must update with the actual source
        //environment.add_variable("???"", ???);

        dup2(req_fd[0], 0), close(req_fd[0]);
        dup2(res_fd[1], 1), close(res_fd[1]);

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
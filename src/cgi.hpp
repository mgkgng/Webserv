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
        env_execveable[i] = 0;
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
void execute_cgi(Request &req)
{
    int fd_req[2], fd_response[2];
    pipe(fd_response);

    int pid = fork();
    if (!pid) 
    {
        pipe(fd_req);
        // Apparently, according to Mathias, the easiest way to manage is just to write content and parse/compose in req/res
        write(fd_req[1], req.content.raw.c_str(), req.content.raw.length());
        close(fd_req[1]), close(fd_response[0]);

        CGI_Environment environment;

        for (std::map<string, string>::const_iterator iter = req.headers.begin(); iter != req.headers.end(); iter++)
            environment.add_variable(iter->first, iter->second);


        std::string cgi = (end_with(req.path, "php")) ? "/usr/bin/php" : "/usr/local/bin/python3";
        std::string uri = "/Users/sspina/Music/Webserv/www" + req.path;

        std::cout << req.path << std::endl;
        // We set the env variable needed; must update with the actual source
        environment.add_variable("REQUEST_METHOD", req.method); // -> Member of req
		environment.add_variable("PATH_TRANSLATED", uri); // -> Argument from function call
		environment.add_variable("SCRIPT_NAME", req.path); // -> Member of req
		environment.add_variable("QUERY_STRING", ""); // -> Member of req
		environment.add_variable("PATH_INFO", "/"); // -> Argument from function call
        // std::cerr << "===== TEST ARGUMENTS CGI =====\n"
		// 	<< "req.type " << req.method << "\n"
		// 	<< "uri " << uri << "\n"
		// 	<< "req.url " << req.path << "\n"
		// 	<< "req.query " << "" << "\n" 
		// 	<< "path_info " << "/" << "\n"
		// << "===== END TEST ARGUMENTS CGI =====" << std::endl;


        dup2(fd_req[0], 0), close(fd_req[0]);
        dup2(fd_response[1], 1), close(fd_response[1]);

        std::cerr << "execve arguments:\nCgi " << cgi << "\nUri: " << uri << std::endl;
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
    else 
    {
        close(fd_response[1]);
        // https://man7.org/linux/man-pages/man2/fcntl.2.html
        // Fcntl: manipulate file descriptor
        // F_SETFL: Set the file status flags to the value specified by arg.
        // O_NONBLOCK: Nonblock.
        // Actually, the subject impose to use fcntl like that
        fcntl(fd_response[0], F_SETFL, O_NONBLOCK);
        // === TODO ===
        // "Something" to be changed, obv. Ideally, we set headers and stuffs
        // response.set_whatweset("HTTP/1.1 200 OK\r\n", fd_response[0], &status_code, pid, true);
        // char buf[10000];
        req.res.protocolVer = "HTTP/1.1";
        req.res.status = statusCodeToString(Ok);
        req.res.body = "";
        req.res.ready = true;
        std::cout << "HERE IN CGI FD: " << fd_response[0] << std::endl;
        req.res.cgi_fd = fd_response[0];
        req.res.cgi_pid = pid;
        //req.res.headers = std::map<string, string>();
    }
    
}
#pragma once

#include "utility.hpp"
#include "statusCodes.hpp"
#include "Route.hpp"
#include "Server.hpp"

class Response {
	public:
		string 						body, statMsg, protocolVer;
		std::map<string, string>	headers;
		unsigned int				statCode;
		bool						ready;
		size_t						sendBits;

		Response() : ready(false), sendBits(0) {}
		~Response() {}

		string	getStr() {
			string res;

			res += this->protocolVer + ' ' + std::to_string(this->statCode) + ' ' + this->statMsg + ' ' + "\r\n";
			for (std::map<string, string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
				res += it->first + ": " + it->second + "\r\n";
			res += '\n';
			res += this->body;
			return (res);
		}

		void clean() {
			this->protocolVer = "";
			this->statMsg = "";
			this->headers.clear();
			this->body = "";
			this->ready = false;
			this->sendBits = 0;
		}

		void replaceAutoIndexBody(string &body, const_string &url, char *date, struct stat &statistics, struct dirent &current_directory) {
			body = replace_all_occurrency(autoidx_body, "$NAME", current_directory.d_name);
			body = replace_all_occurrency(body, "$URL", url + current_directory.d_name),
			body = replace_all_occurrency(body, "$DATE", string(date)),
			body = replace_all_occurrency(body, "$SIZE", file_size(statistics.st_size)),
			body = replace_all_occurrency(body, "$ISDIR", statistics.st_mode & S_IFDIR ? "1" : "");
		}

		// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
		// Basically we look into our page, take the top part, then parse all the file in a while
		// For each file found the "body" part will be appended with the needed data
		// Then we append the bottom and the html page is finished
		// TODO @Minguk: How can server retrieve the Path?
		string putAutoIndex(const_string &url, const_string &path = "/Users/sspina/Music/Webserv/www/error_pages") {
			DIR *directory;
			// https://stackoverflow.com/questions/12991334/members-of-dirent-structure
			struct dirent *current_directory;
			string tmp, autoidxpage = autoidx_head;

			if (!(directory = opendir(path.c_str())))
				return "";
			while ((current_directory = readdir(directory))) {
				if (!strcmp(current_directory->d_name, ".") || (!strcmp(current_directory->d_name, "..") && url == "/"))
					continue;
				
				// https://pubs.opengroup.org/onlinepubs/007908799/xsh/sysstat.h.html
				struct stat statistics;
				char date[128];
				stat((path + "/" + current_directory->d_name).c_str(), &statistics);
				strftime(date, 128, "%d %h %Y", localtime(&statistics.st_ctime));
				replaceAutoIndexBody(tmp, url, date, statistics, *current_directory);
				autoidxpage += tmp;
			}
			closedir(directory);
			autoidxpage += autoidx_bottom;
			return autoidxpage;
		}
};

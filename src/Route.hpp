#pragma once

class Route {
	public:
		std::string					index;
		std::string					root;
		unsigned int				bodySizeLimit;
		std::vector<std::string>	methods;
		bool						autoindex;
		std::string					redirect;
		std::string					cgiExtension;
		std::string					cgiPath;

		Route() {}
		~Route() {}
};

std::ostream &operator<<(std::ostream &os, Route &route)
{
	os << "index: " << route.index << std::endl;
	os << "root: " << route.root << std::endl;
	os << "bodysize limit: " << route.bodySizeLimit << std::endl;
	for (std::vector<std::string>::iterator it = route.methods.begin(); it != route.methods.end(); it++)
		os << "method: " << *it << std::endl;
	os << "autoindex: " << route.autoindex << std::endl;
	os << "redirect: " << route.redirect << std::endl;
	os << "cgi extension: " << route.cgiExtension << std::endl;
	os << "cgi path: " << route.cgiPath << std::endl; 
	return (os);
}
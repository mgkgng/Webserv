#pragma once

using std::string;

class Route {
	public:
		string				index, root, redirect, pythoncgiExtension, perlcgiExtension, cgiPath, rootFile, uploadRoot, path;
		unsigned int		bodySizeLimit;
		std::vector<string>	methods;
		bool				autoindex, uploadable;

		Route() : autoindex(false) {}
		Route(bool autoindex, string & rootFile, bool uploadable, string & uploadRoot, string & index, string & root, string & path, string & redirect, unsigned int bodySizeLimit, std::vector<string> & methods, string & pythoncgiExtension, string & perlcgiExtension) : 
			index(index),
			root(root), 
			redirect(redirect),
			pythoncgiExtension(pythoncgiExtension),
			perlcgiExtension(perlcgiExtension), 
			rootFile(rootFile),
			uploadRoot(uploadRoot),
			path(path),
			bodySizeLimit(bodySizeLimit),
			methods(methods),
			autoindex(autoindex), 
			uploadable(uploadable) {}
		~Route() {}
};

// std::ostream &operator<<(std::ostream &os, Route &route)
// {
// 	os << "index: " << route.index << std::endl;
// 	os << "root: " << route.root << std::endl;
// 	os << "bodysize limit: " << route.bodySizeLimit << std::endl;
// 	for (std::vector<string>::iterator it = route.methods.begin(); it != route.methods.end(); it++)
// 		os << "method: " << *it << std::endl;
// 	os << "autoindex: " << route.autoindex << std::endl;
// 	os << "redirect: " << route.redirect << std::endl;
// 	os << "python cgi extension: " << route.pythoncgiExtension << std::endl;
// 	os << "perl cgi extension: " << route.perlcgiExtension << std::endl;
// 	os << "cgi path: " << route.cgiPath << std::endl; 
// 	return (os);
// }
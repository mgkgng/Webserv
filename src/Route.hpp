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
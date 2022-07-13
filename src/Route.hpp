#pragma once

class Route {
	public:
		std::string					index;
		std::string					root;
		int							bodySizeLimit;
		std::vector<std::string>	methods;
		bool						autoindex;

		Route() {}
		~Route() {}
};
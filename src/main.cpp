#include <JSON.hpp>
#include <Webserv.hpp>

#include <iostream>
#include <algorithm>

// void printtabs(int level) {
// 	for (int i = 0; i < level; i++) {
// 		std::cout << "\t";
// 	}
// }

// void printjsondata(JSON const & json, int level) {
// 	std::cout << "{" << std::endl;;
// 	for (auto const &pair: json.getStrings()) {
// 		printtabs(level + 1);
// 		std::cout << pair.first << ": " << pair.second << std::endl;
// 	}
// 	for (auto const &pair: json.getBooleans()) {
// 		printtabs(level + 1);
// 		std::cout << pair.first << ": " << pair.second << std::endl;
// 	}
// 	for (auto const &pair: json.getNumbers()) {
// 		printtabs(level + 1);
// 		std::cout << pair.first << ": " << pair.second << std::endl;
// 	}
// 	for (auto const &pair: json.getObjects()) {
// 		printtabs(level + 1);
// 		std::cout << pair.first << ": ";
// 		printjsondata(pair.second, level + 1);
// 	}
// 	printtabs(level);
// 	std::cout << "}" << std::endl;;
// }

int main() {
	try {
		JSON test = JSON("config/example.json");
		// printjsondata(test, 0);
		Webserv::makeServersFromJSON(test);
	} catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
}
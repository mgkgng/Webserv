#include <JSON.hpp>

#include <iostream>

int main() {
	try {
		JSON test = JSON("test.json");
		std::cout << test.getBooleans().at("test") << std::endl;
		std::cout << test.getBooleans().at("test2") << std::endl;
		
	} catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
}
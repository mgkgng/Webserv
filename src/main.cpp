#include <JSON.hpp>

#include <iostream>

int main() {
	try {
		JSON test = JSON("test.json");
		std::cout << test.getNumbers().at("nice") << std::endl;
		std::cout << test.getObjects().at("test").getNumbers().at("test2") << std::endl;
	} catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
}
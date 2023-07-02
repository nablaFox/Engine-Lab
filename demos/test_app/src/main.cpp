#include "../include/test_app.hpp"

#include <iostream>
#include <stdexcept>

int main() {
	TestApp app;

	try {
		app.initialize();
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
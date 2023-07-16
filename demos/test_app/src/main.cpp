#include "../include/test_app.hpp"

#include <iostream>
#include <stdexcept>

int main() {
	TestApp app;

	app.init();
	app.run();
	app.cleanup();

	return 0;
}
#include "../include/test_app.hpp"

TestApp::TestApp() {

}

TestApp::~TestApp() {

}

void TestApp::initialize() {
	engine.createGameWindow();
}

void TestApp::run() {
	while (!engine.isRunning()) {
		engine.processEvents();
	}
}

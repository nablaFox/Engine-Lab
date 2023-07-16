#pragma once

#include "../../include/vel_engine.hpp"

class TestApp {
public:
	TestApp() { };
	~TestApp() { };

	void init();
	void run();
	void cleanup();

protected:
	vel::VelEngine engine;

};
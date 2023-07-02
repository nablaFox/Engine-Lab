#pragma once

#include "../../include/vel_engine.hpp"

class TestApp {
public:
	TestApp();
	~TestApp();

	void initialize();
	void run();

protected:
	vel::VelEngine engine;

};
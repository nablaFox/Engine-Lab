#pragma once

#include "vel_window.hpp"

#include <memory>
#include <vector>
#include <string>

namespace vel {

struct GameEngineSettings {
	const char* windowTitle = "Engine Lab";
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;
};

const GameEngineSettings DefaultSettings; // TODO should be in VelEngine

class VelEngine {
public:
	VelEngine(const GameEngineSettings& settings = DefaultSettings);
	~VelEngine() {};

	VelEngine(const VelEngine&) = delete;
	VelEngine& operator=(const VelEngine&) = delete;

	void createGameWindow();
	void processEvents();

	bool isRunning() { return window->isOpen(); }

protected:
	std::unique_ptr<VelWindow> window;
};

}

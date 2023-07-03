#pragma once

#include "vel_window.hpp"
#include "vel_vulkan.hpp"

#include <memory>
#include <vector>
#include <string>

namespace vel {

struct GameEngineSettings {
	const char* windowTitle = "Engine Lab";
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;
};

const GameEngineSettings defaultSettings; // TODO should be in VelEngine

class VelEngine {
public:
	VelEngine(const GameEngineSettings& settings = defaultSettings);
	~VelEngine();

	VelEngine(const VelEngine&) = delete;
	VelEngine& operator=(const VelEngine&) = delete;

	void createGameWindow();
	void processEvents();

	bool isRunning() { return window.isOpen(); }

protected:
	VelWindow window;
	VelVulkan vulkan;
};

}

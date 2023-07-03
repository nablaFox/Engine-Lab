#include "../include/vel_engine.hpp"

namespace vel {

VelEngine::VelEngine(const GameEngineSettings& settings) :
	window{ settings.windowWidth, settings.windowHeight, settings.windowTitle },
	vulkan{ window } { }

VelEngine::~VelEngine() {
}

void VelEngine::createGameWindow() {
	window.initWindow();
	vulkan.initVulkan();
}

void VelEngine::processEvents() {
	window.pollEvents();
}

}
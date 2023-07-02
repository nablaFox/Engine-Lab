#include "../include/vel_engine.hpp"

namespace vel {

VelEngine::VelEngine(const GameEngineSettings& settings) {
	window = std::make_unique<VelWindow>(
		settings.windowWidth,
		settings.windowHeight,
		settings.windowTitle
	);
}

void VelEngine::createGameWindow() {
	window->initWindow();
}

void VelEngine::processEvents() {
	window->pollEvents();
}

}
#include "../include/vel_engine.hpp"

#include "assert.h"

namespace vel {

VelEngine::VelEngine() {
}

VelEngine::~VelEngine() {
	assert(gameWindow == nullptr);
	assert(keyboard == nullptr);
	assert(device == nullptr);
}

void VelEngine::init(const GameEngineSettings& settings) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	gameWindow = glfwCreateWindow(
		settings.windowWidth,
		settings.windowHeight,
		settings.windowTitle,
		nullptr,
		nullptr);

	windowExtent.width = settings.windowWidth;
	windowExtent.height = settings.windowHeight;

	keyboard = new VelKeyboard(); // TEMP
	keyboard->initKeyboard(gameWindow);

	device = new VelDevice(); // TEMP
	device->initDevice(gameWindow, windowExtent);

	isInitialized = true;
}

void VelEngine::processEvents() {
	if (gameWindow == nullptr) return;
	glfwPollEvents();
}

void VelEngine::draw() {
	device->draw(frameNumber);
	frameNumber++;
}

void VelEngine::destroy() {
	if (!isInitialized) return;

	device->destroyDevice();

	delete keyboard;

	delete device;

	glfwDestroyWindow(gameWindow);
	glfwTerminate();

	device = nullptr;
	keyboard = nullptr;
	gameWindow = nullptr;
}

bool VelEngine::isKeyDown(int key) {
	if (keyboard == nullptr) return false;
	return keyboard->isKeyDown(key);
}

}
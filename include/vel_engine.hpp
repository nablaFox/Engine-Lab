#pragma once

#include "vel_core.hpp"

namespace vel {

struct GameEngineSettings {
	const char* windowTitle = "Engine Lab";
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;
};

const GameEngineSettings DefaultSettings;

class VelEngine {
public:
	VelEngine();
	~VelEngine();

	bool isInitialized{ false };
	int frameNumber{ 0 };
	int selectedShader{ 0 };

	void init(const GameEngineSettings& settings = DefaultSettings);
	void destroy();

	void processEvents();
	void draw(); // TEMP

	bool isOpen() { return !glfwWindowShouldClose(gameWindow); }
	bool isKeyDown(int key);
	bool wasKeyPressed(int key);

private:
	VelDevice* device{ nullptr };
	GLFWwindow* gameWindow{ nullptr };
	VelKeyboard* keyboard{ nullptr };

	VkExtent2D windowExtent;
};

}
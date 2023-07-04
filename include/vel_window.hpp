#pragma once

#define GLFW_INCLUDE_VULKAN // TEMP
#include <GLFW/glfw3.h>

namespace vel {

class VelWindow {
public:
	VelWindow(const uint32_t w, const uint32_t h, const char* name);
	~VelWindow();

	VelWindow(const VelWindow&) = delete;
	VelWindow& operator=(const VelWindow&) = delete;

	void initWindow();
	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	bool isOpen() { return glfwWindowShouldClose(window); }
	void pollEvents() { glfwPollEvents(); }
	VkExtent2D getExtent() { return { width, height }; }
	VkExtent2D getActualExtent();

private:
	uint32_t width;
	uint32_t height;
	const char* title;

	GLFWwindow* window;
};

}
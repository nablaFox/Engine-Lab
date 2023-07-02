#include "../include/vel_window.hpp"

#include <stdexcept>

namespace vel {

VelWindow::VelWindow(const uint32_t w, const uint32_t h, const char* name) : width(w), height(h), title(name) { }

VelWindow::~VelWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void VelWindow::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void VelWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

}
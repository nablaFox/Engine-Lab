#include "../include/vel_keyboard.hpp"

namespace vel {

void VelKeyboard::initKeyboard(GLFWwindow* window) {
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		VelKeyboard* keyboard = static_cast<VelKeyboard*>(glfwGetWindowUserPointer(window));
		keyboard->keyCallback(window, key, scancode, action, mods);
		});
}

bool VelKeyboard::isKeyDown(int key) {
	return keys.find(key) != keys.end() ? keys[key] : false;
}

void VelKeyboard::keyCallback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods) {

	keys[key] = (action == GLFW_PRESS);
}

}
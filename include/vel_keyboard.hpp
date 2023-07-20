#pragma once

#include <GLFW/glfw3.h>
#include <map>

namespace vel {

class VelKeyboard {
public:
	void initKeyboard(GLFWwindow* window);
	bool isKeyDown(int key);
	bool wasKeyPressed(int key);

private:
	std::map<int, bool> keys;

	void keyCallback(
		GLFWwindow* window,
		int key,
		int scancode,
		int action, int mods);
};

}
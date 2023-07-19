#include "../../include/vel_engine.hpp"

int main() {
	vel::VelEngine engine;

	engine.init();

	while (engine.isOpen()) {
		engine.processEvents();

		if (engine.isKeyDown(GLFW_KEY_ESCAPE)) break;

		engine.draw(); // TEMP
	}

	engine.destroy();

	return 0;
}
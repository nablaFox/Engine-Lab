#include "../../include/vel_engine.hpp"

int main() {
	vel::VelEngine engine;

	engine.init();

	while (engine.isOpen()) {
		engine.processEvents();

		if (engine.isKeyDown(GLFW_KEY_ESCAPE)) break;

		if (engine.wasKeyPressed(GLFW_KEY_SPACE)) {
			engine.selectedShader = !engine.selectedShader;
		}

		engine.draw();
	}

	engine.destroy();

	return 0;
}
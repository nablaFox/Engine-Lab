#pragma once

#include "vel_window.hpp" // TODO understand the correct dependency

#include <vector>
#include <unordered_set>
#include <string>

namespace vel {

class VelVulkan {
public:
	VelVulkan(VelWindow& window) : window{ window } {};
	~VelVulkan();

	void initVulkan();

private:
	void createInstance();
	std::vector<const char*> getRequiredExtensions();
	bool checkExtensionCompatibility(const std::vector<const char*>& requiredExtensions);
	std::unordered_set<std::string> getAvailableExtensions();

	VkInstance instance;
	VelWindow& window;
};


}
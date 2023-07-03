#pragma once

#include "vel_window.hpp" // TODO understand the correct dependency

#include <vector>
#include <unordered_set>
#include <string>

namespace vel {

class VelVulkan {
public:
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	VelVulkan(VelWindow& window) : window{ window } {};
	~VelVulkan();

	void initVulkan();

	// Not copyable or movable
	VelVulkan(const VelVulkan&) = delete;
	void operator=(const VelVulkan&) = delete;
	VelVulkan(VelVulkan&&) = delete;
	VelVulkan& operator=(VelVulkan&&) = delete;

private:
	void createInstance();
	void setupDebugMessenger();

	std::vector<const char*> getGlfwRequiredExtensions();
	bool checkExtensionSupport(const std::vector<const char*> requiredExtensions);
	bool checkValidationLayerSupport();

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VelWindow& window;

	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};


}
#pragma once

#include "vel_window.hpp"

#include <vector>
#include <unordered_set>
#include <string>
#include <optional>

namespace vel {

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VelVulkan {
public:
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	VelVulkan(VelWindow& window) : window{ window } {}
	~VelVulkan();

	void initVulkan();

	// Not copyable or movable
	VelVulkan(const VelVulkan&) = delete;
	void operator=(const VelVulkan&) = delete;
	VelVulkan(VelVulkan&&) = delete;
	VelVulkan& operator=(VelVulkan&&) = delete;

	VkPhysicalDeviceProperties getProperties() { return properties; }
	VkPhysicalDeviceFeatures getFeatures() { return features; }

private:
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createDevice();
	void createSwapChain();

	std::vector<const char*> getGlfwRequiredExtensions();
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails getSwapChainDetails(VkPhysicalDevice device);
	VkSurfaceFormatKHR getSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR getSwapPresentMode(
		const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D getSwapExtent(
		const VkSurfaceCapabilitiesKHR& capabilities);
	uint32_t getImageCount(
		const VkSurfaceCapabilitiesKHR& capabilities);

	bool checkExtensionSupport();
	bool checkValidationLayerSupport();
	bool isPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool checkPhysicalExtensionSupport(VkPhysicalDevice device);
	bool isSwapChainAdeguate(VkPhysicalDevice device);

private:
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	VelWindow& window;

	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceProperties properties;

	// handlers
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> physicalDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

}
#pragma once

#include "vel_window.hpp"
#include "vel_vulkan_pipeline.hpp"

#include <vector>
#include <unordered_set>
#include <string>
#include <optional>
#include <memory>

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

	// Not copyable or movable
	VelVulkan(const VelVulkan&) = delete;
	void operator=(const VelVulkan&) = delete;
	VelVulkan(VelVulkan&&) = delete;
	VelVulkan& operator=(VelVulkan&&) = delete;

	void initVulkan();
	void testDrawFrame(); // TEMP

	VkPhysicalDeviceProperties getProperties() { return properties; }
	VkPhysicalDeviceFeatures getFeatures() { return features; }

private:
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createDevice();
	void createSwapChain();
	void createImageViews();
	void createPipelineLayout();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFrameBuffers();
	void createCommandPool();
	void createCommandBuffer();
	void createSyncObjects();
	void recordCommandBuffer(
		VkCommandBuffer commandBuffer, uint32_t imageIndex
	);

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
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	std::unique_ptr<VelVulkanPipeline> graphicsPipeline;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	VelWindow& window;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceProperties properties;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	// synchronization
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> physicalDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

}
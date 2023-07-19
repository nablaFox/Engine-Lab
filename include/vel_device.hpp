#pragma once

#include "vel_core.hpp"

#include <vector>

namespace vel {

class VelDevice {
public:
	VelDevice();
	~VelDevice();

	void initDevice(GLFWwindow* gameWindow, VkExtent2D extent);
	void destroyDevice();

	void draw();

	int frameNumber{ 0 };

private:
	void initVulkan();
	void initSwapchain();
	void initDefaultRenderPass();
	void initFramebuffers();
	void initCommands();
	void initSyncStrucutres();

private:
	GLFWwindow* window;
	VkExtent2D windowExtent;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debug_messenger;
	VkPhysicalDevice chosenGPU;
	VkDevice device;

	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	VkFormat swapchainImageFormat;

	VkQueue graphicsQueue;
	uint32_t graphicsQueueFamily;

	VkRenderPass renderPass;

	VkCommandPool commandPool;
	VkCommandBuffer mainCommandBuffer;

	std::vector<VkFramebuffer> framebuffers;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;

	VkSemaphore presentSemaphore, renderSemaphore;
	VkFence renderFence;
};

}
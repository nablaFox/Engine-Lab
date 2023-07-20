#pragma once

#include "vel_core.hpp"
#include "vel_deletion_queue.hpp"

#include <vector>

namespace vel {

class VelDevice {
public:
	VelDevice();
	~VelDevice();

	void initDevice(GLFWwindow* gameWindow, VkExtent2D extent);
	void destroyDevice();

	void draw(int frameNumber, int selectedShader = 0);

private:
	void initVulkan();
	void initSwapchain();
	void initDefaultRenderPass();
	void initFramebuffers();
	void initCommands();
	void initSyncStrucutres();
	void initPipelines();

	VkShaderModule createShaderModule(const char* fileName);

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

	VkPipelineLayout trianglePipelineLayout;

	VkPipeline trianglePipeline;
	VkPipeline coloredTrianglePipeline;

	VkSemaphore presentSemaphore, renderSemaphore;
	VkFence renderFence;

	DeletionQueue mainDeletionQueue;
};

}
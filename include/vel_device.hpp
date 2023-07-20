#pragma once

#define VMA_IMPLEMENTATION;
#include "vel_core.hpp"
#include "vel_deletion_queue.hpp"
#include "vel_mesh.hpp"
#include "vel_pipeline.hpp"

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
	void initAllocator();
	void initSwapchain();
	void initDefaultRenderPass();
	void initFramebuffers();
	void initCommands();
	void initSyncStrucutres();
	void initPipelines();
	void loadMeshes();

	void initPipeline(
		const char* vertShaderPath,
		const char* fragShaderPath,
		VkPipeline* pipeline,
		VkPipelineLayout& pipelineLayout);

	void initMeshPipeline(
		const char* vertShaderPath,
		const char* fragShaderPath,
		VkPipeline* pipeline,
		VkPipelineLayout& pipelineLayout);

	VkShaderModule createShaderModule(const char* fileName);
	void uploadMesh(Mesh& mesh);

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
	VkPipelineLayout meshPipelineLayout;

	PipelineBuilder mainPipelineBuilder;

	VkPipeline trianglePipeline;
	VkPipeline coloredTrianglePipeline;
	VkPipeline meshPipeline;

	VkSemaphore presentSemaphore, renderSemaphore;
	VkFence renderFence;

	Mesh triangleMesh, violinMesh;

	VmaAllocator allocator;

	DeletionQueue mainDeletionQueue;
};

}
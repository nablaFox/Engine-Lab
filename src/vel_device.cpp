#include "../include/vel_device.hpp"
#include "../include/vel_vulkan_initializers.hpp"
#include "../include/vel_pipeline.hpp"
#include "../include/vel_file.hpp"

#include "VkBootstrap.h"

#include <assert.h>
#include <math.h>

namespace vel {

VelDevice::VelDevice() {
}

VelDevice::~VelDevice() {
	assert(window == nullptr);
}

void VelDevice::initDevice(GLFWwindow* gameWindow, VkExtent2D extent) {
	window = gameWindow;
	windowExtent = extent;

	initVulkan();

	initSwapchain();

	initDefaultRenderPass();

	initFramebuffers();

	initCommands();

	initSyncStrucutres();

	initPipelines();
}

void VelDevice::draw(int frameNumber) {
	VK_CHECK(vkWaitForFences(device, 1, &renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(device, 1, &renderFence));

	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(
		device,
		swapchain,
		1000000000,
		presentSemaphore,
		nullptr,
		&swapchainImageIndex));

	VK_CHECK(vkResetCommandBuffer(mainCommandBuffer, 0));

	VkCommandBuffer cmd = mainCommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo =
		vkinit::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	VkClearValue clearValue;
	float flash = abs(sin(frameNumber / 120.f));
	clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

	VkRenderPassBeginInfo rpInfo =
		vkinit::renderPassBeginInfo(renderPass, windowExtent, framebuffers[swapchainImageIndex]);

	rpInfo.clearValueCount = 1;
	rpInfo.pClearValues = &clearValue;

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	// triangle rendering
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline);
	vkCmdDraw(cmd, 3, 1, 0, 0);

	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit = vkinit::submitInfo(&cmd);
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &renderSemaphore;

	VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submit, renderFence));

	VkPresentInfoKHR presentInfo = vkinit::presentInfo();

	presentInfo.pSwapchains = &swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(graphicsQueue, &presentInfo));
}

void VelDevice::destroyDevice() {
	vkDeviceWaitIdle(device);

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyFence(device, renderFence, nullptr);
	vkDestroySemaphore(device, renderSemaphore, nullptr);
	vkDestroySemaphore(device, presentSemaphore, nullptr);

	vkDestroySwapchainKHR(device, swapchain, nullptr);

	vkDestroyRenderPass(device, renderPass, nullptr);

	for (size_t i = 0; i < framebuffers.size(); i++) {
		vkDestroyFramebuffer(device, framebuffers[i], nullptr);
		vkDestroyImageView(device, swapchainImageViews[i], nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);

	vkDestroyDevice(device, nullptr);
	vkb::destroy_debug_utils_messenger(instance, debug_messenger);
	vkDestroyInstance(instance, nullptr);

	window = nullptr;
}

void VelDevice::initVulkan() {
	vkb::InstanceBuilder builder;
	vkb::Instance vkbInstance = builder
		.set_app_name("Engine Lab") // TEMP
		.request_validation_layers(true)
		.require_api_version(1, 1, 0)
		.use_default_debug_messenger()
		.build()
		.value();

	instance = vkbInstance.instance;
	debug_messenger = vkbInstance.debug_messenger;

	VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));

	vkb::PhysicalDeviceSelector selector{ vkbInstance };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 1)
		.set_surface(surface)
		.select()
		.value();

	chosenGPU = physicalDevice.physical_device;

	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkb::Device vkbDevice = deviceBuilder
		.build()
		.value();

	device = vkbDevice.device;

	graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void VelDevice::initSwapchain() {
	vkb::SwapchainBuilder swapChainBuilder{ chosenGPU, device, surface };
	vkb::Swapchain vkbSwapchain = swapChainBuilder
		.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.build()
		.value();

	swapchain = vkbSwapchain.swapchain;
	swapchainImages = vkbSwapchain.get_images().value();
	swapchainImageViews = vkbSwapchain.get_image_views().value();

	swapchainImageFormat = vkbSwapchain.image_format;
}

void VelDevice::initDefaultRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));
}

void VelDevice::initFramebuffers() {
	VkFramebufferCreateInfo fbInfo = vkinit::framebufferCreateInfo(renderPass, windowExtent);

	const uint32_t swapChainImageCount = swapchainImages.size();
	framebuffers = std::vector<VkFramebuffer>(swapChainImageCount);

	for (size_t i = 0; i < swapChainImageCount; i++) {
		fbInfo.pAttachments = &swapchainImageViews[i];
		VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &framebuffers[i]));
	}
}

void VelDevice::initCommands() {
	VkCommandPoolCreateInfo commandPoolInfo =
		vkinit::commandPollCreateInfo(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool));

	VkCommandBufferAllocateInfo cmdAllocInfo =
		vkinit::commandBufferAllocateInfo(commandPool);

	VK_CHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &mainCommandBuffer));
}

void VelDevice::initSyncStrucutres() {
	VkFenceCreateInfo fenceCreateInfo = vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

	VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &renderFence));

	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphoreCreateInfo();

	VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &presentSemaphore));
	VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderSemaphore));
}

void VelDevice::initPipelines() {
	auto triangleVertexShader = createShaderModule("shaders/triangle.vert.spv");
	auto triangleFragShader = createShaderModule("shaders/triangle.frag.spv");

	auto pipelineLayoutInfo = vkinit::pipelineLayoutCreateInfo();
	VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &trianglePipelineLayout));

	PipelineBuilder pipelineBuilder;

	pipelineBuilder.shaderStages.push_back(
		vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader));

	pipelineBuilder.shaderStages.push_back(
		vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

	pipelineBuilder.vertexInputInfo = vkinit::vertexInputStateCreateInfo();

	pipelineBuilder.inputAssembly = vkinit::inputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	pipelineBuilder.viewport.x = 0.0f;
	pipelineBuilder.viewport.y = 0.0f;
	pipelineBuilder.viewport.width = (float)windowExtent.width;
	pipelineBuilder.viewport.height = (float)windowExtent.height;
	pipelineBuilder.viewport.minDepth = 0.0f;
	pipelineBuilder.viewport.maxDepth = 1.0f;

	pipelineBuilder.scissor.offset = { 0, 0 };
	pipelineBuilder.scissor.extent = windowExtent;

	pipelineBuilder.rasterizer = vkinit::rasterizationCreateInfo(VK_POLYGON_MODE_FILL);

	pipelineBuilder.multisampling = vkinit::multisamplingStateCreateInfo();

	pipelineBuilder.colorBlendAttachment = vkinit::colorBlendAttachmentState();

	pipelineBuilder.pipelineLayout = trianglePipelineLayout;

	trianglePipeline = pipelineBuilder.buildPipeline(device, renderPass);

	vkDestroyShaderModule(device, triangleVertexShader, nullptr);
	vkDestroyShaderModule(device, triangleFragShader, nullptr);
}

VkShaderModule VelDevice::createShaderModule(const char* fileName) {
	VelFile file{ fileName };

	if (!file.isOpen()) {
		std::cout << "Failed to load file: " << fileName << std::endl;
		std::abort();
	}

	std::cout << "File " << fileName << " successfully loaded" << std::endl;

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.codeSize = file.getFileLength();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(file.getData());

	VkShaderModule shaderModule;
	VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));

	return shaderModule;
}

}
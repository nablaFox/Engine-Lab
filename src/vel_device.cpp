#include "../include/vel_device.hpp"
#include "../include/vel_vulkan_initializers.hpp"
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

	initAllocator();

	initSwapchain();

	initDefaultRenderPass();

	initFramebuffers();

	initCommands();

	initSyncStrucutres();

	initPipelines();

	loadMeshes();
}

void VelDevice::draw(int frameNumber, int selectedShader) {
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

	//auto pipeline = selectedShader ? trianglePipeline : coloredTrianglePipeline;

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &triangleMesh.vertexBuffer.buffer, &offset);

	vkCmdDraw(cmd, triangleMesh.vertices.size(), 1, 0, 0);

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

	mainDeletionQueue.flush();

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

void VelDevice::initAllocator() {
	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.physicalDevice = chosenGPU;
	allocatorInfo.device = device;
	allocatorInfo.instance = instance;
	vmaCreateAllocator(&allocatorInfo, &allocator);

	VK_DELETE_PUSH(
		mainDeletionQueue,
		vmaDestroyAllocator(allocator));
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

	VK_DELETE_PUSH(
		mainDeletionQueue,
		vkDestroySwapchainKHR(device, swapchain, nullptr));
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

	VK_DELETE_PUSH(
		mainDeletionQueue,
		vkDestroyRenderPass(device, renderPass, nullptr));
}

void VelDevice::initFramebuffers() {
	VkFramebufferCreateInfo fbInfo = vkinit::framebufferCreateInfo(renderPass, windowExtent);

	const uint32_t swapChainImageCount = swapchainImages.size();
	framebuffers = std::vector<VkFramebuffer>(swapChainImageCount);

	for (size_t i = 0; i < swapChainImageCount; i++) {
		fbInfo.pAttachments = &swapchainImageViews[i];
		VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &framebuffers[i]));

		VK_DELETE_PUSH(
			mainDeletionQueue,
			vkDestroyFramebuffer(device, framebuffers[i], nullptr),
			vkDestroyImageView(device, swapchainImageViews[i], nullptr));
	}
}

void VelDevice::initCommands() {
	VkCommandPoolCreateInfo commandPoolInfo =
		vkinit::commandPollCreateInfo(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool));

	VkCommandBufferAllocateInfo cmdAllocInfo =
		vkinit::commandBufferAllocateInfo(commandPool);

	VK_CHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &mainCommandBuffer));

	VK_DELETE_PUSH(
		mainDeletionQueue,
		vkDestroyCommandPool(device, commandPool, nullptr));
}

void VelDevice::initSyncStrucutres() {
	VkFenceCreateInfo fenceCreateInfo = vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

	VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &renderFence));

	VK_DELETE_PUSH(mainDeletionQueue, vkDestroyFence(device, renderFence, nullptr));

	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphoreCreateInfo();

	VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &presentSemaphore));
	VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderSemaphore));

	VK_DELETE_PUSH(
		mainDeletionQueue,
		vkDestroySemaphore(device, presentSemaphore, nullptr),
		vkDestroySemaphore(device, renderSemaphore, nullptr));
}

void VelDevice::initPipelines() {
	mainPipelineBuilder.vertexInputInfo = vkinit::vertexInputStateCreateInfo();

	mainPipelineBuilder.inputAssembly = vkinit::inputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	mainPipelineBuilder.viewport.x = 0.0f;
	mainPipelineBuilder.viewport.y = 0.0f;
	mainPipelineBuilder.viewport.width = (float)windowExtent.width;
	mainPipelineBuilder.viewport.height = (float)windowExtent.height;
	mainPipelineBuilder.viewport.minDepth = 0.0f;
	mainPipelineBuilder.viewport.maxDepth = 1.0f;

	mainPipelineBuilder.scissor.offset = { 0, 0 };
	mainPipelineBuilder.scissor.extent = windowExtent;

	mainPipelineBuilder.rasterizer = vkinit::rasterizationCreateInfo(VK_POLYGON_MODE_FILL);

	mainPipelineBuilder.multisampling = vkinit::multisamplingStateCreateInfo();

	mainPipelineBuilder.colorBlendAttachment = vkinit::colorBlendAttachmentState();

	auto pipelineLayoutInfo = vkinit::pipelineLayoutCreateInfo();
	VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &trianglePipelineLayout));

	initPipeline(
		"shaders/triangle.vert.spv",
		"shaders/triangle.frag.spv",
		&trianglePipeline,
		trianglePipelineLayout);

	initPipeline(
		"shaders/colored_triangle.vert.spv",
		"shaders/colored_triangle.frag.spv",
		&coloredTrianglePipeline,
		trianglePipelineLayout);

	initMeshPipeline(
		"shaders/triangle_mesh.vert.spv",
		"shaders/colored_triangle.frag.spv",
		&meshPipeline,
		trianglePipelineLayout);

	VK_DELETE_PUSH(
		mainDeletionQueue,
		vkDestroyPipeline(device, meshPipeline, nullptr),
		vkDestroyPipeline(device, trianglePipeline, nullptr),
		vkDestroyPipeline(device, coloredTrianglePipeline, nullptr),
		vkDestroyPipelineLayout(device, trianglePipelineLayout, nullptr));
}

void VelDevice::loadMeshes() {
	triangleMesh.vertices.resize(3);

	triangleMesh.vertices[0].position = { 1.f,1.f, 0.0f };
	triangleMesh.vertices[1].position = { -1.f, 1.f, 0.0f };
	triangleMesh.vertices[2].position = { 0.f,-1.f, 0.0f };

	triangleMesh.vertices[0].color = { 0.f, 1.f, 0.0f }; //pure green
	triangleMesh.vertices[1].color = { 0.f, 1.f, 0.0f }; //pure green
	triangleMesh.vertices[2].color = { 0.f, 1.f, 0.0f }; //pure green

	uploadMesh(triangleMesh);
}

void VelDevice::initPipeline(
	const char* vertShaderPath,
	const char* fragShaderPath,
	VkPipeline* pipeline,
	VkPipelineLayout& pipelineLayout) {
	auto vertShader = createShaderModule(vertShaderPath);
	auto fragShader = createShaderModule(fragShaderPath);

	mainPipelineBuilder.shaderStages.push_back(
		vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShader));

	mainPipelineBuilder.shaderStages.push_back(
		vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader));

	mainPipelineBuilder.pipelineLayout = pipelineLayout;

	*pipeline = mainPipelineBuilder.buildPipeline(device, renderPass);

	mainPipelineBuilder.shaderStages.clear();

	vkDestroyShaderModule(device, fragShader, nullptr);
	vkDestroyShaderModule(device, vertShader, nullptr);
}

void VelDevice::initMeshPipeline(
	const char* vertShaderPath,
	const char* fragShaderPath,
	VkPipeline* pipeline,
	VkPipelineLayout& pipelineLayout) {
	VertexInputDescription vertexDescription = Vertex::getVertexDescription();

	mainPipelineBuilder.vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	mainPipelineBuilder.vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();

	mainPipelineBuilder.vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	mainPipelineBuilder.vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();

	initPipeline(vertShaderPath, fragShaderPath, pipeline, pipelineLayout);
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

void VelDevice::uploadMesh(Mesh& mesh) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

	bufferInfo.size = mesh.vertices.size() * sizeof(Vertex);
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	VmaAllocationCreateInfo vmaallocInfo{};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	VK_CHECK(vmaCreateBuffer(
		allocator,
		&bufferInfo,
		&vmaallocInfo,
		&mesh.vertexBuffer.buffer,
		&mesh.vertexBuffer.allocation,
		nullptr));

	VK_DELETE_PUSH(
		mainDeletionQueue,
		vmaDestroyBuffer(allocator, mesh.vertexBuffer.buffer, mesh.vertexBuffer.allocation));

	// pointer used to temporarily address the mapped memory
	void* data;

	// maps the allocated GPU memory into the CPU's address space 
	vmaMapMemory(allocator, mesh.vertexBuffer.allocation, &data);

	// copy the vertices inside the memory given by the data pointer
	memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));

	vmaUnmapMemory(allocator, mesh.vertexBuffer.allocation);
}

}
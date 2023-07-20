#pragma once

#include "vel_types.hpp"

namespace vkinit {

VkCommandPoolCreateInfo commandPollCreateInfo(
	uint32_t queueFamilyIndex,
	VkCommandPoolCreateFlags flags = 0);

VkCommandBufferAllocateInfo commandBufferAllocateInfo(
	VkCommandPool pool,
	uint32_t count = 1,
	VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkFramebufferCreateInfo framebufferCreateInfo(
	VkRenderPass renderPass,
	VkExtent2D extent);

VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);

VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0);

VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);

VkRenderPassBeginInfo renderPassBeginInfo(
	VkRenderPass renderPass,
	VkExtent2D windowExtent,
	VkFramebuffer framebuffer);

VkSubmitInfo submitInfo(VkCommandBuffer* cmd);

VkPresentInfoKHR presentInfo();

VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(
	VkShaderStageFlagBits stage,
	VkShaderModule shaderModule);

VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo();

VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo(VkPrimitiveTopology topology);

VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo(VkPolygonMode polygonMode);

VkPipelineMultisampleStateCreateInfo multisamplingStateCreateInfo();

VkPipelineColorBlendAttachmentState colorBlendAttachmentState();

VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo();

}
#include "../include/vel_vulkan_pipeline.hpp"
#include "../include/vel_utils.hpp"

#include <iostream>

namespace vel {

VelVulkanPipeline::VelVulkanPipeline(
	VkDevice& device,
	PipelineConfigInfo configInfo,
	const std::string vertFilePath,
	const std::string fragFilePath) :
	device{ device },
	configInfo{ configInfo },
	vertFilePath{ vertFilePath },
	fragFilePath{ fragFilePath } { }

VelVulkanPipeline::~VelVulkanPipeline() {
}

VelVulkanPipeline::PipelineConfigInfo VelVulkanPipeline::defaultPipelineConfigInfo(VkExtent2D extent) {
	PipelineConfigInfo configInfo{};

	// vertex input
	configInfo.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	configInfo.vertexInputInfo.vertexBindingDescriptionCount = 0;
	configInfo.vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
	configInfo.vertexInputInfo.vertexAttributeDescriptionCount = 0;
	configInfo.vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

	// input assembly
	configInfo.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	configInfo.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

	// viewport and scissors
	configInfo.viewport.x = 0.0f;
	configInfo.viewport.y = 0.0f;
	configInfo.viewport.width = (float)extent.width;
	configInfo.viewport.height = (float)extent.height;
	configInfo.viewport.minDepth = 0.0f;
	configInfo.viewport.maxDepth = 1.0f;

	configInfo.scissor.offset = { 0, 0 };
	configInfo.scissor.extent = extent;

	configInfo.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	configInfo.viewportState.viewportCount = 1;
	configInfo.viewportState.pViewports = &configInfo.viewport;
	configInfo.viewportState.scissorCount = 1;
	configInfo.viewportState.pScissors = &configInfo.scissor;

	// rasterizer
	configInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	configInfo.rasterizer.depthClampEnable = VK_FALSE;
	configInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
	configInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	configInfo.rasterizer.lineWidth = 1.0f;
	configInfo.rasterizer.cullMode = VK_CULL_MODE_NONE;
	configInfo.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	configInfo.rasterizer.depthBiasClamp = VK_FALSE;
	configInfo.rasterizer.depthBiasConstantFactor = 0.0f;
	configInfo.rasterizer.depthBiasClamp = 0.0f;
	configInfo.rasterizer.depthBiasSlopeFactor = 0.0f;

	// multisampling
	configInfo.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	configInfo.multisampling.sampleShadingEnable = VK_FALSE;
	configInfo.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	configInfo.multisampling.minSampleShading = 1.0f;
	configInfo.multisampling.pSampleMask = nullptr;
	configInfo.multisampling.alphaToCoverageEnable = VK_FALSE;
	configInfo.multisampling.alphaToOneEnable = VK_FALSE;

	// color blending
	configInfo.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
	configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	configInfo.colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	configInfo.colorBlendingInfo.logicOpEnable = VK_FALSE;
	configInfo.colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
	configInfo.colorBlendingInfo.attachmentCount = 1;
	configInfo.colorBlendingInfo.pAttachments = &configInfo.colorBlendAttachment;
	configInfo.colorBlendingInfo.blendConstants[0] = 0.0f;
	configInfo.colorBlendingInfo.blendConstants[1] = 0.0f;
	configInfo.colorBlendingInfo.blendConstants[2] = 0.0f;
	configInfo.colorBlendingInfo.blendConstants[3] = 0.0f;

	// Depth Testing
	configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
	configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
	configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.minDepthBounds = 0.0f;
	configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
	configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.front = {};
	configInfo.depthStencilInfo.back = {};

	return configInfo;
}

void VelVulkanPipeline::create(
	const VkPipelineLayout& pipelineLayout,
	const VkRenderPass& renderPass) {
	auto shaders = createShaders();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaders.shaderStages;
	pipelineInfo.pVertexInputState = &configInfo.vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &configInfo.inputAssembly;
	pipelineInfo.pRasterizationState = &configInfo.rasterizer;
	pipelineInfo.pMultisampleState = &configInfo.multisampling;
	pipelineInfo.pViewportState = &configInfo.viewportState;
	pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
	pipelineInfo.pColorBlendState = &configInfo.colorBlendingInfo;

	pipelineInfo.layout = pipelineLayout;

	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(
		device,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	for (const auto& module : shaders.shadersModule) {
		vkDestroyShaderModule(device, module, nullptr);
	}
}

void VelVulkanPipeline::destroy() {
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
}

VelVulkanPipeline::ShadersCreation VelVulkanPipeline::createShaders() {
	auto vertShaderCode = readFile(vertFilePath);
	auto fragShaderCode = readFile(fragFilePath);

	// wrap the byte code in a Shader Module
	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	// assign the shaders to the respective pipeline stages
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	return {
		{ vertShaderModule, fragShaderModule },
		{ vertShaderStageInfo, fragShaderStageInfo }
	};
}

VkShaderModule VelVulkanPipeline::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

}
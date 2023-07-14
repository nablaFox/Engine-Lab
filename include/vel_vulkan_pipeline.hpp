#include <vulkan/vulkan_core.h>

#include <vector>
#include <string>

namespace vel {

class VelVulkanPipeline {
public:
	struct PipelineConfigInfo {
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportState;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineMultisampleStateCreateInfo multisampling;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendingInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	};

	struct ShadersCreation {
		VkShaderModule shadersModule[2];
		VkPipelineShaderStageCreateInfo shaderStages[2];
	};

	VelVulkanPipeline(
		VkDevice& device,
		PipelineConfigInfo configInfo,
		const std::string vertFilePath,
		const std::string fragFilepath);

	~VelVulkanPipeline();

	VelVulkanPipeline(const VelVulkanPipeline&) = delete;
	void operator=(const VelVulkanPipeline&) = delete;

	static PipelineConfigInfo defaultPipelineConfigInfo(VkExtent2D extent);

	void create(const VkPipelineLayout& pipelineLayout,
		const VkRenderPass& renderPass);

	VkPipeline get() { return graphicsPipeline; }
	void destroy();

private:
	ShadersCreation createShaders();
	VkShaderModule createShaderModule(const std::vector<char>& code);

	VkPipeline graphicsPipeline;
	VkDevice& device;

	PipelineConfigInfo configInfo;
	const std::string vertFilePath;
	const std::string fragFilePath;
};

}
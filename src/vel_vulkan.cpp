#include "../include/vel_vulkan.hpp"

#include <iostream>
#include <unordered_set>
#include <stdexcept>

namespace vel {

VelVulkan::~VelVulkan() {
	vkDestroyInstance(instance, nullptr);
}

void VelVulkan::initVulkan() {
	createInstance();
}

void VelVulkan::createInstance() {
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Engine Lab";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	if (!checkExtensionCompatibility(extensions)) {
		return throw std::runtime_error("Missing required glfw extension");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

std::vector<const char*> VelVulkan::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	return extensions;
}

bool VelVulkan::checkExtensionCompatibility(const std::vector<const char*>& requiredExtensions) {
	auto available = getAvailableExtensions();
	bool compatible = true;

	std::cout << "required extensions:" << std::endl;
	for (const auto& required : requiredExtensions) {
		std::cout << "\t" << required << std::endl;
		if (available.find(required) == available.end()) {
			compatible = false;
		}
	}

	return compatible;
}

std::unordered_set<std::string> VelVulkan::getAvailableExtensions() {
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "available extensions:" << std::endl;
	std::unordered_set<std::string> available;
	for (const auto& extension : extensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
		available.insert(extension.extensionName);
	}

	return available;
}

}
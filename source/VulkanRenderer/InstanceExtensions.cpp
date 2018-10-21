#include "InstanceExtensions.hpp"
#include "VulkanFunctions.hpp"

using namespace De::Vulkan;

const std::vector<VkExtensionProperties>& De::getAvailableExtensions()
{
	static uint32_t extensionsCount{0};
	if(extensionsCount <= 0)
	{
		vkEnumerateInstanceExtensionProperties(
			nullptr, &extensionsCount, nullptr
		);
	}
	static std::vector<VkExtensionProperties> availableExtensions;
	if(availableExtensions.empty())
	{
		availableExtensions.resize(extensionsCount);
		vkEnumerateInstanceExtensionProperties(
			nullptr, &extensionsCount, availableExtensions.data()
		);
	}
	return availableExtensions;
}

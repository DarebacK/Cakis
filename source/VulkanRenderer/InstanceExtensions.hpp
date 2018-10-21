#pragma once
#include <vector>
#include "Vulkan"
namespace De
{
	const std::vector<VkExtensionProperties>& getAvailableExtensions();
}
#include "Device.hpp"
#include "PhysicalDevice.hpp"
#include <array>

namespace De::Vk
{
  static const std::array<const char*, 1> extensions = { "VK_KHR_swapchain"};

Device::Device(VkPhysicalDevice physicalDevice, VkDeviceCreateFlags flags, 
      uint32_t queueCreateInfoCount, const VkDeviceQueueCreateInfo* queueCreateInfos, 
      uint32_t layerCount, const char* const* layerNames,  
      const VkPhysicalDeviceFeatures* features, 
      const VkAllocationCallbacks* allocationCallbacks)
  : allocationCallbacks{allocationCallbacks}
{
  VkDeviceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = flags;
  info.queueCreateInfoCount = queueCreateInfoCount;
  info.pQueueCreateInfos = queueCreateInfos;
  info.enabledLayerCount = layerCount;
  info.ppEnabledLayerNames = layerNames;
  info.ppEnabledExtensionNames = extensions.data();
  info.enabledExtensionCount = extensions.size();
  info.pEnabledFeatures = features;
  vkCheckResult(vkCreateDevice(physicalDevice, &info, allocationCallbacks, &handle));
}

Device::~Device()
{
  vkDestroyDevice(handle, allocationCallbacks);
}
}
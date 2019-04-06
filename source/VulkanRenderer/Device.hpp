#pragma once
#include "Vulkan"

namespace De::Vk
{
  // aka "logical device"
	class Device
	{
	public:
    explicit Device(VkPhysicalDevice physicalDevice, VkDeviceCreateFlags flags, 
      uint32_t queueCreateInfoCount, const VkDeviceQueueCreateInfo* queueCreateInfos, 
      uint32_t layerCount, const char* const* layerNames, 
      const VkPhysicalDeviceFeatures* features, 
      const VkAllocationCallbacks* allocationCallbacks = nullptr);
             Device(const Device& other) = delete;
             Device(Device&& other) = delete;
             ~Device();
    Device& operator=(const Device& rhs) = delete;
    Device& operator=(Device&& rhs) = delete;
    operator VkDevice() {return handle;}



  private:
    VkDevice handle;
    const VkAllocationCallbacks* allocationCallbacks = nullptr;
	};
}

#include "PhysicalDevice.hpp"

namespace De::Vk
{
  extern PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
  extern PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
  extern PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
  extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
  static void swap(PhysicalDevice& first, PhysicalDevice& second) noexcept
  {
      using std::swap;
      swap(first.handle, second.handle);
  }

PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle)
    : handle{handle}
{
    initAvailableExtensions();
    initFeatures();
    initProperties();
    initQueueFamiliesProperties();
}
void PhysicalDevice::initAvailableExtensions()
{
    uint32_t extensionsCount{};
    vkCheckResult(vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionsCount, nullptr));
    availableExtensions.resize(extensionsCount);
    vkCheckResult(vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionsCount, availableExtensions.data()));
}
void PhysicalDevice::initFeatures()
{
   vkGetPhysicalDeviceFeatures(handle, &features);
}
void PhysicalDevice::initProperties()
{
   vkGetPhysicalDeviceProperties(handle, &properties);
}
void PhysicalDevice::initQueueFamiliesProperties()
{
    uint32_t queueFamiliesCount{};
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, nullptr);
    queueFamiliesProperties.resize(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, queueFamiliesProperties.data());
}
PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept
{
    swap(*this, other);    
}
PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& rhs) noexcept
{
    swap(*this, rhs);
    return *this;
}

uint32_t selectUniversalQueueFamily(const std::vector<VkQueueFamilyProperties>& familiesProperties)
{
  for(uint32_t i = 0; i < familiesProperties.size(); ++i)
  {
    const auto& familyProperties = familiesProperties[i];
    if(familyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT 
      && familyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT 
      && familyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT)
    {
      return i;
    }
  }
  throw Error("Failed to find suitable queue family");
}
}
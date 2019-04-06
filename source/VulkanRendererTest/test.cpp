#include "pch.h"
#include "Loader.hpp"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "Device.hpp"

using namespace De::Vk;

TEST(VulkanRenderer, initialization) {
  Loader loader;
  Instance instance{loader.loadInstance()};
  auto& physicalDevices = instance.getPhysicalDevices();
  VkDeviceQueueCreateInfo queueInfo{};
  queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfo.queueFamilyIndex = selectUniversalQueueFamily(physicalDevices[0].getQueueFamiliesProperties());
  queueInfo.queueCount = 1;
  VkPhysicalDeviceFeatures features{};
  const char* layer = "VK_LAYER_LUNARG_standard_validation";
  Device device(physicalDevices[0], {}, 1, &queueInfo, 1, &layer, &features);
  loadDeviceCoreFunctions(device);
  loadDeviceSwapchainFunctions(device);
}
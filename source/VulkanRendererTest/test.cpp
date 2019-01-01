#include "pch.h"
#include "Loader.hpp"
#include "Instance.hpp"

using namespace De::Vulkan;

TEST(VulkanRenderer, initialization) {
  Loader loader;
  Instance instance{loader.loadInstance()};
}
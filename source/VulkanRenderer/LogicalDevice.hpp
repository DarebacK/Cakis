#pragma once
#include "Instance.hpp"

namespace De::Vulkan
{
	class LogicalDevice
	{
	public:
        explicit LogicalDevice(const PhysicalDevice& physicalDevice);
                 LogicalDevice(const LogicalDevice& other) = delete;
                 LogicalDevice(LogicalDevice&& other) = delete;
                ~LogicalDevice();
        LogicalDevice& operator=(const LogicalDevice& rhs) = delete;
        LogicalDevice& operator=(LogicalDevice&& rhs) = delete;
	};
}

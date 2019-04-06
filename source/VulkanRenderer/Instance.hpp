#pragma once
#include "Vulkan"
#include <memory>
#include <vector>

namespace De::Vk
{
  class PhysicalDevice;

	class Instance
	{
	public:
		explicit  Instance(const VkInstanceCreateInfo& info, const VkAllocationCallbacks* allocator = nullptr);
					    Instance(const Instance& other) = delete;
					    Instance(Instance&& other) noexcept;
				      ~Instance();
		Instance&	operator=(const Instance& rhs) = delete;
		Instance&	operator=(Instance&& rhs) noexcept;
    const std::vector<PhysicalDevice>& getPhysicalDevices() const;

	private:
		class Impl;
		std::unique_ptr<Impl> pImpl{nullptr};
	};
}

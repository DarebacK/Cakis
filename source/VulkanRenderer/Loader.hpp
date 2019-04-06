#pragma once
#include "Vulkan"
#include <memory>
#include "ApplicationInfo.hpp"
#include <vector>

namespace De::Vk
{
	class Instance;

	class Loader
	{
	public:
		Loader();
		Loader(const Loader& other) = delete;
		Loader(Loader&& other) = default;
		~Loader();
    Loader&	operator=(const Loader& rhs) = delete;
		Loader& operator=(Loader&& rhs) = default;
		Instance	loadInstance(const std::vector<const char*>& layersToEnable = {}, 
						 const std::vector<const char*>& extensionsToLoad = {},
						 const ApplicationInfo& applicationInfo = {}, 
						 const VkAllocationCallbacks* allocator = nullptr);
		const std::vector<VkExtensionProperties>& getAvailableExtensions() const;
	private:
		class Impl;
		std::unique_ptr<Impl> pImpl{nullptr};
	};

  void loadDeviceCoreFunctions(VkDevice device = nullptr);
  void loadDeviceSwapchainFunctions(VkDevice device = nullptr);
}

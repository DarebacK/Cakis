#pragma once
#include "Vulkan"
#include <memory>
#include <vector>

namespace De::Vulkan
{
	class Instance
	{
	public:
		explicit	Instance(const VkInstanceCreateInfo& info,
							 const VkAllocationCallbacks* allocator = nullptr);
					Instance(const Instance& other) = delete;
					Instance(Instance&& other) noexcept;
					~Instance();
		Instance&	operator=(const Instance& rhs) = delete;
		Instance&	operator=(Instance&& rhs) noexcept;

	private:
        friend class PhysicalDevice;
		class Impl;
		std::unique_ptr<Impl> pImpl{nullptr};
	};

    class PhysicalDevice
	{
	public:
        explicit PhysicalDevice(Instance& instance, VkPhysicalDevice handle);
                 PhysicalDevice(const PhysicalDevice& other) = delete;
                 PhysicalDevice(PhysicalDevice&& other) noexcept;
                ~PhysicalDevice() = default;
        PhysicalDevice& operator=(PhysicalDevice& rhs) = delete;
        PhysicalDevice& operator=(PhysicalDevice&& rhs) noexcept;
        const std::vector<VkExtensionProperties>& getAvailableExtensions() const {return availableExtensions;}
        const VkPhysicalDeviceFeatures& getFeatures() const {return features;}
        const VkPhysicalDeviceProperties& getProperties() const {return properties;}

	private:
        VkPhysicalDevice handle{};
        std::vector<VkExtensionProperties> availableExtensions;
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceProperties properties{};

        friend void swap(PhysicalDevice& first, PhysicalDevice& second) noexcept;
        void initAvailableExtensions(Instance& instance);
        void initFeatures(Instance& instance);
        void initProperties(Instance& instance);
	};
}

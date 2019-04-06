#pragma once
#include "Vulkan"
#include <vector>

namespace De::Vk
{
	class PhysicalDevice
	{
	public:
    explicit PhysicalDevice(VkPhysicalDevice handle);
             PhysicalDevice(const PhysicalDevice& other) = delete;
             PhysicalDevice(PhysicalDevice&& other) noexcept;
            ~PhysicalDevice() = default;
    PhysicalDevice& operator=(const PhysicalDevice& rhs) = delete;
    PhysicalDevice& operator=(PhysicalDevice&& rhs) noexcept;
    operator VkPhysicalDevice() const {return handle;}
    const std::vector<VkExtensionProperties>&   getAvailableExtensions() const {return availableExtensions;}
    const VkPhysicalDeviceFeatures&             getFeatures() const {return features;}
    const VkPhysicalDeviceProperties&           getProperties() const {return properties;}
    const std::vector<VkQueueFamilyProperties>& getQueueFamiliesProperties() const {return queueFamiliesProperties;}
	
	private:
    VkPhysicalDevice handle{};
    std::vector<VkExtensionProperties> availableExtensions{};
    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceProperties properties{};
    std::vector<VkQueueFamilyProperties> queueFamiliesProperties{};

    friend void swap(PhysicalDevice& first, PhysicalDevice& second) noexcept;
    void initAvailableExtensions();
    void initFeatures();
    void initProperties();
    void initQueueFamiliesProperties();
	};

  uint32_t selectUniversalQueueFamily(const std::vector<VkQueueFamilyProperties>& familiesProperties);
}
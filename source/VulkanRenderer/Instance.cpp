#include "Instance.hpp"
#include "VulkanFunctions.hpp"
#include "Error.hpp"
#include <vector>

namespace De::Vulkan
{
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name{nullptr};
#include "ListOfVulkanFunctions.inl"
}

using namespace De::Vulkan;

// Instance -------------------------------------------------------------------
class Instance::Impl
{
public:
	explicit Impl(const VkInstanceCreateInfo& info,
				  const VkAllocationCallbacks* allocator = nullptr)
	{
		initVkInstance(info, allocator);
		loadInstanceLevelFunctions();
    initPhysicalDevices();
	}
	Impl(const Impl& other) = delete;
	Impl(Impl&& other) noexcept;
	~Impl()
	{
		if(!instance)
		{
			return;
		}
		vkDestroyInstance(instance, allocator);
	}
	Impl& operator=(const Impl& rhs) = delete;
	Impl& operator=(Impl&& rhs) noexcept;
	friend void swap(Impl& first, Impl& second) noexcept
	{
		using std::swap;
		swap(first.instance, second.instance);
		swap(first.allocator, second.allocator);
	}

	VkInstance instance{nullptr};
	VkAllocationCallbacks* allocator{nullptr};
  std::vector<PhysicalDevice> physicalDevices;
	void initVkInstance(const VkInstanceCreateInfo& info,
				  const VkAllocationCallbacks* allocator)
	{
		VkResult result = vkCreateInstance(&info, allocator, &instance);
        checkResult(result);
	}
	void loadInstanceLevelFunctions()
	{
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name )	\
	name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );	\
	if(!name)	\
	{	\
		throw Error{std::string("could not load vulkan function ") + std::string(#name) };	\
	}
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name )	\
	name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );
#include "ListOfVulkanFunctions.inl"
	}
    void initPhysicalDevices()
	{
	    uint32_t devicesCount{};
        VkResult result = vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
        checkResult(result);
        std::vector<VkPhysicalDevice> vkPhysicalDevices;
	    vkPhysicalDevices.resize(devicesCount);
        result = vkEnumeratePhysicalDevices(instance, &devicesCount, vkPhysicalDevices.data());
	    checkResult(result);
        physicalDevices.reserve(devicesCount);
        for(auto vkPhysicalDevice : vkPhysicalDevices)
        {
            physicalDevices.emplace_back(vkPhysicalDevice);
        }
	}
};

Instance::Instance(const VkInstanceCreateInfo& info,
				   const VkAllocationCallbacks* allocator)
	:pImpl(std::make_unique<Impl>(info, allocator))
{}
Instance::Instance(Instance&& other) noexcept
{
	swap(pImpl, other.pImpl);
}
Instance& Instance::operator=(Instance&& rhs) noexcept
{
	swap(pImpl, rhs.pImpl);
	return *this;
}
Instance::~Instance() = default;

// PhysicalDevice -------------------------------------------------------------
namespace De::Vulkan
{
    static void swap(PhysicalDevice& first, PhysicalDevice& second) noexcept
    {
        using std::swap;
        swap(first.handle, second.handle);
    }
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
    VkResult result = vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionsCount, nullptr);
    checkResult(result);
    availableExtensions.resize(extensionsCount);
    result = vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionsCount, availableExtensions.data());
    checkResult(result);
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

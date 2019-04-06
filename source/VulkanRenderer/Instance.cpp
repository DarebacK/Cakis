#include "Instance.hpp"
#include "Error.hpp"
#include <vector>
#include "PhysicalDevice.hpp"

namespace De::Vk
{
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name{nullptr};
#include "ListOfVulkanFunctions.inl"

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
		vkCheckResult(vkCreateInstance(&info, allocator, &instance));
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
        vkCheckResult(vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr));
        std::vector<VkPhysicalDevice> vkPhysicalDevices;
	    vkPhysicalDevices.resize(devicesCount);
        vkCheckResult(vkEnumeratePhysicalDevices(instance, &devicesCount, vkPhysicalDevices.data()));
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
const std::vector<PhysicalDevice>& Instance::getPhysicalDevices() const
{
  return pImpl->physicalDevices;
}
}
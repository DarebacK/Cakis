#include "VulkanRenderer.h"
#include "ApplicationInfo.hpp"
#include "Library.hpp"
#include "Exception.hpp"

#define VK_NO_PROTOTYPES
#include "vulkan.h"

#include <windows.h>

#include <memory>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <array>

class Error : public De::Exception
{
public:
	explicit	Error(std::string message)
    :De::Exception{"Vulkan error: " + message}
  {}
	explicit	Error(const char* message)
      :Error{std::string{message}}
  {}
				Error(const Error& other) = default;
				Error(Error&& other) = default;
				Error&	operator=(const Error& rhs) = default;
				Error&	operator=(Error&& rhs) = default;
	virtual		~Error() = default;
};

#define vkCheckResult(func) \
switch(func) \
{ \
  case VK_SUCCESS: break; \
  case VK_ERROR_OUT_OF_HOST_MEMORY: throw Error{"Vulkan error: VK_ERROR_OUT_OF_HOST_MEMORY"}; \
  case VK_ERROR_OUT_OF_DEVICE_MEMORY: throw Error{"Vulkan error: VK_ERROR_OUT_OF_DEVICE_MEMORY"}; \
  case VK_ERROR_INITIALIZATION_FAILED: throw Error{"Vulkan error: VK_ERROR_INITIALIZATION_FAILED"}; \
  case VK_ERROR_LAYER_NOT_PRESENT: throw Error{"Vulkan error: VK_ERROR_LAYER_NOT_PRESENT"}; \
  case VK_ERROR_EXTENSION_NOT_PRESENT: throw Error{"Vulkan error: VK_ERROR_EXTENSION_NOT_PRESENT"}; \
  case VK_ERROR_INCOMPATIBLE_DRIVER: throw Error{"Vulkan error: VK_ERROR_INCOMPATIBLE_DRIVER"}; \
  case VK_ERROR_FEATURE_NOT_PRESENT: throw Error{"Vulkan error: VK_ERROR_FEATURE_NOT_PRESENT"}; \
  case VK_ERROR_TOO_MANY_OBJECTS: throw Error{"Vulkan error: VK_ERROR_TOO_MANY_OBJECTS"}; \
  case VK_ERROR_DEVICE_LOST: throw Error{"Vulkan error: VK_ERROR_DEVICE_LOST"}; \
  default: throw Error{"Vulkan error: unknown reason"}; \
}

namespace {
#define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name{nullptr};
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define DE_VK_KHR_SWAPCHAIN_FUNCTION(name) PFN_##name name{nullptr};
#include "ListOfVulkanFunctions.inl"

const char* const vulkanLibraryPath = 
#if defined(_WIN32)
	"vulkan-1.dll";
#elif defined(__linux__)
	"libvulkan.so.1";
#endif
}

static VkApplicationInfo makeVkApplicationInfo(const De::ApplicationInfo applicationInfo)
{
	VkApplicationInfo vkAppInfo;
	vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkAppInfo.pNext = nullptr;
	vkAppInfo.pApplicationName = applicationInfo.name.data();
	vkAppInfo.applicationVersion = applicationInfo.version.major << 22 | 
		applicationInfo.version.minor << 12 | 
		applicationInfo.version.patch;	// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VK_MAKE_VERSION.html
	vkAppInfo.pEngineName = "DarEngine";
	vkAppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	vkAppInfo.apiVersion = VK_API_VERSION_1_1;
	return vkAppInfo;
}
static VkInstanceCreateInfo	makeVkInstanceCreateInfo(const VkApplicationInfo& vkApplicationInfo, 
													const std::vector<const char*> layersToEnable, 
													std::vector<const char*> extensionsToLoad)
{
	VkInstanceCreateInfo vkInstanceCreateInfo;
	vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vkInstanceCreateInfo.pNext = nullptr;
	vkInstanceCreateInfo.flags = 0;
	vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
	vkInstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layersToEnable.size());
	vkInstanceCreateInfo.ppEnabledLayerNames = layersToEnable.data();
	vkInstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsToLoad.size());
	vkInstanceCreateInfo.ppEnabledExtensionNames = extensionsToLoad.data();
	return vkInstanceCreateInfo;
}

class PhysicalDevice
{
public:
  explicit PhysicalDevice(VkPhysicalDevice handle)
    : handle{handle}
  {
    initAvailableExtensions();
    initFeatures();
    initProperties();
    initQueueFamiliesProperties();
  }
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
  void initAvailableExtensions()
  {
    uint32_t extensionsCount{};
    vkCheckResult(vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionsCount, nullptr));
    availableExtensions.resize(extensionsCount);
    vkCheckResult(vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionsCount, availableExtensions.data()));
  }
  void initFeatures()
  {
    vkGetPhysicalDeviceFeatures(handle, &features);
  }
  void initProperties()
  {
   vkGetPhysicalDeviceProperties(handle, &properties);
  }
  void initQueueFamiliesProperties()
  {
    uint32_t queueFamiliesCount{};
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, nullptr);
    queueFamiliesProperties.resize(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, queueFamiliesProperties.data());
  }
};
static uint32_t selectUniversalQueueFamily(const std::vector<VkQueueFamilyProperties>& familiesProperties)
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

class Instance
{
public:
	explicit  Instance(const VkInstanceCreateInfo& info, const VkAllocationCallbacks* allocator = nullptr)
  {
    initVkInstance(info, allocator);
		loadInstanceLevelFunctions();
    initPhysicalDevices();
  }
	~Instance()
  {
    if(!instance)
		{
			return;
		}
		vkDestroyInstance(instance, allocator);
  }
	Instance&	operator=(const Instance& rhs) = delete;
	Instance&	operator=(Instance&& rhs) noexcept;
  const std::vector<PhysicalDevice>& getPhysicalDevices() const { return physicalDevices; }

private:
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

class Loader
{
public:
	Loader()
  {
  	// redefine vulkan function macros to load the vulkan functions we need
#define EXPORTED_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(vulkanLibrary.loadFunction(#name));
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name));
#include "ListOfVulkanFunctions.inl"
		initAvailableExtensions();
  }
	VkInstanceCreateInfo	loadInstanceInfo(const std::vector<const char*>& layersToEnable = {}, 
						const std::vector<const char*>& extensionsToLoad = {},
						const De::ApplicationInfo& applicationInfo = {}, 
						const VkAllocationCallbacks* allocator = nullptr)
  {
    checkExtensionsAvailability(extensionsToLoad);
		VkApplicationInfo vkApplicationInfo = makeVkApplicationInfo(applicationInfo);
		VkInstanceCreateInfo vkInstanceCreateInfo = makeVkInstanceCreateInfo(vkApplicationInfo, 
			layersToEnable, extensionsToLoad);
    return vkInstanceCreateInfo;
  }
	const std::vector<VkExtensionProperties>& getAvailableExtensions() const;

private:
  De::Library vulkanLibrary{vulkanLibraryPath};
	std::vector<VkExtensionProperties> availableExtensions;

  void checkExtensionsAvailability(const std::vector<const char*>& extensions)
	{
		std::vector<std::string> unavailableExtensions;
		for(auto extension : extensions)
		{
			bool wasFound{false};
			for(auto availableExtension : availableExtensions)
			{
				if(std::string(extension) == std::string(availableExtension.extensionName))
				{
					wasFound = true;
					break;
				}
			}
			if(!wasFound)
			{
				
				unavailableExtensions.emplace_back(extension);
			}
		}
		if(!unavailableExtensions.empty())
		{
			std::stringstream errorMessageStream;
			errorMessageStream << "extensions not available: ";
			for(auto unavailableExtension : unavailableExtensions)
			{
				errorMessageStream << unavailableExtension << ", ";
			}
			std::string errorMessage = errorMessageStream.str();
			errorMessage = errorMessage.substr(0, errorMessage.size() - 2);
			throw Error{errorMessage};
		}
	}
  void initAvailableExtensions()
	{
		uint32_t extensionsCount{0};
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, 
			nullptr);
		availableExtensions.resize(extensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, 
			availableExtensions.data());
	}
};

void loadDeviceCoreFunctions(VkDevice device = nullptr)
{
  if(device)
  {
  #define DEVICE_LEVEL_VULKAN_FUNCTION(name) \
  name = (PFN_##name) vkGetDeviceProcAddr(device, #name); \
  if(!name) throw Error{std::string("could not load vulkan function ") + std::string(#name)};
  #include "ListOfVulkanFunctions.inl"
  }
}
void loadDeviceSwapchainFunctions(VkDevice device = nullptr)
{
  #define DE_VK_KHR_SWAPCHAIN_FUNCTION(name) \
  name = (PFN_##name) vkGetDeviceProcAddr(device, #name); \
  if(!name) throw Error{std::string("could not load vulkan function ") + std::string(#name)};
  #include "ListOfVulkanFunctions.inl"
}

static const std::array<const char*, 1> deviceExtensions = { "VK_KHR_swapchain" };
// aka "logical device"
class Device
{
public:
  explicit Device(VkPhysicalDevice physicalDevice, VkDeviceCreateFlags flags, 
    uint32_t queueCreateInfoCount, const VkDeviceQueueCreateInfo* queueCreateInfos, 
    uint32_t layerCount, const char* const* layerNames, 
    const VkPhysicalDeviceFeatures* features, 
    const VkAllocationCallbacks* allocationCallbacks = nullptr)
  : allocationCallbacks{allocationCallbacks}
  {
    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    info.queueCreateInfoCount = queueCreateInfoCount;
    info.pQueueCreateInfos = queueCreateInfos;
    info.enabledLayerCount = layerCount;
    info.ppEnabledLayerNames = layerNames;
    info.ppEnabledExtensionNames = deviceExtensions.data();
    info.enabledExtensionCount = (uint32_t)deviceExtensions.size();
    info.pEnabledFeatures = features;
    vkCheckResult(vkCreateDevice(physicalDevice, &info, allocationCallbacks, &handle));
  }
  ~Device()
  {
    vkDestroyDevice(handle, allocationCallbacks);
  }
  Device& operator=(const Device& rhs) = delete;
  Device& operator=(Device&& rhs) = delete;
  operator VkDevice() {return handle;}

private:
  VkDevice handle;
  const VkAllocationCallbacks* allocationCallbacks = nullptr;
};

void initVulkanRenderer(HWND windowHandle)
{
  Loader loader;
  Instance instance{loader.loadInstanceInfo()};
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
#include "VulkanRenderer.h"
#include "ApplicationInfo.hpp"
#include "Exception.hpp"

#include <memory>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <array>

#ifdef _WIN32
  #include <windows.h>
  #define VK_USE_PLATFORM_WIN32_KHR
  #define VK_NO_PROTOTYPES
  #include "vulkan.h"
  static const std::array<const char*, 2> instanceExtensions = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
#endif

class VulkanError : public De::Exception
{
public:
	explicit	VulkanError(std::string message)
    :De::Exception{"Vulkan error: " + message}
  {}
	explicit	VulkanError(const char* message)
      :VulkanError{std::string{message}}
  {}
				VulkanError(const VulkanError& other) = default;
				VulkanError(VulkanError&& other) = default;
				VulkanError&	operator=(const VulkanError& rhs) = default;
				VulkanError&	operator=(VulkanError&& rhs) = default;
	virtual		~VulkanError() = default;
};

#define checkResult(func) \
switch(func) \
{ \
  case VK_SUCCESS: break; \
  case VK_ERROR_OUT_OF_HOST_MEMORY: throw VulkanError{"Vulkan error: VK_ERROR_OUT_OF_HOST_MEMORY"}; \
  case VK_ERROR_OUT_OF_DEVICE_MEMORY: throw VulkanError{"Vulkan error: VK_ERROR_OUT_OF_DEVICE_MEMORY"}; \
  case VK_ERROR_INITIALIZATION_FAILED: throw VulkanError{"Vulkan error: VK_ERROR_INITIALIZATION_FAILED"}; \
  case VK_ERROR_LAYER_NOT_PRESENT: throw VulkanError{"Vulkan error: VK_ERROR_LAYER_NOT_PRESENT"}; \
  case VK_ERROR_EXTENSION_NOT_PRESENT: throw VulkanError{"Vulkan error: VK_ERROR_EXTENSION_NOT_PRESENT"}; \
  case VK_ERROR_INCOMPATIBLE_DRIVER: throw VulkanError{"Vulkan error: VK_ERROR_INCOMPATIBLE_DRIVER"}; \
  case VK_ERROR_FEATURE_NOT_PRESENT: throw VulkanError{"Vulkan error: VK_ERROR_FEATURE_NOT_PRESENT"}; \
  case VK_ERROR_TOO_MANY_OBJECTS: throw VulkanError{"Vulkan error: VK_ERROR_TOO_MANY_OBJECTS"}; \
  case VK_ERROR_DEVICE_LOST: throw VulkanError{"Vulkan error: VK_ERROR_DEVICE_LOST"}; \
  default: throw VulkanError{"Vulkan error: unknown reason"}; \
}

namespace {
  #define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
  #define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
  #define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
  #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name) PFN_##name name{nullptr};
  #define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
  #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name) PFN_##name name{nullptr};
  #include "ListOfVulkanFunctions.inl"

  VkInstance instance = nullptr;
  std::vector<VkExtensionProperties> availableInstanceExtensions;
  const std::array<const char*, 1> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
  const std::vector<const char*> layersToEnable = {}; 
  const De::ApplicationInfo applicationInfo = {}; 
  const VkAllocationCallbacks* allocator = nullptr;
  VkSurfaceKHR presentationSurface = nullptr;
  VkDevice device = nullptr;

  static uint32_t selectUniversalQueueFamily(VkPhysicalDevice physicalDevice)
  {
    uint32_t queueFamiliesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamiliesProperties;
    queueFamiliesProperties.resize(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamiliesProperties.data());

    for(uint32_t i = 0; i < queueFamiliesProperties.size(); ++i)
    {
      const VkQueueFamilyProperties& familyProperties = queueFamiliesProperties[i];
      if(familyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT 
        && familyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT 
        && familyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT)
      {
        VkBool32 presentationSupported = false;
        checkResult(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, presentationSurface, &presentationSupported));
        if(presentationSupported) return i;
      }
    }
    throw VulkanError("Failed to find suitable queue family");
  }

  static void initInstance(const VkInstanceCreateInfo& info)
  {
    checkResult(vkCreateInstance(&info, allocator, &instance));

    #define INSTANCE_LEVEL_VULKAN_FUNCTION(name)	\
	  name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );	\
	  if(!name)	\
	  {	\
		  throw VulkanError{std::string("could not load vulkan function ") + std::string(#name) };	\
	  }
    #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name)	\
	  name = (PFN_##name)vkGetInstanceProcAddr( instance, #name ); \
    if(!name)	\
	  {	\
		  throw VulkanError{std::string("could not load vulkan function ") + std::string(#name) };	\
	  }
    #include "ListOfVulkanFunctions.inl"
  }

  static VkPhysicalDevice findSuitablePhysicalDevice()
  {
    uint32_t devicesCount{};
    checkResult(vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr));
    std::vector<VkPhysicalDevice> vkPhysicalDevices;
	  vkPhysicalDevices.resize(devicesCount);
    checkResult(vkEnumeratePhysicalDevices(instance, &devicesCount, vkPhysicalDevices.data()));
    //for(VkPhysicalDevice physicalDevice : vkPhysicalDevices)
    //{
    //  uint32_t extensionsCount;
    //  checkResult(vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionsCount, nullptr));
    //  std::vector<VkExtensionProperties> availableExtensions;
    //  availableExtensions.resize(extensionsCount);
    //  checkResult(vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionsCount, availableExtensions.data()));

    //  VkPhysicalDeviceFeatures features;
    //  vkGetPhysicalDeviceFeatures(handle, &features);

    //  VkPhysicalDeviceProperties properties;
    //  vkGetPhysicalDeviceProperties(handle, &properties);

    //  uint32_t queueFamiliesCount;
    //  vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, nullptr);
    //  std::vector<VkQueueFamilyProperties> queueFamiliesProperties;
    //  queueFamiliesProperties.resize(queueFamiliesCount);
    //  vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamiliesCount, queueFamiliesProperties.data());
    //}
    return vkPhysicalDevices.at(0); // just 
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
													                             const std::vector<const char*> layersToEnable)
  {
	  VkInstanceCreateInfo vkInstanceCreateInfo;
	  vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	  vkInstanceCreateInfo.pNext = nullptr;
	  vkInstanceCreateInfo.flags = 0;
	  vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
	  vkInstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layersToEnable.size());
	  vkInstanceCreateInfo.ppEnabledLayerNames = layersToEnable.data();
	  vkInstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	  vkInstanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
	  return vkInstanceCreateInfo;
  }

  void loadDeviceCoreFunctions(VkDevice device = nullptr)
  {
    if(device)
    {
    #define DEVICE_LEVEL_VULKAN_FUNCTION(name) \
    name = (PFN_##name) vkGetDeviceProcAddr(device, #name); \
    if(!name) throw VulkanError{std::string("could not load vulkan function ") + std::string(#name)};
    #include "ListOfVulkanFunctions.inl"
    }
  }
  void loadDeviceSwapchainFunctions(VkDevice device = nullptr)
  {
    #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name) \
    name = (PFN_##name) vkGetDeviceProcAddr(device, #name); \
    if(!name) throw VulkanError{std::string("could not load vulkan function ") + std::string(#name)};
    #include "ListOfVulkanFunctions.inl"
  }
  void initDevice(VkPhysicalDevice physicalDevice)
  {
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = selectUniversalQueueFamily(physicalDevice);
    queueInfo.queueCount = 1;

    VkPhysicalDeviceFeatures features{};
    const char* layer = "VK_LAYER_LUNARG_standard_validation";
    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &queueInfo;
    info.enabledLayerCount = 1;
    info.ppEnabledLayerNames = &layer;
    info.ppEnabledExtensionNames = deviceExtensions.data();
    info.enabledExtensionCount = (uint32_t)deviceExtensions.size();
    info.pEnabledFeatures = &features;
    checkResult(vkCreateDevice(physicalDevice, &info, allocator, &device));
    loadDeviceCoreFunctions(device);
    loadDeviceSwapchainFunctions(device);
  }

  void checkInstanceExtensionsAvailability()
  {
	  std::vector<std::string> unavailableExtensions;
	  for(auto extension : instanceExtensions)
	  {
		  bool wasFound{false};
		  for(auto availableExtension : availableInstanceExtensions)
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
		  throw VulkanError{errorMessage};
	  }
  }
  void enumarateAvailableInstanceExtensions()
  {
    uint32_t instanceExtensionsCount{0};
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, nullptr);
		availableInstanceExtensions.resize(instanceExtensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, availableInstanceExtensions.data());
  }
  VkInstanceCreateInfo loadInstanceInfo()
  {
    enumarateAvailableInstanceExtensions();
    checkInstanceExtensionsAvailability();
	  VkApplicationInfo vkApplicationInfo = makeVkApplicationInfo(applicationInfo);
	  VkInstanceCreateInfo vkInstanceCreateInfo = makeVkInstanceCreateInfo(vkApplicationInfo, 
		  layersToEnable);
    return vkInstanceCreateInfo;
  }
}


#ifdef _WIN32
  bool initVulkanRenderer(HINSTANCE winInstanceHandle, HWND windowHandle)
  {
    HMODULE vulkanLibrary = LoadLibrary("vulkan-1.dll");
    if(!vulkanLibrary) return false;
    #define EXPORTED_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(GetProcAddress(vulkanLibrary, #name));
    #define GLOBAL_LEVEL_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name));
    #include "ListOfVulkanFunctions.inl"
    initInstance(loadInstanceInfo());

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = winInstanceHandle;
    surfaceCreateInfo.hwnd = windowHandle;
    checkResult(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, allocator, &presentationSurface));
    if(!presentationSurface) throw VulkanError{"Vulkan error: failed to create presentatioin surface"};

    initDevice(findSuitablePhysicalDevice());

    return true;
  }
#endif
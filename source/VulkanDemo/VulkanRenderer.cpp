#include "DarEngine.h"
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
  static const std::array<const char*, 2> instanceExtensions = {"VK_KHR_surface", "VK_KHR_win32_surface"};
#endif

#define VK_NO_PROTOTYPES
#include "vulkan.h"

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

#define checkResultErrorCase(enumValue) case enumValue: throw VulkanError{__FUNCTION__ ": " #enumValue}; 
#define checkResult(func) \
switch(func) \
{ \
  case VK_SUCCESS: break; \
  checkResultErrorCase(VK_ERROR_OUT_OF_HOST_MEMORY) \
  checkResultErrorCase(VK_ERROR_OUT_OF_DEVICE_MEMORY) \
  checkResultErrorCase(VK_ERROR_INITIALIZATION_FAILED) \
  checkResultErrorCase(VK_ERROR_LAYER_NOT_PRESENT) \
  checkResultErrorCase(VK_ERROR_EXTENSION_NOT_PRESENT) \
  checkResultErrorCase(VK_ERROR_INCOMPATIBLE_DRIVER) \
  checkResultErrorCase(VK_ERROR_FEATURE_NOT_PRESENT) \
  checkResultErrorCase(VK_ERROR_TOO_MANY_OBJECTS) \
  checkResultErrorCase(VK_ERROR_DEVICE_LOST) \
  checkResultErrorCase(VK_ERROR_SURFACE_LOST_KHR) \
  checkResultErrorCase(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR) \
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
  const std::vector<const char*> layersToEnable = {}; 
  const De::ApplicationInfo applicationInfo = {}; 
  const VkAllocationCallbacks* allocator = nullptr;
  VkDevice device = nullptr;
  VkSurfaceKHR presentationSurface = nullptr;
  VkSwapchainKHR swapchain = nullptr;

  uint32_t selectUniversalQueueFamily(VkPhysicalDevice physicalDevice)
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

  void initInstance(const VkInstanceCreateInfo& info)
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

  VkPhysicalDevice findSuitablePhysicalDevice()
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

  VkApplicationInfo makeVkApplicationInfo(const De::ApplicationInfo applicationInfo)
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
  VkInstanceCreateInfo	makeVkInstanceCreateInfo(const VkApplicationInfo& vkApplicationInfo, 
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
    const std::array<const char*, 1> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
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

  void initSwapchain(VkPhysicalDevice physicalDevice)
  {
    uint32_t presentModesCount;
    checkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, nullptr));
    VkPresentModeKHR presentModes[VK_PRESENT_MODE_RANGE_SIZE_KHR];
    checkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, presentModes));
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR; // preferred
    for(int i = 0; i < presentModesCount; ++i)
    {
      if(presentModes[i] == presentMode) {
        break;
      } else if(i == presentModesCount - 1)
      {
        presentMode = VK_PRESENT_MODE_FIFO_KHR; // the only one which has to be supported
      }
    }
    VkSurfaceCapabilitiesKHR presentationSurfaceCapabalities;
    checkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, presentationSurface, &presentationSurfaceCapabalities));
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = presentationSurface;
    createInfo.minImageCount = (presentationSurfaceCapabalities.maxImageCount >= 3) && presentMode == VK_PRESENT_MODE_MAILBOX_KHR ? 3 : 2;
    createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    darAssert(presentationSurfaceCapabalities.currentExtent.width != 0xFFFFFFFF); // if equals, the surface size (window size) is determined by the size of the image
    createInfo.imageExtent = presentationSurfaceCapabalities.currentExtent;       // so at this line, we would have to get the image size different way
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // "VK_SHARING_MODE_EXCLUSIVE specifies that access to any range or image subresource of the object will be exclusive to a single queue family at a time."
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // "the image is treated as if it has a constant alpha of 1.0"
    createInfo.presentMode = presentMode;
    createInfo.clipped = true;
    checkResult(vkCreateSwapchainKHR(device, &createInfo, allocator, &swapchain));
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
    darAssert(presentationSurface != nullptr);

    VkPhysicalDevice physicalDevice = findSuitablePhysicalDevice();
    initDevice(physicalDevice);
    initSwapchain(physicalDevice);

    return true;
  }
#endif
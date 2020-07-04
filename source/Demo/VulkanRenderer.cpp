#define DAR_MODULE_NAME "VulkanRenderer"
#include "DarEngine.hpp"
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
  #define VK_USE_PLATFORM_WIN32_KHR
  static const char* instanceExtensions[] = {
    "VK_KHR_surface", "VK_KHR_win32_surface"
  #ifdef DAR_DEBUG 
    ,"VK_EXT_debug_utils"
  #endif
  };
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

#define _resultToStringCase(error) case error: sprintf_s(string, stringLength, #error); break;
static void resultToString(VkResult error, char* string, int stringLength)
{
  switch(error) {
    _resultToStringCase(VK_ERROR_OUT_OF_HOST_MEMORY)
    _resultToStringCase(VK_ERROR_OUT_OF_DEVICE_MEMORY) 
    _resultToStringCase(VK_ERROR_INITIALIZATION_FAILED) 
    _resultToStringCase(VK_ERROR_LAYER_NOT_PRESENT) 
    _resultToStringCase(VK_ERROR_EXTENSION_NOT_PRESENT) 
    _resultToStringCase(VK_ERROR_INCOMPATIBLE_DRIVER) 
    _resultToStringCase(VK_ERROR_FEATURE_NOT_PRESENT) 
    _resultToStringCase(VK_ERROR_TOO_MANY_OBJECTS) 
    _resultToStringCase(VK_ERROR_DEVICE_LOST) 
    _resultToStringCase(VK_ERROR_SURFACE_LOST_KHR) 
    _resultToStringCase(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
    default: sprintf_s(string, stringLength, "unkown result");
  }
}

static bool _checkResultImpl(VkResult result, const char* parentFunction, int lineNumber, const char* functionCall)
{
  switch(result) 
  { 
    case VK_SUCCESS:
    case VK_TIMEOUT:
    case VK_NOT_READY:
      return true;
    case VK_SUBOPTIMAL_KHR:{
      char resultString[64];
      resultToString(result, resultString, 64);
      logWarning("%s:%s returned %s", parentFunction, functionCall, resultString);
    return true;}
    default:{ 
      char resultString[64];
      resultToString(result, resultString, 64); 
      logError("%s:%s returned %s", parentFunction, functionCall, resultString); 
    return false;}
  } 
}
#define checkResult(func) _checkResultImpl(func, __FUNCTION__, __LINE__, #func)

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
  const De::ApplicationInfo applicationInfo = {}; 
  const VkAllocationCallbacks* allocator = nullptr;
  VkDevice device = nullptr;
  VkSurfaceKHR presentationSurface = nullptr;
  VkSwapchainKHR swapchain = nullptr;
  VkImage* swapchainImages = nullptr;
  uint32_t swapchainImageCount = 0;
  VkFence presentationImageFence = nullptr;
  VkImage presentationImage = nullptr;

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

  void enumarateAvailableInstanceExtensions()
  {
    uint32_t instanceExtensionsCount{0};
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, nullptr);
		availableInstanceExtensions.resize(instanceExtensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionsCount, availableInstanceExtensions.data());
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
  
  VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                               VkDebugUtilsMessageTypeFlagsEXT messageType,
                                               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                               void* pUserData) 
  {
    switch(messageSeverity) 
    {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        logError("%s", pCallbackData->pMessage);   
      break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        logWarning("%s", pCallbackData->pMessage);
      break;
      default:
      break;
    }
    return false;
  }

  void initInstance()
  {
    enumarateAvailableInstanceExtensions();
    checkInstanceExtensionsAvailability();
    
    VkApplicationInfo vkApplicationInfo;
	  vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	  vkApplicationInfo.pNext = nullptr;
	  vkApplicationInfo.pApplicationName = applicationInfo.name.data();
	  vkApplicationInfo.applicationVersion = applicationInfo.version.major << 22 | 
		  applicationInfo.version.minor << 12 | 
		  applicationInfo.version.patch;	// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VK_MAKE_VERSION.html
	  vkApplicationInfo.pEngineName = "DarEngine";
	  vkApplicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	  vkApplicationInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo{};
	  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	  createInfo.pApplicationInfo = &vkApplicationInfo;
    #ifdef DAR_DEBUG
      const char* layers[] = {"VK_LAYER_LUNARG_standard_validation"};
      createInfo.enabledLayerCount = arrayCount(layers);
	    createInfo.ppEnabledLayerNames = layers;
    #endif
	  createInfo.enabledExtensionCount = arrayCount(instanceExtensions);
	  createInfo.ppEnabledExtensionNames = instanceExtensions;
    checkResult(vkCreateInstance(&createInfo, allocator, &instance));

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

    #ifdef DAR_DEBUG
      VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
      messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      messengerCreateInfo.messageSeverity = //VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | 
                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
      messengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      messengerCreateInfo.pfnUserCallback = debugCallback;
      VkDebugUtilsMessengerEXT messenger = nullptr;
      vkCreateDebugUtilsMessengerEXT(instance, &messengerCreateInfo, allocator, &messenger);
    #endif
  }

  VkPhysicalDevice findSuitablePhysicalDevice()
  {
    uint32_t devicesCount{};
    checkResult(vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr));
    VkPhysicalDevice physicalDevices[8];
    checkResult(vkEnumeratePhysicalDevices(instance, &devicesCount, physicalDevices));
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
    return physicalDevices[0];
  }

  void loadDeviceCoreFunctions()
  {
    if(device)
    {
      #define DEVICE_LEVEL_VULKAN_FUNCTION(name) \
      name = (PFN_##name) vkGetDeviceProcAddr(device, #name); \
      if(!name) throw VulkanError{std::string("could not load vulkan function ") + std::string(#name)};
      #include "ListOfVulkanFunctions.inl"
    }
  }
  void loadDeviceSwapchainFunctions()
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
    float queuePriorities[] = {1.f};
    queueInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &queueInfo;
#ifdef DAR_DEBUG
    const char* layers[] = {"VK_LAYER_LUNARG_standard_validation"};
    info.enabledLayerCount = arrayCount(layers);
    info.ppEnabledLayerNames = layers;
#else
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = nullptr;
#endif
    const char* extensions[] = {"VK_KHR_swapchain"};
    info.ppEnabledExtensionNames = extensions;
    info.enabledExtensionCount = arrayCount(extensions);
    VkPhysicalDeviceFeatures features{};
    info.pEnabledFeatures = &features;
    checkResult(vkCreateDevice(physicalDevice, &info, allocator, &device));
    loadDeviceCoreFunctions();
    loadDeviceSwapchainFunctions();
  }

  VkFence createFence(int flags)
  {
    VkFenceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = flags;
    VkFence fence = nullptr;
    checkResult(vkCreateFence(device, &createInfo, allocator, &fence));
    return fence;
  }

  bool initSwapchain(VkPhysicalDevice physicalDevice)
  {
    uint32_t presentModesCount;
    if(!checkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, nullptr))) return false;
    VkPresentModeKHR presentModes[VK_PRESENT_MODE_RANGE_SIZE_KHR];
    if(!checkResult(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, presentModes))) return false;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR; // preferred
    for(uint32_t i = 0; i < presentModesCount; ++i)
    {
      if(presentModes[i] == presentMode) {
        break;
      } else if(i == presentModesCount - 1)
      {
        presentMode = VK_PRESENT_MODE_FIFO_KHR; // the only one which has to be supported
      }
    }
    VkSurfaceCapabilitiesKHR presentationSurfaceCapabalities;
    if(!checkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, presentationSurface, &presentationSurfaceCapabalities))) return false;
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &surfaceFormatCount, nullptr); // call it just to silence the validation error
    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &surfaceFormatCount, surfaceFormats.data()); // call it just to silence the validation error
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = presentationSurface;
    createInfo.minImageCount = (presentationSurfaceCapabalities.maxImageCount >= 3) && presentMode == VK_PRESENT_MODE_MAILBOX_KHR ? 3 : 2;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    assert(presentationSurfaceCapabalities.currentExtent.width != 0xFFFFFFFF); // if equals, the surface size (window size) is determined by the size of the image
    createInfo.imageExtent = presentationSurfaceCapabalities.currentExtent;       // so at this line, we would have to get the image size different way
    createInfo.imageArrayLayers = 1; // "imageArrayLayers is the number of views in a multiview/stereo surface. For non-stereoscopic-3D applications, this value is 1."
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // "VK_SHARING_MODE_EXCLUSIVE specifies that access to any range or image subresource of the object will be exclusive to a single queue family at a time."
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // "the image is treated as if it has a constant alpha of 1.0"
    createInfo.presentMode = presentMode;
    createInfo.clipped = true;
    if(!checkResult(vkCreateSwapchainKHR(device, &createInfo, allocator, &swapchain))) return false;
    if(!checkResult(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr))) return false;
    swapchainImages = (VkImage*)malloc(swapchainImageCount * sizeof(VkImage));
    if(!checkResult(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages))) return false;
    presentationImageFence = createFence(false);
    uint32_t nextImageIndex;
    checkResult(vkAcquireNextImageKHR(device, swapchain, 2000000000, nullptr, presentationImageFence, &nextImageIndex));
    presentationImage = swapchainImages[nextImageIndex];
    return true;
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
    initInstance();

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = winInstanceHandle;
    surfaceCreateInfo.hwnd = windowHandle;
    checkResult(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, allocator, &presentationSurface));
    assert(presentationSurface != nullptr);

    VkPhysicalDevice physicalDevice = findSuitablePhysicalDevice();
    initDevice(physicalDevice);
    if(!initSwapchain(physicalDevice)) return false;

    return true;
  }
#endif

void rendererPresent()
{
}
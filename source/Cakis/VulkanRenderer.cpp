#define DAR_MODULE_NAME "VulkanRenderer"

#include "VulkanRenderer.h"

#include <memory>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <array>

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include "vulkan.h"

#include "ApplicationInfo.hpp"
#include "DarEngine.hpp"
#include "Exception.hpp"
#include "File.hpp"

namespace 
{
#ifdef DAR_DEBUG
  const char* const layers[] = { "VK_LAYER_LUNARG_standard_validation" };
  uint32_t layerCount = arrayCount(layers);
#else
  const char* const * layers = nullptr;
  uint32_t layerCount = 0;
#endif
  static const char* requiredInstanceExtensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME
#ifdef _WIN32
    , VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
#ifdef DAR_DEBUG 
    , VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
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
VkSwapchainCreateInfoKHR swapChainInfo = {};
VkSwapchainKHR swapChain = nullptr;
VkImage* swapChainImages = nullptr;
VkImageView* swapChainImageViews = nullptr;
VkFramebuffer* swapChainFramebuffers = nullptr;
VkCommandBuffer* swapChainCommandBuffers = nullptr;
VkSemaphore* swapChainImageAvailableSemaphores = nullptr;
VkSemaphore* swapChainImageRenderFinishedSemaphores = nullptr;
VkFence* swapChainImageFences = nullptr;
uint32_t swapChainImageCount = 0;
VkCommandPool swapChainCommandPool = nullptr;
//VkFence presentationImageFence = nullptr;
//VkImage presentationImage = nullptr;
uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
VkQueue graphicsQueue = nullptr;
VkQueue transferQueue = nullptr;
VkQueue presentQueue = nullptr;
VkRenderPass renderPass = nullptr;
VkPipelineLayout pipelineLayout = nullptr;
VkPipeline graphicsPipeline = nullptr;
uint64_t renderCount = 0;

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
      //&& familyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT 
      && familyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT)
    {
      VkBool32 presentationSupported = false;
      checkResult(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, presentationSurface, &presentationSupported));
      if(presentationSupported) return i;
    }
  }
  throw VulkanRenderer::Exception("Failed to find suitable queue family");
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
	for(auto extension : requiredInstanceExtensions)
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
		throw VulkanRenderer::Exception{errorMessage};
	}
}
  
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData
) 
{
  switch(messageSeverity) 
  {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      logInfo("%s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      logWarning("%s", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      logError("%s", pCallbackData->pMessage);   
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
  createInfo.pNext = &messengerCreateInfo;
#endif
	createInfo.pApplicationInfo = &vkApplicationInfo;
  createInfo.enabledLayerCount = layerCount;
  createInfo.ppEnabledLayerNames = layers;
	createInfo.enabledExtensionCount = arrayCount(requiredInstanceExtensions);
	createInfo.ppEnabledExtensionNames = requiredInstanceExtensions;
  checkResult(vkCreateInstance(&createInfo, allocator, &instance));

  #define INSTANCE_LEVEL_VULKAN_FUNCTION(name)	\
	name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );	\
	if(!name)	\
	{	\
		throw VulkanRenderer::Exception{std::string("could not load vulkan function ") + std::string(#name) };	\
	}
  #define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name)	\
	name = (PFN_##name)vkGetInstanceProcAddr( instance, #name ); \
  if(!name)	\
	{	\
		throw VulkanRenderer::Exception{std::string("could not load vulkan function ") + std::string(#name) };	\
	}
  #include "ListOfVulkanFunctions.inl"

  #ifdef DAR_DEBUG
    VkDebugUtilsMessengerEXT messenger = nullptr;
    checkResult(vkCreateDebugUtilsMessengerEXT(instance, &messengerCreateInfo, allocator, &messenger));
  #endif
}

void initializePresentationSurface(HWND window)
{
  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
  surfaceCreateInfo.hwnd = window;
  checkResult(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, allocator, &presentationSurface));
  assert(presentationSurface != nullptr);
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
    if(!name) throw VulkanRenderer::Exception{std::string("could not load vulkan function ") + std::string(#name)};
    #include "ListOfVulkanFunctions.inl"
  }
}
void loadDeviceExtensionFunctions()
{
  #define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name) \
  name = (PFN_##name) vkGetDeviceProcAddr(device, #name); \
  if(!name) throw VulkanRenderer::Exception{std::string("could not load vulkan function ") + std::string(#name)};
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

  graphicsQueueFamilyIndex = queueInfo.queueFamilyIndex;

  VkDeviceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info.queueCreateInfoCount = 1;
  info.pQueueCreateInfos = &queueInfo;
  info.enabledLayerCount = layerCount;
  info.ppEnabledLayerNames = layers;
  const char* extensions[] = {"VK_KHR_swapchain"};
  info.ppEnabledExtensionNames = extensions;
  info.enabledExtensionCount = arrayCount(extensions);
  VkPhysicalDeviceFeatures features{};
  info.pEnabledFeatures = &features;
  checkResult(vkCreateDevice(physicalDevice, &info, allocator, &device));

  loadDeviceCoreFunctions();
  loadDeviceExtensionFunctions();

  vkGetDeviceQueue(device, queueInfo.queueFamilyIndex, 0, &graphicsQueue);
  transferQueue = graphicsQueue;
  presentQueue = graphicsQueue;
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

bool initSwapChain(VkPhysicalDevice physicalDevice)
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
    } else if(i == presentModesCount - 1) {
      presentMode = VK_PRESENT_MODE_FIFO_KHR; // the only one which has to be supported
    }
  }
  VkSurfaceCapabilitiesKHR presentationSurfaceCapabalities;
  if(!checkResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, presentationSurface, &presentationSurfaceCapabalities))) return false;
  uint32_t surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &surfaceFormatCount, nullptr); // call it just to silence the validation error
  std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &surfaceFormatCount, surfaceFormats.data()); // call it just to silence the validation error

  swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainInfo.surface = presentationSurface;
  swapChainInfo.minImageCount = std::min(presentationSurfaceCapabalities.minImageCount + 1, presentationSurfaceCapabalities.maxImageCount);;
  swapChainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
  swapChainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  assert(presentationSurfaceCapabalities.currentExtent.width != 0xFFFFFFFF); // if equals, the surface size (window size) is determined by the size of the image
  swapChainInfo.imageExtent = presentationSurfaceCapabalities.currentExtent;       // so at this line, we would have to get the image size different way
  swapChainInfo.imageArrayLayers = 1; // "imageArrayLayers is the number of views in a multiview/stereo surface. For non-stereoscopic-3D applications, this value is 1."
  swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  // "VK_SHARING_MODE_EXCLUSIVE specifies that access to any range or image subresource of the object will be exclusive to a single queue family at a time."
  swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapChainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // "the image is treated as if it has a constant alpha of 1.0"
  swapChainInfo.presentMode = presentMode;
  swapChainInfo.clipped = true;
  if(!checkResult(vkCreateSwapchainKHR(device, &swapChainInfo, allocator, &swapChain))) return false;

  if(!checkResult(vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, nullptr))) return false;
  swapChainImages = (VkImage*)malloc(swapChainImageCount * sizeof(VkImage));
  if(!checkResult(vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, swapChainImages))) return false;

  swapChainImageViews = new VkImageView[swapChainImageCount];
  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = swapChainInfo.imageFormat;
  imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.levelCount = 1;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = 1;
  for(uint32_t i = 0; i < swapChainImageCount; ++i) {
    imageViewCreateInfo.image = swapChainImages[i];
    checkResult(vkCreateImageView(device, &imageViewCreateInfo, allocator, swapChainImageViews + i));
  }

  /*presentationImageFence = createFence(false);*/
  return true;
}

void initializeRenderPass()
{
  VkAttachmentDescription colorAttachmentDesc;
  colorAttachmentDesc.flags = 0;
  colorAttachmentDesc.format = swapChainInfo.imageFormat;
  colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef;
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDesc;
  subpassDesc.flags = 0;
  subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDesc.inputAttachmentCount = 0;
  subpassDesc.pInputAttachments = nullptr;
  subpassDesc.colorAttachmentCount = 1;
  subpassDesc.pColorAttachments = &colorAttachmentRef;
  subpassDesc.pResolveAttachments = nullptr;
  subpassDesc.pDepthStencilAttachment = nullptr;
  subpassDesc.preserveAttachmentCount = 0;
  subpassDesc.pPreserveAttachments = nullptr;

  VkSubpassDependency dependency;
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = 0;

  VkRenderPassCreateInfo renderPassInfo;
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.pNext = nullptr;
  renderPassInfo.flags = 0;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachmentDesc;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpassDesc;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  checkResult(vkCreateRenderPass(device, &renderPassInfo, allocator, &renderPass));
}

class ShaderModule {
public:
  explicit ShaderModule(const uint32_t* code, size_t codeSize)
  {
    VkShaderModuleCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = codeSize;
    createInfo.pCode = code;

    checkResult(vkCreateShaderModule(device, &createInfo, allocator, &handle));
  }
  ShaderModule(const ShaderModule& other) = delete;
  ShaderModule(ShaderModule&& other) = delete;
  ~ShaderModule()
  {
    vkDestroyShaderModule(device, handle, allocator);
  }

  operator VkShaderModule() { return handle; }

private:
  VkShaderModule handle;
};

void initializePipeline()
{
  std::vector<uint8_t> shaderCode;

#define getShaderPath(shaderName) "shaders/" shaderName ".spv"
  De::readEntireFile(getShaderPath("triangle.vert"), shaderCode);
  ShaderModule triangleVertexShader(reinterpret_cast<uint32_t*>(shaderCode.data()), shaderCode.size());

  De::readEntireFile(getShaderPath("triangle.frag"), shaderCode);
  ShaderModule triangleFragmentShader(reinterpret_cast<uint32_t*>(shaderCode.data()), shaderCode.size());

  VkPipelineLayoutCreateInfo pipelineLayoutInfo;
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pNext = nullptr;
  pipelineLayoutInfo.flags = 0;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  checkResult(vkCreatePipelineLayout(device, &pipelineLayoutInfo, allocator, &pipelineLayout));

  VkPipelineShaderStageCreateInfo shaderStageInfos[2];

  shaderStageInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageInfos[0].pNext = nullptr;
  shaderStageInfos[0].flags = 0;
  shaderStageInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStageInfos[0].module = triangleVertexShader;
  shaderStageInfos[0].pName = "main";
  shaderStageInfos[0].pSpecializationInfo = nullptr;

  shaderStageInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageInfos[1].pNext = nullptr;
  shaderStageInfos[1].flags = 0;
  shaderStageInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStageInfos[1].module = triangleFragmentShader;
  shaderStageInfos[1].pName = "main";
  shaderStageInfos[1].pSpecializationInfo = nullptr;

  VkPipelineVertexInputStateCreateInfo vertexInputStateInfo;
  vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateInfo.pNext = nullptr;
  vertexInputStateInfo.flags = 0;
  vertexInputStateInfo.vertexBindingDescriptionCount = 0;
  vertexInputStateInfo.pVertexBindingDescriptions = nullptr;
  vertexInputStateInfo.vertexAttributeDescriptionCount = 0;
  vertexInputStateInfo.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo;
  inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateInfo.pNext = nullptr;
  inputAssemblyStateInfo.flags = 0;
  inputAssemblyStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport;
  viewport.x = 0.f;
  viewport.y = 0.f;
  viewport.width = static_cast<float>(swapChainInfo.imageExtent.width);
  viewport.height = static_cast<float>(swapChainInfo.imageExtent.height);
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;

  VkRect2D scissor;
  scissor.offset = { 0, 0 };
  scissor.extent = swapChainInfo.imageExtent;

  VkPipelineViewportStateCreateInfo viewportStateInfo;
  viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateInfo.pNext = nullptr;
  viewportStateInfo.flags = 0;
  viewportStateInfo.viewportCount = 1;
  viewportStateInfo.pViewports = &viewport;
  viewportStateInfo.scissorCount = 1;
  viewportStateInfo.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizerStateInfo;
  rasterizerStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerStateInfo.pNext = nullptr;
  rasterizerStateInfo.flags = 0;
  rasterizerStateInfo.depthClampEnable = VK_FALSE;
  rasterizerStateInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizerStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizerStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizerStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizerStateInfo.depthBiasEnable = VK_FALSE;
  rasterizerStateInfo.depthBiasConstantFactor = 0.f;
  rasterizerStateInfo.depthBiasClamp = 0.f;
  rasterizerStateInfo.depthBiasSlopeFactor = 0.f;
  rasterizerStateInfo.lineWidth = 1.f;

  VkPipelineMultisampleStateCreateInfo multisampleStateInfo;
  multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleStateInfo.pNext = nullptr;
  multisampleStateInfo.flags = 0;
  multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleStateInfo.sampleShadingEnable = VK_FALSE;
  multisampleStateInfo.minSampleShading = 1.f;
  multisampleStateInfo.pSampleMask = nullptr;
  multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
  multisampleStateInfo.alphaToOneEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
  colorBlendAttachmentState.blendEnable = VK_FALSE;
  colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
    VK_COLOR_COMPONENT_G_BIT | 
    VK_COLOR_COMPONENT_B_BIT | 
    VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlendStateInfo;
  colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateInfo.pNext = nullptr;
  colorBlendStateInfo.flags = 0;
  colorBlendStateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
  colorBlendStateInfo.attachmentCount = 1;
  colorBlendStateInfo.pAttachments = &colorBlendAttachmentState;
  colorBlendStateInfo.blendConstants[0] = 0.f;
  colorBlendStateInfo.blendConstants[1] = 0.f;
  colorBlendStateInfo.blendConstants[2] = 0.f;
  colorBlendStateInfo.blendConstants[3] = 0.f;

  VkGraphicsPipelineCreateInfo pipelineInfo;
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pNext = nullptr;
  pipelineInfo.flags = 0;
  pipelineInfo.stageCount = arrayCount(shaderStageInfos);
  pipelineInfo.pStages = shaderStageInfos;
  pipelineInfo.pVertexInputState = &vertexInputStateInfo;
  pipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;
  pipelineInfo.pTessellationState = nullptr;
  pipelineInfo.pViewportState = &viewportStateInfo;
  pipelineInfo.pRasterizationState = &rasterizerStateInfo;
  pipelineInfo.pMultisampleState = &multisampleStateInfo;
  pipelineInfo.pDepthStencilState = nullptr;
  pipelineInfo.pColorBlendState = &colorBlendStateInfo;
  pipelineInfo.pDynamicState = nullptr;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = 0;

  checkResult(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, allocator, &graphicsPipeline));
}

void initializeSwapChainFramebuffers()
{
  swapChainFramebuffers = new VkFramebuffer[swapChainImageCount];

  VkFramebufferCreateInfo framebufferInfo;
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.pNext = nullptr;
  framebufferInfo.flags = 0;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.width = swapChainInfo.imageExtent.width;
  framebufferInfo.height = swapChainInfo.imageExtent.height;
  framebufferInfo.layers = 1;
  for(uint32_t i = 0; i < swapChainImageCount; ++i) {
    framebufferInfo.pAttachments = swapChainImageViews + i;
    checkResult(vkCreateFramebuffer(device, &framebufferInfo, allocator, swapChainFramebuffers + i));
  }
}

class CommandRecorder
{
public:
  explicit CommandRecorder(VkCommandBuffer commandBuffer)
    : commandBuffer(commandBuffer)
  {
    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    checkResult(vkBeginCommandBuffer(commandBuffer, &beginInfo));
  }
  CommandRecorder(const CommandRecorder& other) = delete;
  CommandRecorder(CommandRecorder&& other) = delete;
  ~CommandRecorder()
  {
    VkResult result = vkEndCommandBuffer(commandBuffer);
    if(result != VK_SUCCESS) {
      logError("Failed to end command buffer: %d", result);
    }
  }

private:
  VkCommandBuffer commandBuffer;
};

class RenderPassRecorder
{
public:
  RenderPassRecorder(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo& beginInfo, VkSubpassContents contents)
    : commandBuffer(commandBuffer)
  {
    vkCmdBeginRenderPass(commandBuffer, &beginInfo, contents);
  }
  RenderPassRecorder(const RenderPassRecorder& other) = delete;
  RenderPassRecorder(RenderPassRecorder&& other) = delete;
  ~RenderPassRecorder()
  {
    vkCmdEndRenderPass(commandBuffer);
  }

private:
  VkCommandBuffer commandBuffer;
};

void initializeCommandBuffers()
{
  VkCommandPoolCreateInfo poolInfo;
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.pNext = nullptr;
  poolInfo.flags = 0;
  poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

  checkResult(vkCreateCommandPool(device, &poolInfo, allocator, &swapChainCommandPool));

  swapChainCommandBuffers = new VkCommandBuffer[swapChainImageCount];

  VkCommandBufferAllocateInfo allocateInfo;
  allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.pNext = nullptr;
  allocateInfo.commandPool = swapChainCommandPool;
  allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandBufferCount = swapChainImageCount;
  checkResult(vkAllocateCommandBuffers(device, &allocateInfo, swapChainCommandBuffers));

  VkRenderPassBeginInfo renderPassBeginInfo;
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.pNext = nullptr;
  renderPassBeginInfo.renderPass = renderPass;
  renderPassBeginInfo.renderArea.offset = { 0, 0 };
  renderPassBeginInfo.renderArea.extent = swapChainInfo.imageExtent;
  VkClearValue clearColor;
  clearColor.color = { 0.2f, 0.2f, 0.2f, 1.0f };
  clearColor.depthStencil = {};
  renderPassBeginInfo.clearValueCount = 1;
  renderPassBeginInfo.pClearValues = &clearColor;
  for(uint32_t i = 0; i < swapChainImageCount; ++i) {
    VkCommandBuffer commandBuffer = swapChainCommandBuffers[i];

    CommandRecorder commandRecorder(commandBuffer);

    renderPassBeginInfo.framebuffer = swapChainFramebuffers[i];
    RenderPassRecorder renderPassRecorder(commandBuffer, renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  }
}

void initializeSyncObjects()
{
  swapChainImageAvailableSemaphores = new VkSemaphore[swapChainImageCount];
  swapChainImageRenderFinishedSemaphores = new VkSemaphore[swapChainImageCount];
  swapChainImageFences = new VkFence[swapChainImageCount];

  VkSemaphoreCreateInfo semaphoreInfo;
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.pNext = nullptr;
  semaphoreInfo.flags = 0;

  VkFenceCreateInfo fenceInfo;
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = nullptr;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for(uint32_t i = 0; i < swapChainImageCount; ++i) {
    checkResult(vkCreateSemaphore(device, &semaphoreInfo, allocator, swapChainImageAvailableSemaphores + i));
    checkResult(vkCreateSemaphore(device, &semaphoreInfo, allocator, swapChainImageRenderFinishedSemaphores + i));
    checkResult(vkCreateFence(device, &fenceInfo, allocator, swapChainImageFences + i));
  }
}

} // anonymous namespace

VulkanRenderer::VulkanRenderer(HWND window)
{
  HMODULE vulkanLibrary = LoadLibrary("vulkan-1.dll");
  if(!vulkanLibrary) {
    throw Exception("Failed to load vulkan library.");
  }
  #define EXPORTED_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(GetProcAddress(vulkanLibrary, #name));
  #define GLOBAL_LEVEL_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name));
  #include "ListOfVulkanFunctions.inl"
  initInstance();

  initializePresentationSurface(window);

  VkPhysicalDevice physicalDevice = findSuitablePhysicalDevice();
  initDevice(physicalDevice);
  if(!initSwapChain(physicalDevice)) {
    throw Exception("Failed to initialize swapChain.");
  }
  initializeRenderPass();
  initializePipeline();
  initializeSwapChainFramebuffers();
  initializeCommandBuffers();
  initializeSyncObjects();
}

void VulkanRenderer::onWindowResize(int clientAreaWidth, int clientAreaHeight)
{

}

void VulkanRenderer::render(const GameState& gameState)
{
  uint32_t syncIndex = renderCount % swapChainImageCount;

  uint32_t imageIndex;
  //TODO: error handling
  VkSemaphore swapChainImageAvailableSemaphore = swapChainImageAvailableSemaphores[syncIndex];
  vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, swapChainImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

  VkFence renderCommandsExecutedFence = swapChainImageFences[syncIndex];
  if(vkWaitForFences(device, 1, &renderCommandsExecutedFence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
    logError("Failed to wait for a swapchain image fence with index %u.", syncIndex);
  }
  vkResetFences(device, 1, &renderCommandsExecutedFence);
  
  VkSubmitInfo submitInfo;
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pNext = nullptr;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &swapChainImageAvailableSemaphore;
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = swapChainCommandBuffers + imageIndex;
  VkSemaphore swapChainImageRenderFinishedSemaphore = swapChainImageRenderFinishedSemaphores[syncIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &swapChainImageRenderFinishedSemaphore;

  checkResult(vkQueueSubmit(graphicsQueue, 1, &submitInfo, renderCommandsExecutedFence));

  VkPresentInfoKHR presentInfo;
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pNext = nullptr;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &swapChainImageRenderFinishedSemaphore;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapChain;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  //TODO: error handling
  vkQueuePresentKHR(presentQueue, &presentInfo);

  ++renderCount;
}
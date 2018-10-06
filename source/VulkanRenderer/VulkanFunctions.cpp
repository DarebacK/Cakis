#include "VulkanFunctions.hpp"
#include "Library.hpp"

namespace De::Vulkan {
// define variables for the vulkan functions we need
#define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name{nullptr};
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name{nullptr};
#include "ListOfVulkanFunctions.inl"

	// define vulkan library path
	const char* const vulkanLibraryPath = 
#if defined(_WIN32)
	"vulkan-1.dll";
#elif defined(__linux__)
	"libvulkan.so.1";
#endif

	void loadVulkanFunctions()
	{
		static Library vulkanLibrary{ vulkanLibraryPath };
		// redefine vulkan function macros to load the vulkan functions we need
#define EXPORTED_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(vulkanLibrary.loadFunction(#name));
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name));
#include "ListOfVulkanFunctions.inl"
	}
}
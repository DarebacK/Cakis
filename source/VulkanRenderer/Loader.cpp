#include "Loader.hpp"
#include "Library.hpp"
#include "Instance.hpp"
#include <set>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "Error.hpp"

namespace
{
	const char* const vulkanLibraryPath = 
#if defined(_WIN32)
	"vulkan-1.dll";
#elif defined(__linux__)
	"libvulkan.so.1";
#endif
}

namespace De::Vulkan
{
// define variables for the vulkan functions we need
#define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
//#define DEVICE_LEVEL_VULKAN_FUNCTION(name) PFN_##name name{nullptr};
//#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name{nullptr};
#include "ListOfVulkanFunctions.inl"
}

namespace
{
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
}

class De::Vulkan::Loader::Impl
{
public:
	Impl()
	{
		// redefine vulkan function macros to load the vulkan functions we need
#define EXPORTED_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(vulkanLibrary.loadFunction(#name));
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name));
#include "ListOfVulkanFunctions.inl"
		initAvailableExtensions();
	}
	Instance	loadInstance(const std::vector<const char*>& layersToEnable, 
					 const std::vector<const char*>& extensionsToLoad,
					 const ApplicationInfo& applicationInfo, 
					 const VkAllocationCallbacks* allocator)
	{
		checkExtensionsAvailability(extensionsToLoad);
		VkApplicationInfo vkApplicationInfo = makeVkApplicationInfo(applicationInfo);
		VkInstanceCreateInfo vkInstanceCreateInfo = makeVkInstanceCreateInfo(vkApplicationInfo, 
			layersToEnable, extensionsToLoad);
		Instance instance{vkInstanceCreateInfo, allocator};
		return instance;
	}
	const std::vector<VkExtensionProperties>& getAvailableExtensions() const
	{
		return availableExtensions;
	}

private:
	Library vulkanLibrary{vulkanLibraryPath};
	std::vector<VkExtensionProperties> availableExtensions;

	void initAvailableExtensions()
	{
		uint32_t extensionsCount{0};
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, 
			nullptr);
		availableExtensions.resize(extensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, 
			availableExtensions.data());
	}
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
};

De::Vulkan::Loader::Loader()
	:pImpl{std::make_unique<Impl>()}
{}
De::Vulkan::Loader::~Loader() = default;

De::Vulkan::Instance De::Vulkan::Loader::loadInstance(const std::vector<const char*>& layersToEnable, 
	const std::vector<const char*>& extensionsToLoad, 
	const ApplicationInfo& applicationInfo, 
	const VkAllocationCallbacks* allocator)
{
	return pImpl->loadInstance(layersToEnable, extensionsToLoad, applicationInfo, allocator);
}
const std::vector<VkExtensionProperties>& De::Vulkan::Loader::getAvailableExtensions() const
{
	return pImpl->getAvailableExtensions();
}
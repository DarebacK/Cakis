#include "Error.hpp"

using namespace De::Vk;

Error::Error(std::string message)
	:Exception{"Vulkan error: " + message}
{}
Error::Error(const char* message)
	:Error{std::string{message}}
{}


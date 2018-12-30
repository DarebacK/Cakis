#pragma once
#include "Exception.hpp"

namespace De::Vulkan
{
	class Error : public Exception
	{
	public:
		explicit	Error(std::string message);
		explicit	Error(const char* message);
					Error(const Error& other) = default;
					Error(Error&& other) = default;
					Error&	operator=(const Error& rhs) = default;
					Error&	operator=(Error&& rhs) = default;
		virtual		~Error() = default;
	};
}
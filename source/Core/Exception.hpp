#pragma once

#include <string>
#include <exception>

namespace De
{
	class Exception : public std::exception
	{
	public:
		explicit	Exception(std::string message);
		explicit	Exception(const char* message);
					Exception(const Exception& other) = default;
					Exception(Exception&& other) = default;
		Exception&	operator=(const Exception& rhs) = default;
		Exception&	operator=(Exception&& rhs) = default;
		virtual		~Exception() = default;

		const char* what() const noexcept override;
	private:
		std::string message;
	};
}
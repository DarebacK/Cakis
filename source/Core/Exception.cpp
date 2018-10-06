#include "Exception.hpp"
#include <utility>

using namespace De;

Exception::Exception(std::string message)
	:message{std::move(message)}
{}
Exception::Exception(const char* message)
	:message{message}
{}
const char* Exception::what() const noexcept
{
	return message.c_str();
}

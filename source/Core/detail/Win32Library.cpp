#if defined(_WIN32)
#include "Win32Library.hpp"
#include "../Exception.hpp"
#include <sstream>

using namespace De;

Win32Library::Win32Library(const char* fileName)
{
	library = LoadLibrary(fileName);

	if (!library)
	{
		std::ostringstream errorMessage;
		errorMessage << "Couldn't load library named \""
			<< fileName << "\"";
		throw Exception(errorMessage.str());
	}
}

Win32Library::Win32Library(Win32Library&& other) noexcept
	:library{ other.library }
{
	other.library = nullptr;
}

Win32Library::~Win32Library()
{
	if (library)
	{
		FreeLibrary(library);
	}
}

Win32Library& Win32Library::operator=(Win32Library&& rhs) noexcept
{
	library = rhs.library;
	rhs.library = nullptr;
	return *this;
}

FARPROC Win32Library::loadFunction(const char* name)
{
	FARPROC function = GetProcAddress(library, name);
	if (!function)
	{
		std::ostringstream errorMessage;
		errorMessage << "Couldn't load function named \""
			<< name << "\"";
		throw Exception(errorMessage.str());
	}
	return function;
}
#endif
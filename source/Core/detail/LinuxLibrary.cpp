#if defined(__linux__)
#include "LinuxLibrary.hpp"
#include "../Exception.hpp"
#include <sstream>
#include <dlfcn.h>

using namespace De;

class LinuxLibrary::Impl
{
public:
	explicit Impl(const char* fileName)
	{
		library = dlopen(fileName, RTLD_NOW);
		if (!library)
		{
			std::ostringstream errorMessage;
			errorMessage << "Couldn't load library named \""
				<< fileName << "\"";
			throw Exception(errorMessage.str());
		}
	}
	Impl(const Impl& other) = delete;
	Impl(Impl&& other) noexcept
		:library{ other.library }
	{
		other.library = nullptr;
	}
	Impl& operator=(const Impl& rhs) = delete;
	Impl& operator=(Impl&& rhs) noexcept
	{
		library = rhs.library;
		rhs.library = nullptr;
		return *this;
	}
	~Impl()
	{
		if (library)
		{
			dlcose(library);
		}
	}

	void* loadFunction(const char* name)
	{
		void* function = dlsym(library, name);
		if (!function)
		{
			std::ostringstream errorMessage;
			errorMessage << "Couldn't load function named \""
				<< name << "\"";
			throw Exception(errorMessage.str());
		}
		return function;
	}
private:
	void* library{ nullptr };
};

LinuxLibrary::LinuxLibrary(const char* fileName)
	:pImpl{ std::make_unique<Impl>(fileName) }
{}
Library::~Library() = default;
void* LinuxLibrary::loadFunction(const char* name)
{
	return pImpl->loadFunction(name);
}
#endif
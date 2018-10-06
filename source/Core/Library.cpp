#include "Library.hpp"
#include "Exception.hpp"
#include <sstream>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif

using namespace De;

class Library::Impl
{
public:
	explicit Impl(const char* fileName)
	{
#if defined(_WIN32)
			library = LoadLibrary(fileName);
#elif defined(__linux__)
			library = dlopen(fileName, RTLD_NOW);
#endif
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
		:library{other.library}
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
		if(library)
		{
#if defined(_WIN32)
			FreeLibrary(static_cast<HMODULE>(library));
#elif defined(__linux__)
			dlcose(library);
#endif
		}
	}

	void* loadFunction(const char* name)
	{
		void* function{nullptr};
#if defined(_WIN32)
			function = GetProcAddress(static_cast<HMODULE>(library), name);
#elif defined(__linux__)
			function = dlsym(library, name);
#endif
		if(!function)
		{
			std::ostringstream errorMessage;
			errorMessage << "Couldn't load function named \""
				<< name << "\"";
			throw Exception(errorMessage.str());
		}
		return function;
	}
private:
	void* library{nullptr};
};

Library::Library(const char* fileName)
	:pImpl{std::make_unique<Impl>(fileName)}
{}
Library::~Library() = default;
void* Library::loadFunction(const char* name)
{
	return pImpl->loadFunction(name);
}

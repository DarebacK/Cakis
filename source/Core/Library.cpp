#include "Library.hpp"

#if defined(_WIN32)
#include "detail/Win32Library.hpp"
using LibraryType = De::Win32Library;
#elif defined(__linux__)
#include "detail/LinuxLibrary.hpp"
using LibraryType = De::LinuxLibrary;
#endif

using namespace De;

class Library::Impl
{
public:
	explicit Impl(const char* fileName)
	{
		library = std::make_unique<LibraryType>(fileName);
	}
	Impl(const Impl& other) = delete;
	Impl(Impl&& other) = default;
	Impl& operator=(const Impl& rhs) = delete;
	Impl& operator=(Impl&& rhs) = default;
	void* loadFunction(const char* name)
	{
		return library->loadFunction(name);
	}
private:
	std::unique_ptr<LibraryType> library;
};

Library::Library(const char* fileName)
	:pImpl{std::make_unique<Impl>(fileName)}
{}
Library::~Library() = default;
void* Library::loadFunction(const char* name)
{
	return pImpl->loadFunction(name);
}

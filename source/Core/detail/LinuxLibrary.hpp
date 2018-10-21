#pragma once
#include <memory>

namespace De
{
	class LinuxLibrary
	{
	public:
		explicit	LinuxLibrary(const char* fileName);
					LinuxLibrary(const LinuxLibrary& other) = delete;
					LinuxLibrary(LinuxLibrary&& other) = default;
					~LinuxLibrary() = default;
		LinuxLibrary&	operator=(const LinuxLibrary& rhs) = delete;
		LinuxLibrary&	operator=(LinuxLibrary&& rhs) = default;
		void*	loadFunction(const char* name);
	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
}

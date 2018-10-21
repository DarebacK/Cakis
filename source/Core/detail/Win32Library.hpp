#pragma once
#include <Windows.h>
namespace De
{
	class Win32Library
	{
	public:
		explicit	Win32Library(const char* fileName);
					Win32Library(const Win32Library& other) = delete;
					Win32Library(Win32Library&& other) noexcept;
					~Win32Library();
		Win32Library&	operator=(const Win32Library& rhs) = delete;
		Win32Library&	operator=(Win32Library&& rhs) noexcept;
		FARPROC	loadFunction(const char* name);
	private:
		HMODULE library{ nullptr };
	};
}
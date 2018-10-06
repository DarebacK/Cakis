#pragma once

namespace De::Platform
{
	constexpr bool isWin32()
	{
#if defined(_WIN32)
		return true;
#else
		return false;
#endif
	}
	constexpr bool isLinux()
	{
#if defined(__linux__)
		return true;
#else 
		return false;
#endif
	}
}
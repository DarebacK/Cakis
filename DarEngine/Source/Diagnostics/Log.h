#pragma once
#include <string>

namespace DarEngine
{
	inline void LogError(std::string message);
	inline void LogError(std::wstring message);
	inline void LogWarning(std::string message);
	inline void LogWarning(std::wstring message);
	inline void LogInfo(std::string message);
	inline void LogInfo(std::wstring message);
}

#include "Log.inl"

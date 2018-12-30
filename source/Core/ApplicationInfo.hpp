#pragma once
#include "Version.hpp"
#include <string>

namespace De
{
	struct ApplicationInfo
	{
		std::string name{};
		Version		version{};
	};
}
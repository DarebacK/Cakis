#pragma once

#include "stdafx.h"

inline int DarEngine::GetScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

inline int DarEngine::GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

#pragma once

#include "stdafx.h"

inline int Platform::GetScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

inline int Platform::GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

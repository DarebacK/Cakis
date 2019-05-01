#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
bool initVulkanRenderer(HINSTANCE instanceHandle, HWND windowHandle);
#endif _WIN32

void rendererPresent();
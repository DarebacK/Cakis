#pragma once

#ifdef _WIN32
#include <windows.h>
bool initVulkanRenderer(HINSTANCE instanceHandle, HWND windowHandle);
#endif _WIN32
#pragma once
#include <windows.h>

bool initD3D11Renderer(HWND window);
void rendererBeginFrame();
void rendererEndFrame();
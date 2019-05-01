#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GameState.h"

bool initD3D11Renderer(HWND window);
void render(const GameState& gameState);

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GameState.hpp"

namespace Renderer {
bool init(HWND window);
void onWindowResize(int clientAreaWidth, int clientAreaHeight);
void render(const GameState& gameState);
}

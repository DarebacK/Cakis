#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <Exception.hpp>

#include "GameState.hpp"

namespace Renderer {
  DECLARE_AND_DEFINE_SIMPLE_EXCEPTION(InitializeException)
  DECLARE_AND_DEFINE_SIMPLE_EXCEPTION(Exception)

  void initialize(HWND window);
  void onWindowResize(int clientAreaWidth, int clientAreaHeight);
  void render(const GameState& gameState);
}

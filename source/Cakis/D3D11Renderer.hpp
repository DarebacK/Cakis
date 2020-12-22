#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <Exception.hpp>

#include "GameState.hpp"

class D3D11Renderer {
public:
  DECLARE_AND_DEFINE_SIMPLE_EXCEPTION(InitializeException)
  DECLARE_AND_DEFINE_SIMPLE_EXCEPTION(Exception)

  explicit D3D11Renderer(HWND window);
  D3D11Renderer(const D3D11Renderer& other) = delete;
  D3D11Renderer(const D3D11Renderer&& other) = delete;
  ~D3D11Renderer() = default;

  void onWindowResize(int clientAreaWidth, int clientAreaHeight);
  void render(const GameState& gameState);
};

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <Exception.hpp>

#include "GameState.hpp"

class VulkanRenderer {
public:
  DECLARE_AND_DEFINE_SIMPLE_EXCEPTION(InitializeException)
  DECLARE_AND_DEFINE_SIMPLE_EXCEPTION(Exception)

  explicit VulkanRenderer(HWND window);
  VulkanRenderer(const VulkanRenderer& other) = delete;
  VulkanRenderer(const VulkanRenderer&& other) = delete;
  ~VulkanRenderer() = default;

  void onWindowResize(int clientAreaWidth, int clientAreaHeight);
  void render(const GameState& gameState);
};
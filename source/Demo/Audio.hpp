#pragma once

#include "GameState.hpp"

namespace Audio
{
  bool initialize();
  void update(const GameState& gameState);
  void uninitialize();
} // namespace Audio
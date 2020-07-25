#pragma once

#include "GameState.hpp"

class Audio
{
public:
  Audio();
  Audio(const Audio& other) = delete;
  Audio(Audio&& other) = delete;
  Audio& operator=(const Audio& rhs) = delete;
  Audio& operator=(Audio&& rhs) = delete;
  ~Audio();

  void update(const GameState& gameState);
private:
  bool isInitialized = true;
};
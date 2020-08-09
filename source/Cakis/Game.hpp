#pragma once

#include "GameState.hpp"

class Game
{
public:
  Game();
  void update(const GameState& lastState, GameState* nextState);
};
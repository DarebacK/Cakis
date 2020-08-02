#pragma once

#include "GameState.hpp"

class Game
{
public:
  void update(const GameState& lastState, GameState* nextState);
};
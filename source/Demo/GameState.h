#pragma once

struct KeyboardKey
{
  bool pressedDown;
};

struct Input
{
  KeyboardKey F1;
};

struct GameState
{
  Input input;
};



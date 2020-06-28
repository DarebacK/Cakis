#pragma once

struct MouseButton
{
  bool pressedDown;
  bool pressedUp;
};

struct Mouse
{
  int x;
  int y;
  MouseButton left;
  MouseButton middle;
  MouseButton right;
};

struct KeyboardKey
{
  bool pressedDown;
};

struct Input
{
  Mouse mouse;
  KeyboardKey enter;
  KeyboardKey F1;
  KeyboardKey rightAlt;
};

struct GameState
{
  Input input;
  float dTime;
  int clientAreaWidth;
  int clientAreaHeight;
};



#pragma once

#include <DarMath.hpp>

struct MouseButton
{
  bool pressedDown;
  bool isDown;
  bool pressedUp;
};

struct Mouse
{
  MouseButton left;
  MouseButton middle;
  MouseButton right;
  float dWheel;
};

struct Keyboard
{
  struct Key
  {
    bool pressedDown;
    bool pressedUp;
  };

  Key enter;
  Key F1;
  Key rightAlt;
};

struct Input
{
  Mouse mouse;
  Vec2i cursorPosition;
  Keyboard keyboard;
};

struct GameState
{
  Input input;

  float dTime;

  TrackSphere camera = TrackSphere(0.f, Pi / 8.f, 8.f, 2.f, 10.f);

  int clientAreaWidth;
  int clientAreaHeight;
};



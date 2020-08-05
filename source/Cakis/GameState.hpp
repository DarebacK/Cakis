#pragma once

#include <DarMath.hpp>

struct Mouse
{
  struct Button
  {
    bool pressedDown;
    bool isDown;
    bool pressedUp;
  };
  Button left;
  Button middle;
  Button right;
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

  static constexpr Vec3i gridSize = {6, 5, 4};
};



#include "Game.hpp"

using Tetracube = Vec3f[4];
static constexpr Tetracube tetraCubes[] = {
  {{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, {2.f, 0.f, 0.f}, {3.f, 0.f, 0.f}}, // I
  {{1.f, 0.f, 0.f}, {2.f, 0.f, 0.f}, {1.f, 0.f, 1.f}, {2.f, 0.f, 1.f}}, // O
  {{1.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, {1.f, 0.f, 1.f}, {2.f, 0.f, 1.f}}, // T
  {{0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, {1.f, 0.f, 1.f}, {2.f, 0.f, 1.f}}, // L
  {{2.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, {1.f, 0.f, 1.f}, {2.f, 0.f, 1.f}}, // J
  {{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, {1.f, 0.f, 1.f}, {2.f, 0.f, 1.f}}, // S
  {{1.f, 0.f, 0.f}, {2.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, {1.f, 0.f, 1.f}}, // Z
  {{1.f, 0.f, 0.f}, {1.f, 0.f, 1.f}, {2.f, 0.f, 1.f}, {1.f, 1.f, 1.f}}, // B
  {{1.f, 0.f, 0.f}, {1.f, 0.f, 1.f}, {2.f, 0.f, 1.f}, {1.f, 1.f, 0.f}}, // D
  {{1.f, 0.f, 0.f}, {1.f, 0.f, 1.f}, {2.f, 0.f, 1.f}, {2.f, 1.f, 1.f}}  // F
};
CubeClass cubeClasses[] = {
  {ColorRgbaf{  0.f,   1.f,   1.f, 1.f}},
  {ColorRgbaf{  1.f,   1.f,   0.f, 1.f}},
  {ColorRgbaf{  1.f,   0.f,   1.f, 1.f}},
  {ColorRgbaf{  1.f,  0.5f,   0.f, 1.f}},
  {ColorRgbaf{  0.f,   0.f,   1.f, 1.f}},
  {ColorRgbaf{  0.f,   1.f,   0.f, 1.f}},
  {ColorRgbaf{  1.f,   0.f,   0.f, 1.f}},
  {ColorRgbaf{ 0.5f, 0.19f,   0.f, 1.f}},
  {ColorRgbaf{0.25f, 0.25f, 0.25f, 1.f}},
  {ColorRgbaf{0.77f, 0.77f, 0.77f, 1.f}}
};

static void updateCamera(const GameState& lastState, GameState* nextState)
{
  nextState->camera = lastState.camera;

  if(lastState.input.mouse.right.isDown && nextState->input.mouse.right.isDown) {
    int dX = nextState->input.cursorPosition.x - lastState.input.cursorPosition.x;
    if(dX != 0) {
      float normalizedDx = float(dX) / nextState->clientAreaWidth;
      float dTheta = normalizedDx * 2 * Pi;
      nextState->camera.rotateTheta(dTheta);
    }

    int dY = nextState->input.cursorPosition.y - lastState.input.cursorPosition.y;
    if(dY != 0) {
      float normalizedDy = float(dY) / nextState->clientAreaHeight;
      float dPhi = -normalizedDy * 2 * Pi;
      nextState->camera.rotatePhi(dPhi);
    }
  }

  nextState->camera.zoom(nextState->input.mouse.dWheel);
}

static void updatePlayingSpace(const GameState& lastState, GameState* nextState)
{
  nextState->playingSpace = lastState.playingSpace;

  constexpr int xMax = GameState::gridSize.x - 1;
  constexpr int yMax = GameState::gridSize.y - 1;
  constexpr int zMax = GameState::gridSize.z - 1;
  nextState->playingSpace.at(0, 0, 0) = 0;
  nextState->playingSpace.at(xMax, 0, 0) = 1;
  nextState->playingSpace.at(0, yMax, 0) = 2;
  nextState->playingSpace.at(0, 0, zMax) = 3;
  nextState->playingSpace.at(xMax, yMax, 0) = 4;
  nextState->playingSpace.at(xMax, 0, zMax) = 5;
  nextState->playingSpace.at(0, yMax, zMax) = 6;
  nextState->playingSpace.at(xMax, yMax, zMax) = 7;
}

void Game::update(const GameState& lastState, GameState* nextState)
{
  updateCamera(lastState, nextState);
  updatePlayingSpace(lastState, nextState);
  nextState->cubeClasses = cubeClasses;
  nextState->cubeClassCount = arrayCount(cubeClasses);
}
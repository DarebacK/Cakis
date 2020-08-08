#include "Game.hpp"

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
  nextState->playingSpace.at(xMax, 0, 0) = 0;
  nextState->playingSpace.at(0, yMax, 0) = 0;
  nextState->playingSpace.at(0, 0, zMax) = 0;
  nextState->playingSpace.at(xMax, yMax, 0) = 0;
  nextState->playingSpace.at(xMax, 0, zMax) = 0;
  nextState->playingSpace.at(0, yMax, zMax) = 0;
  nextState->playingSpace.at(xMax, yMax, zMax) = 0;
}

void Game::update(const GameState& lastState, GameState* nextState)
{
  updateCamera(lastState, nextState);
  updatePlayingSpace(lastState, nextState);
}
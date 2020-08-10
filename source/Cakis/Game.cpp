#define DAR_MODULE_NAME "Game"

#include "Game.hpp"

#include <cstdlib>
#include <ctime>

static constexpr Vec3i tetraCubePositions[][4] = {
  {{0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0}}, // I
  {{1, 0, 0}, {2, 0, 0}, {1, 0, 1}, {2, 0, 1}}, // O
  {{1, 0, 0}, {0, 0, 1}, {1, 0, 1}, {2, 0, 1}}, // T
  {{0, 0, 0}, {0, 0, 1}, {1, 0, 1}, {2, 0, 1}}, // L
  {{2, 0, 0}, {0, 0, 1}, {1, 0, 1}, {2, 0, 1}}, // J
  {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {2, 0, 1}}, // S
  {{1, 0, 0}, {2, 0, 0}, {0, 0, 1}, {1, 0, 1}}, // Z
  {{1, 0, 0}, {1, 0, 1}, {2, 0, 1}, {1, 1, 1}}, // B
  {{1, 0, 0}, {1, 0, 1}, {2, 0, 1}, {1, 1, 0}}, // D
  {{1, 0, 0}, {1, 0, 1}, {2, 0, 1}, {2, 1, 1}}  // F
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

Game::Game()
{
  std::srand((unsigned int)std::time(0));
}

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
}
static void updateCurrentTetracube(const GameState& lastState, GameState* nextState)
{
  nextState->currentTetracubeDTimeLeftover = lastState.currentTetracubeDTimeLeftover + nextState->dTime;;

  bool collisionHappened;
  do {
    collisionHappened = false;
    const bool shouldSpawnTetracube = nextState->events.count(Event::TetracubeDropped) != 0 ||
      lastState.events.count(Event::GameStarted) != 0;
    if(shouldSpawnTetracube) {
      int tetracubeIndex = std::rand() % arrayCount(cubeClasses);

      Tetracube* currentTetracube = &nextState->currentTetracube;

      std::copy(
        tetraCubePositions[tetracubeIndex], tetraCubePositions[tetracubeIndex] + 4,
        currentTetracube->positions
      );
      Vec3i translationToCenter = {
        (int)std::floor(GameState::gridSize.x / 2.f) - 2,
        GameState::gridSize.y + 1,
        (int)std::floor(GameState::gridSize.z / 2.f) - 1
      };
      for(Vec3i& position : currentTetracube->positions) {
        position += translationToCenter;
      }

      currentTetracube->cubeClassIndex = (PlayingSpace::ValueType)tetracubeIndex;
    }
    else {
      nextState->currentTetracube = lastState.currentTetracube;
      nextState->currentTetracubeFallingSpeed = lastState.currentTetracubeFallingSpeed;

      Tetracube* currentTetracube = &nextState->currentTetracube;
      int currentTetracubeXMin = GameState::gridSize.x - 1;
      int currentTetracubeXMax = 0;
      int currentTetracubeZMin = GameState::gridSize.z - 1;
      int currentTetracubeZMax = 0;
      for(const Vec3i& position : currentTetracube->positions) {
        currentTetracubeXMin = std::min(currentTetracubeXMin, position.x);
        currentTetracubeXMax = std::max(currentTetracubeXMax, position.x);
        currentTetracubeZMin = std::min(currentTetracubeZMin, position.z);
        currentTetracubeZMax = std::max(currentTetracubeZMax, position.z);
      }
      if(nextState->input.keyboard.left.pressedDown && currentTetracubeXMin > 0) {
        bool isBlockedByAnotherCube = false;
        for(const Vec3i& position : currentTetracube->positions) {
          if(nextState->playingSpace.isInside(position.x - 1, position.y, position.z) &&
            nextState->playingSpace.at(position.x - 1, position.y, position.z) >= 0) {
            isBlockedByAnotherCube = true;
            break;
          }
        }
        if(!isBlockedByAnotherCube) {
          for(Vec3i& position : currentTetracube->positions) {
            --position.x;
          }
        }
      }
      if(nextState->input.keyboard.right.pressedDown && currentTetracubeXMax < GameState::gridSize.x - 1) {
        bool isBlockedByAnotherCube = false;
        for(const Vec3i& position : currentTetracube->positions) {
          if(nextState->playingSpace.isInside(position.x + 1, position.y, position.z) &&
            nextState->playingSpace.at(position.x + 1, position.y, position.z) >= 0) {
            isBlockedByAnotherCube = true;
            break;
          }
        }
        if(!isBlockedByAnotherCube) {
          for(Vec3i& position : currentTetracube->positions) {
            ++position.x;
          }
        }
      }
      if(nextState->input.keyboard.down.pressedDown && currentTetracubeZMin > 0) {
        bool isBlockedByAnotherCube = false;
        for(const Vec3i& position : currentTetracube->positions) {
          if(nextState->playingSpace.isInside(position.x, position.y, position.z - 1) &&
            nextState->playingSpace.at(position.x, position.y, position.z - 1) >= 0) {
            isBlockedByAnotherCube = true;
            break;
          }
        }
        if(!isBlockedByAnotherCube) {
          for(Vec3i& position : currentTetracube->positions) {
            --position.z;
          }
        }
      }
      if(nextState->input.keyboard.up.pressedDown && currentTetracubeZMax < GameState::gridSize.z - 1) {
        bool isBlockedByAnotherCube = false;
        for(const Vec3i& position : currentTetracube->positions) {
          if(nextState->playingSpace.isInside(position.x, position.y, position.z + 1) &&
            nextState->playingSpace.at(position.x, position.y, position.z + 1) >= 0) {
            isBlockedByAnotherCube = true;
            break;
          }
        }
        if(!isBlockedByAnotherCube) {
          for(Vec3i& position : currentTetracube->positions) {
            ++position.z;
          }
        }
      }
    }

    Tetracube* currentTetracube = &nextState->currentTetracube;

    assert(nextState->currentTetracubeFallingSpeed != 0.f);
    float fallingSpeedInverse = 1.f / nextState->currentTetracubeFallingSpeed;
    int toMove = nextState->currentTetracubeDTimeLeftover / fallingSpeedInverse;
    if(toMove > 0) {
      int moveBy;
      for(moveBy = 1; moveBy <= toMove; ++moveBy) {
        for(const Vec3i& position : currentTetracube->positions) {
          int newY = position.y - moveBy;
          if(nextState->playingSpace.isInside(position.x, newY, position.z) &&
            nextState->playingSpace.at(position.x, newY, position.z) >= 0 ||
            newY <= -1) {
            collisionHappened = true;
            nextState->events.emplace(Event::TetracubeDropped, Event());
            goto collisionCheckEnd;
          }
        }
      }
      collisionCheckEnd:
      --moveBy;
      for(Vec3i& position : currentTetracube->positions) {
        position.y -= moveBy;
        if(collisionHappened) {
          if(nextState->playingSpace.isInside(position)) {
            nextState->playingSpace.at(position) = currentTetracube->cubeClassIndex;
          } else {
            nextState->events.emplace(Event::GameLost, Event());
            logInfo("Lose condition triggered.");
            return;
          }
        }
      }
      nextState->currentTetracubeDTimeLeftover -= moveBy * fallingSpeedInverse;
    }
  } while(collisionHappened);
}
static void updateCubeClasses(const GameState& lastState, GameState* nextState)
{
  nextState->cubeClasses = cubeClasses;
  nextState->cubeClassCount = arrayCount(cubeClasses);
}

void Game::update(const GameState& lastState, GameState* nextState)
{
  updateCamera(lastState, nextState);
  updatePlayingSpace(lastState, nextState);
  updateCurrentTetracube(lastState, nextState);
  updateCubeClasses(lastState, nextState);
}
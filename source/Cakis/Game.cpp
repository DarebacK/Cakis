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
static void spawnTetracube(Tetracube* tetracube)
{
  int tetracubeIndex = std::rand() % arrayCount(cubeClasses);
  std::copy(
    tetraCubePositions[tetracubeIndex], tetraCubePositions[tetracubeIndex] + 4,
    tetracube->positions
  );
  Vec3i translationToCenter = {
    (int)std::floor(GameState::gridSize.x / 2.f) - 2,
    GameState::gridSize.y + 1,
    (int)std::floor(GameState::gridSize.z / 2.f) - 1
  };
  for(Vec3i& position : tetracube->positions) {
    position += translationToCenter;
  }

  tetracube->cubeClassIndex = (PlayingSpace::ValueType)tetracubeIndex;
}
static void moveTetracube(Tetracube* tetracube, const Vec3i& moveBy, const PlayingSpace& playingSpace)
{
  bool isBlockedByAnotherCube = false;
  for(const Vec3i& position : tetracube->positions) {
    Vec3i movedBy = position + moveBy;
    if(playingSpace.isInside(movedBy) &&
      playingSpace.at(movedBy) >= 0) {
      isBlockedByAnotherCube = true;
      break;
    }
  }
  if(!isBlockedByAnotherCube) {
    for(Vec3i& position : tetracube->positions) {
      position += moveBy;
    }
  }
}
static void updateCurrentTetracube(const GameState& lastState, GameState* nextState)
{
  nextState->currentTetracube = lastState.currentTetracube;

  if(nextState->phase == GameState::Phase::Playing) {
    nextState->currentTetracubeFallingSpeed = lastState.currentTetracubeFallingSpeed;
    nextState->currentTetracubeDTimeLeftover = lastState.currentTetracubeDTimeLeftover + nextState->dTime;;

    bool collisionHappened;
    do {
      collisionHappened = false;
      const bool shouldSpawnTetracube = nextState->events.count(Event::TetracubeDropped) != 0 ||
        lastState.events.count(Event::GameStarted) != 0;
      if(shouldSpawnTetracube) {
        spawnTetracube(&nextState->currentTetracube);
      }
      else {
        Tetracube* currentTetracube = &nextState->currentTetracube;
        int currentTetracubeXMin = GameState::gridSize.x - 1;
        int currentTetracubeXMax = 0;
        int currentTetracubeZMin = GameState::gridSize.z - 1;
        int currentTetracubeZMax = 0;
        int currentTetracubeYMin = GameState::gridSize.y - 1;
        for(const Vec3i& position : currentTetracube->positions) {
          currentTetracubeXMin = std::min(currentTetracubeXMin, position.x);
          currentTetracubeXMax = std::max(currentTetracubeXMax, position.x);
          currentTetracubeZMin = std::min(currentTetracubeZMin, position.z);
          currentTetracubeZMax = std::max(currentTetracubeZMax, position.z);
          currentTetracubeYMin = std::min(currentTetracubeYMin, position.y);
        }
        if(nextState->input.keyboard.left.pressedDown && currentTetracubeXMin > 0) {
          moveTetracube(currentTetracube, {-1, 0, 0}, nextState->playingSpace);
        } else if(nextState->input.keyboard.right.pressedDown && currentTetracubeXMax < GameState::gridSize.x - 1) {
          moveTetracube(currentTetracube, {1, 0, 0}, nextState->playingSpace);
        } else if(nextState->input.keyboard.down.pressedDown && currentTetracubeZMin > 0) {
          moveTetracube(currentTetracube, {0, 0, -1}, nextState->playingSpace);
        } else if(nextState->input.keyboard.up.pressedDown && currentTetracubeZMax < GameState::gridSize.z - 1) {
          moveTetracube(currentTetracube, {0, 0, 1}, nextState->playingSpace);
        }
      }

      Tetracube* currentTetracube = &nextState->currentTetracube;

      assert(nextState->currentTetracubeFallingSpeed != 0.f);
      float fallingSpeedInverse = 1.f / nextState->currentTetracubeFallingSpeed;
      int toMove = int(nextState->currentTetracubeDTimeLeftover / fallingSpeedInverse);
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
            }
            else {
              nextState->events.emplace(Event::GameLost, Event());
              logInfo("Lose condition triggered.");
              return;
            }
          }
        }
        nextState->currentTetracubeDTimeLeftover -= moveBy * fallingSpeedInverse;
        nextState->currentTetracubeDTimeLeftover = std::max(nextState->currentTetracubeDTimeLeftover, 0.f);
      }
    } while(collisionHappened);

    if(nextState->input.keyboard.space.pressedDown) {
      Tetracube* currentTetracube = &nextState->currentTetracube;
      int toMove = 0;
      while(true) {
        for(const Vec3i& position : currentTetracube->positions) {
          int nextY = position.y - toMove - 1;
          bool isBlockedByACube = (nextState->playingSpace.isInside(position.x, nextY, position.z) && nextState->playingSpace.at(position.x, nextY, position.z) >= 0);
          if(nextY < 0 || isBlockedByACube) {
            goto dropDistanceCalculationEnd;
          }
        }
        ++toMove;
      }
      dropDistanceCalculationEnd:

      bool loseConditionTriggered = false;
      for(Vec3i& position : currentTetracube->positions) {
        if(!nextState->playingSpace.isInside(position.x, position.y - toMove, position.z)) {
          loseConditionTriggered = true;
        }
        position.y -= toMove;
      }

      nextState->events.emplace(Event::TetracubeDropped, Event());
      if(loseConditionTriggered) {
        logInfo("Lose condition triggered.");
        nextState->events.emplace(Event::GameLost, Event());
        return;
      } else {
        for(const Vec3i& position : currentTetracube->positions) {
          nextState->playingSpace.at(position.x, position.y, position.z) = currentTetracube->cubeClassIndex;
        }
        spawnTetracube(currentTetracube);
      }
    }
  }
}
static void updateCubeClasses(const GameState& lastState, GameState* nextState)
{
  nextState->cubeClasses = cubeClasses;
  nextState->cubeClassCount = arrayCount(cubeClasses);
}
static void updateGamePhase(const GameState& lastState, GameState* nextState)
{
  if(lastState.events.count(Event::GameLost) > 0) {
    nextState->phase = GameState::Phase::GameLost;
  } else {
    nextState->phase = lastState.phase;
  }
}

void Game::update(const GameState& lastState, GameState* nextState)
{
  updateGamePhase(lastState, nextState);
  updateCamera(lastState, nextState);
  updatePlayingSpace(lastState, nextState);
  updateCurrentTetracube(lastState, nextState);
  updateCubeClasses(lastState, nextState);
}
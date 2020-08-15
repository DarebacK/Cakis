#define DAR_MODULE_NAME "Game"

#include "Game.hpp"

#include <cstdlib>
#include <ctime>

static constexpr Vec3i tetracubePositions[][4] = {
  {{-1, 0, 0}, { 0, 0, 0}, { 1, 0, 0}, {2, 0, 0}}, // I
  {{ 0, 0, 0}, { 1, 0, 0}, { 0, 0, 1}, {1, 0, 1}}, // O
  {{ 0, 0, 0}, {-1, 0, 1}, { 0, 0, 1}, {1, 0, 1}}, // T
  {{-1, 0, 0}, {-1, 0, 1}, { 0, 0, 1}, {1, 0, 1}}, // L
  {{ 1, 0, 0}, {-1, 0, 1}, { 0, 0, 1}, {1, 0, 1}}, // J
  {{-1, 0, 0}, { 0, 0, 0}, { 0, 0, 1}, {1, 0, 1}}, // S
  {{ 0, 0, 0}, { 1, 0, 0}, {-1, 0, 1}, {0, 0, 1}}, // Z
  {{ 0, 0, 0}, { 0, 0, 1}, { 1, 0, 1}, {0, 1, 1}}, // B
  {{ 0, 0, 0}, { 0, 0, 1}, { 1, 0, 1}, {0, 1, 0}}, // D
  {{ 0, 0, 0}, { 0, 0, 1}, { 1, 0, 1}, {1, 1, 1}}  // F
};
static constexpr Vec3i tetracubeOrigin = {1, 0, 0};
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
    tetracubePositions[tetracubeIndex], tetracubePositions[tetracubeIndex] + 4,
    tetracube->positions
  );
  Vec3i translationToCenter = {
    (int)std::floor(GameState::gridSize.x / 2.f) - 2,
    GameState::gridSize.y + 1,
    (int)std::floor(GameState::gridSize.z / 2.f) - 1
  };
  tetracube->translation = tetracubeOrigin + translationToCenter;

  tetracube->cubeClassIndex = (PlayingSpace::ValueType)tetracubeIndex;
}
static void tryToMoveTetracube(Tetracube* tetracube, const Vec3i& moveBy, const PlayingSpace& playingSpace)
{
  bool isBlockedByAnotherCube = false;
  for(const Vec3i& position : tetracube->positions) {
    Vec3i movedBy = position + tetracube->translation + moveBy;
    if(playingSpace.isInside(movedBy) &&
      playingSpace.at(movedBy) >= 0) {
      isBlockedByAnotherCube = true;
      break;
    }
  }
  if(!isBlockedByAnotherCube) {
    tetracube->translation += moveBy;
  }
}
static void tryToRotateTetracube(Tetracube* tetracube, const Mat4f& rotation, const PlayingSpace& playingSpace)
{
  bool canRotate = true;
  Vec3i rotatedPositions[4];
  for(int i = 0; i < 4; ++i) {
    rotatedPositions[i] = toVec3iRounded(toVec4f(tetracube->positions[i], 1.f) * rotation);
    const Vec3i translatedRotatedPosition = rotatedPositions[i] + tetracube->translation;
    const bool isInside = playingSpace.isInside(translatedRotatedPosition);
    if(isInside && playingSpace.at(translatedRotatedPosition) >= 0) {
      canRotate = false;
      break;
    } else if(!playingSpace.isInside(
      translatedRotatedPosition.x, 
      std::min(0, translatedRotatedPosition.y)/*Ignore y above playing space*/, 
      translatedRotatedPosition.z)) {
      canRotate = false;
      break;
    }
  }
  if(canRotate) {
    std::copy(rotatedPositions, rotatedPositions + arrayCount(rotatedPositions), tetracube->positions);
  }
}
static bool canClearRow(const PlayingSpace& playingSpace, int rowToClear)
{
  for(int x = 0; x < GameState::gridSize.x; ++x) {
    for(int z = 0; z < GameState::gridSize.z; ++z) {
      if(playingSpace.at(x, rowToClear, z) == PlayingSpace::emptyValue) {
        return false;
      }
    }
  }
  return true;
}
static void clearRow(PlayingSpace* playingSpace, int rowToClear) 
{
  for(int row = rowToClear; row < GameState::gridSize.y - 1; ++row) {
    for(int x = 0; x < GameState::gridSize.x; ++x) {
      for(int z = 0; z < GameState::gridSize.z; ++z) {
        playingSpace->at(x, row, z) = playingSpace->at(x, row + 1, z);
      }
    }
  }
  for(int x = 0; x < GameState::gridSize.x; ++x) {
    for(int z = 0; z < GameState::gridSize.z; ++z) {
      playingSpace->at(x, GameState::gridSize.y - 1, z) = PlayingSpace::emptyValue;
    }
  }
}
static void checkForRowClear(GameState* state, int* rowsToCheck, int rowsToCheckCount)
{
  std::sort(rowsToCheck, rowsToCheck + rowsToCheckCount);
  int rowsCleared = 0;
  for(int i = 0; i < rowsToCheckCount; ++i) {
    const int rowToClear = rowsToCheck[i] - rowsCleared;
    if(canClearRow(state->playingSpace, rowToClear)) {
      clearRow(&state->playingSpace, rowToClear);
      ++rowsCleared;
    }
  }
}
static void checkForRowClear(GameState* state, const Tetracube& droppedTetracube)
{
  int rowsToCheck[arrayCount(droppedTetracube.positions)];
  int rowsToCheckCount = 0;
  for(const Vec3i& position : droppedTetracube.positions) {
    int* rowsToCheckEnd = rowsToCheck + rowsToCheckCount;
    const int row = position.y + droppedTetracube.translation.y;
    const bool alreadyContaisRow = std::find(rowsToCheck, rowsToCheckEnd, row) != rowsToCheckEnd;
    if(!alreadyContaisRow) {
      rowsToCheck[rowsToCheckCount++] = row;
    }
  }
  checkForRowClear(state, rowsToCheck, rowsToCheckCount);
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
          Vec3i translatedPosition = position + currentTetracube->translation;
          currentTetracubeXMin = std::min(currentTetracubeXMin, translatedPosition.x);
          currentTetracubeXMax = std::max(currentTetracubeXMax, translatedPosition.x);
          currentTetracubeZMin = std::min(currentTetracubeZMin, translatedPosition.z);
          currentTetracubeZMax = std::max(currentTetracubeZMax, translatedPosition.z);
          currentTetracubeYMin = std::min(currentTetracubeYMin, translatedPosition.y);
        }
        if(nextState->input.keyboard.left.pressedDown && currentTetracubeXMin > 0) {
          tryToMoveTetracube(currentTetracube, {-1, 0, 0}, nextState->playingSpace);
        } else if(nextState->input.keyboard.right.pressedDown && currentTetracubeXMax < GameState::gridSize.x - 1) {
          tryToMoveTetracube(currentTetracube, {1, 0, 0}, nextState->playingSpace);
        } else if(nextState->input.keyboard.down.pressedDown && currentTetracubeZMin > 0) {
          tryToMoveTetracube(currentTetracube, {0, 0, -1}, nextState->playingSpace);
        } else if(nextState->input.keyboard.up.pressedDown && currentTetracubeZMax < GameState::gridSize.z - 1) {
          tryToMoveTetracube(currentTetracube, {0, 0, 1}, nextState->playingSpace);
        }
        if(nextState->input.keyboard.q.pressedDown) {
          tryToRotateTetracube(currentTetracube, Mat4f::rotationZ(Pi / 2.f), nextState->playingSpace);
        } else if(nextState->input.keyboard.w.pressedDown) {
          tryToRotateTetracube(currentTetracube, Mat4f::rotationX(Pi / 2.f), nextState->playingSpace);
        } else if(nextState->input.keyboard.e.pressedDown) {
          tryToRotateTetracube(currentTetracube, Mat4f::rotationZ(-Pi / 2.f), nextState->playingSpace);
        } else if(nextState->input.keyboard.a.pressedDown) {
          tryToRotateTetracube(currentTetracube, Mat4f::rotationY(Pi / 2.f), nextState->playingSpace);
        } else if(nextState->input.keyboard.s.pressedDown) {
          tryToRotateTetracube(currentTetracube, Mat4f::rotationX(-Pi / 2.f), nextState->playingSpace);
        } else if(nextState->input.keyboard.d.pressedDown) {
          tryToRotateTetracube(currentTetracube, Mat4f::rotationY(-Pi / 2.f), nextState->playingSpace);
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
            Vec3i newPosition = position + currentTetracube->translation;
            newPosition.y -= moveBy;
            if(nextState->playingSpace.isInside(newPosition) &&
              nextState->playingSpace.at(newPosition) >= 0 ||
              newPosition.y == PlayingSpace::emptyValue) {
              collisionHappened = true;
              nextState->events.emplace(Event::TetracubeDropped, Event());
              goto collisionCheckEnd;
            }
          }
        }
        collisionCheckEnd:
        --moveBy;
        currentTetracube->translation.y -= moveBy;
        for(Vec3i& position : currentTetracube->positions) {
          Vec3i translatedPosition = position + currentTetracube->translation;
          if(collisionHappened) {
            if(nextState->playingSpace.isInside(translatedPosition)) {
              nextState->playingSpace.at(translatedPosition) = currentTetracube->cubeClassIndex;
            } else {
              nextState->events.emplace(Event::GameLost, Event());
              logInfo("Lose condition triggered.");
              return;
            }
          }
        }
        if(collisionHappened) {
          checkForRowClear(nextState, *currentTetracube);
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
          Vec3i nextPosition = position + currentTetracube->translation;
          nextPosition.y = nextPosition.y - toMove - 1;
          bool isBlockedByACube = (nextState->playingSpace.isInside(nextPosition) && nextState->playingSpace.at(nextPosition) >= 0);
          if(nextPosition.y < 0 || isBlockedByACube) {
            goto dropDistanceCalculationEnd;
          }
        }
        ++toMove;
      }
      dropDistanceCalculationEnd:
      currentTetracube->translation.y -= toMove;

      bool loseConditionTriggered = false;
      for(const Vec3i& position : currentTetracube->positions) {
        Vec3i translatedPosition = position + currentTetracube->translation;
        if(!nextState->playingSpace.isInside(translatedPosition)) {
          loseConditionTriggered = true;
        }
      }

      nextState->events.emplace(Event::TetracubeDropped, Event());
      if(loseConditionTriggered) {
        logInfo("Lose condition triggered.");
        nextState->events.emplace(Event::GameLost, Event());
        return;
      } else {
        for(const Vec3i& position : currentTetracube->positions) {
          Vec3i translatedPosition = position + currentTetracube->translation;
          nextState->playingSpace.at(translatedPosition) = currentTetracube->cubeClassIndex;
        }
        checkForRowClear(nextState, *currentTetracube);
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
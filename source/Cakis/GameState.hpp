#pragma once

#include <unordered_map>

#include <DarMath.hpp>
#include <Color.hpp>

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
  Key left;
  Key right;
  Key down;
  Key up;
  Key F1;
  Key rightAlt;
  Key space;
  Key q, w, e;
  Key a, s, d;
};

struct Input
{
  Mouse mouse;
  Vec2i cursorPosition;
  Keyboard keyboard;
};

struct Event
{
  enum Type {
    Invalid = 0,
    GameStarted,
    TetracubeDropped,
    GameLost
  };
};

struct CubeClass
{
  ColorRgbaf color;
};

class PlayingSpace
{
public:
  using ValueType = int8_t;
  static constexpr ValueType emptyValue = -1;

  explicit PlayingSpace(const Vec3i& size)
    : size(size)
    , count(calculateCount(size))
    , values(new ValueType[count])
  {
    std::fill_n(values, count, emptyValue);
  }
  ~PlayingSpace() { delete[] values; }
  PlayingSpace(const PlayingSpace& other)
    : PlayingSpace(other.size)
  {
    std::copy(other.begin(), other.end(), begin());
  };
  PlayingSpace(PlayingSpace&& other) noexcept
    : size(other.size)
    , count(other.count)
    , values(other.values)
  {
    other.values = nullptr;
  };
  PlayingSpace& operator=(const PlayingSpace& rhs)
  {
    size = rhs.size;
    if(count != rhs.count) {
      delete[] values;
      values = new ValueType[rhs.count];
    }
    count = rhs.count;
    std::copy(rhs.begin(), rhs.end(), begin());
    return *this;
  }
  PlayingSpace& operator=(PlayingSpace&& rhs) noexcept
  {
    size = rhs.size;
    count = rhs.count;
    values = rhs.values;
    rhs.values = nullptr;
    return *this;
  }

  bool isInside(int x, int y, int z) const noexcept 
  {
    return x >= 0 && x < size.x &&
      y >= 0 && y < size.y &&
      z >= 0 && z < size.z;
  };
  bool isInside(const Vec3i& position) const noexcept { return isInside(position.x, position.y, position.z); };
  ValueType at(int x, int y, int z) const noexcept 
  { 
    assert(isInside(x, y, z));
    return *(begin() + x + z*size.x + y*size.x*size.z); 
  }
  ValueType at(const Vec3i& position) const noexcept { return at(position.x, position.y, position.z); }
  ValueType& at(int x, int y, int z) noexcept 
  { 
    assert(isInside(x, y, z));
    return *(begin() + x + z*size.x + y*size.x*size.z);; 
  }
  ValueType& at(const Vec3i& position) noexcept { return at(position.x, position.y, position.z); }
  ValueType* begin() noexcept { return values; }
  const ValueType* begin() const noexcept { return values; }
  ValueType* end() noexcept { return values + count; }
  const ValueType* end() const noexcept { return values + count; }

  const Vec3i& getSize() const noexcept { return size; }
  const int getCount() const noexcept { return count; }

private:
  static int calculateCount(const Vec3i& size) noexcept { return size.x * size.y * size.z; }

  Vec3i size;
  int count;
  ValueType* values;
};

struct Tetracube
{
  Vec3i positions[4];
  Vec3i translation;
  PlayingSpace::ValueType cubeClassIndex;
};

struct GameState
{
  Input input;

  std::unordered_multimap<Event::Type, Event> events;

  enum class Phase
  {
    Invalid = 0,
    Playing,
    GameLost
  } phase;

  float dTime;

  TrackSphere camera = TrackSphere(0.f, Pi / 8.f, 8.f, 2.f, 10.f);

  int clientAreaWidth;
  int clientAreaHeight;

  static constexpr Vec3i gridSize = {6, 5, 4};
  PlayingSpace playingSpace{ gridSize };
  const CubeClass* cubeClasses;
  int cubeClassCount;

  Tetracube currentTetracube;
  float currentTetracubeFallingSpeed = 0.5f;
  float currentTetracubeDTimeLeftover = 0.f;
};



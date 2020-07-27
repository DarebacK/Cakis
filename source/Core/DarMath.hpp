#pragma once

/**
 * @brief Left handed coordinate system, row major matrices, row vectors, pre-multiplication.
 */

#include <algorithm>
#include <cmath>

#include "DarEngine.hpp"

constexpr float Pi = 3.14159265358979323846f;

struct Vec2f
{
  float x, y;
};
constexpr inline Vec2f operator-(const Vec2f& v) noexcept { return { -v.x, -v.y }; }
constexpr inline Vec2f operator-(const Vec2f& left, const Vec2f& right) noexcept 
{
  return Vec2f{ left.x - right.x, left.y - right.y };
}
constexpr inline bool operator==(const Vec2f& left, const Vec2f& right) noexcept 
{
  return left.x == right.x && left.y == right.y;
}
constexpr inline bool operator!=(const Vec2f& left, const Vec2f& right) noexcept { return !(left == right); }
struct Vec3f
{
  float x, y, z;
};
constexpr inline Vec3f operator-(const Vec3f& v) noexcept { return { -v.x, -v.y, -v.z }; }
constexpr inline Vec3f operator-(const Vec3f& left, const Vec3f& right) noexcept
{
  return Vec3f{ left.x - right.x, left.y - right.y, left.z - right.z };
}
constexpr inline bool operator==(const Vec3f& left, const Vec3f& right) noexcept
{
  return left.x == right.x && left.y == right.y && left.z == right.z;
}
constexpr inline bool operator!=(const Vec3f& left, const Vec3f& right) noexcept { return !(left == right); }
struct Vec4f
{
  float x, y, z, w;
};
constexpr inline Vec4f operator-(const Vec4f& v) noexcept { return { -v.x, -v.y, -v.z, -v.w }; }
constexpr inline Vec4f operator-(const Vec4f& left, const Vec4f& right) noexcept
{
  return Vec4f{ left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
}
constexpr inline bool operator==(const Vec4f& left, const Vec4f& right) noexcept
{
  return left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w;
}
constexpr inline bool operator!=(const Vec4f& left, const Vec4f& right) noexcept { return !(left == right); }

constexpr inline float dot(const Vec2f& v1, const Vec2f& v2) noexcept
{
  return (v1.x * v2.x) + (v1.y * v2.y);
}
constexpr inline float dot(const Vec3f& v1, const Vec3f& v2) noexcept
{
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
constexpr inline float dot(const Vec4f& v1, const Vec4f& v2) noexcept
{
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

constexpr inline Vec3f cross(const Vec3f& v1, const Vec3f& v2) noexcept
{
  return {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)};
}

using std::sqrt;

inline auto length(const Vec2f& v) noexcept
{
  return sqrt(dot(v, v));
}
inline auto length(const Vec3f& v) noexcept
{
  return sqrt(dot(v, v));
}
inline auto length(const Vec4f& v) noexcept
{
  return sqrt(dot(v, v));
}

inline Vec2f normalized(const Vec2f& v) noexcept
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length};
}
inline Vec3f normalized(const Vec3f& v) noexcept
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length, v.z / length};
}
inline Vec4f normalized(const Vec4f& v) noexcept
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length, v.z / length, v.w / length};
}

inline bool isNormalized(const Vec2f& v) noexcept { return abs(length(v) - 1.f) <= FLT_EPSILON; }
inline bool isNormalized(const Vec3f& v) noexcept { return abs(length(v) - 1.f) <= FLT_EPSILON; }
inline bool isNormalized(const Vec4f& v) noexcept { return abs(length(v) - 1.f) <= FLT_EPSILON; }

using std::clamp;

constexpr inline float degreesToRadians(float degrees) noexcept
{
  return degrees * Pi / 180.f;
}

struct Mat4f
{
  static Mat4f identity() noexcept
  {
    return 
    {{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}
    }};
  }
  static Mat4f translation(float x, float y, float z) noexcept
  {
    return 
    {{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {   x,    y,    z, 1.0f}
    }};
  }
  static Mat4f translation(const Vec3f& by) noexcept
  {
    return translation(by.x, by.y, by.z);
  }
  static Mat4f perspectiveProjectionD3d(
    float verticalFieldOfViewRadians, 
    float aspectRatio, 
    float nearZ, 
    float farZ
  ) noexcept
  {
    // from https://github.com/microsoft/DirectXMath/blob/83634c742a85d1027765af53fbe79506fd72e0c3/Inc/DirectXMathMatrix.inl
    verticalFieldOfViewRadians *= 0.5f;
    float vFovSin = sin(verticalFieldOfViewRadians);
    float vFovCos = cos(verticalFieldOfViewRadians);
    float height = vFovCos / vFovSin;
    float width = height / aspectRatio;
    float fRange = farZ / (farZ - nearZ);
    return 
    {{
      {width,    0.f,             0.f, 0.f},
      {  0.f, height,             0.f, 0.f},
      {  0.f,    0.f,          fRange, 1.f},
      {  0.f,    0.f, -fRange * nearZ, 0.f}
    }};
  }
  static Mat4f lookAt(const Vec3f& eyePosition, const Vec3f& focusPosition, const Vec3f& upDirection) noexcept
  {
    assert(eyePosition != focusPosition);
    Vec3f eyeDirection = normalized(focusPosition - eyePosition);
    return lookTo(eyePosition, eyeDirection, upDirection);
  }
  /**
   * @param eyeDirection has to be normalized
   */
  static Mat4f lookTo(const Vec3f& eyePosition, const Vec3f& eyeDirection, const Vec3f& upDirection) noexcept
  {
    assert(eyeDirection != (Vec3f{ 0.f, 0.f, 0.f }));
    assert(upDirection != (Vec3f{ 0.f, 0.f, 0.f }));
    assert(isNormalized(eyeDirection));
    // from https://github.com/microsoft/DirectXMath/blob/83634c742a85d1027765af53fbe79506fd72e0c3/Inc/DirectXMathMatrix.inl
    Vec3f r0 = normalized(cross(upDirection, eyeDirection));
    Vec3f r1 = cross(eyeDirection, r0);
    Vec3f negatedEyePosition = -eyePosition;
    float d0 = dot(r0, negatedEyePosition);
    float d1 = dot(r1, negatedEyePosition);
    float d2 = dot(eyeDirection, negatedEyePosition);
    return
    {{
      {r0.x, r1.x, eyeDirection.x, 0.f},
      {r0.y, r1.y, eyeDirection.y, 0.f},
      {r0.z, r1.z, eyeDirection.z, 0.f},
      {d0,   d1,   d2,             1.f}
    }};
  }

  void translateBy(const Vec3f& translation) noexcept
  {
    values[3][0] += translation.x;
    values[3][1] += translation.y;
    values[3][2] += translation.z;
  }
  void transpose() noexcept
  {
    float tmp;
    for(int i = 1; i < 4; ++i) {
      for(int j = i; j < 4; ++j) {
        tmp = values[i - 1][j];
        values[i - 1][j] = values[j][i - 1];
        values[j][i - 1] = tmp;
      }
    }
  }

  float* operator[](int index) noexcept {return values[index];}
  const float* operator[](int index) const noexcept {return values[index];}

  float values[4][4];
};

Mat4f operator*(const Mat4f& left, const Mat4f& right) noexcept;
Vec4f operator*(const Vec4f& left, const Mat4f& right) noexcept;
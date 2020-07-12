#pragma once

/**
 * @brief Left handed coordinate system, row major matrices, row vectors, pre-multiplication.
 */

#include <cmath>
#include <algorithm>

constexpr float Pi = 3.14159265358979323846f;

struct Vec2f
{
  float x, y;
};
struct Vec3f
{
  float x, y, z;
};
struct Vec4f
{
  float x, y, z, w;
};

inline float dot(const Vec2f& v1, const Vec2f& v2)
{
  return (v1.x * v2.x) + (v1.y * v2.y);
}
inline float dot(const Vec3f& v1, const Vec3f& v2)
{
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
inline float dot(const Vec4f& v1, const Vec4f& v2)
{
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

inline Vec3f cross(const Vec3f& v1, const Vec3f& v2)
{
  return {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)};
}

using std::sqrt;

inline auto length(const Vec2f& v)
{
  return sqrt(dot(v, v));
}
inline auto length(const Vec3f& v)
{
  return sqrt(dot(v, v));
}
inline auto length(const Vec4f& v)
{
  return sqrt(dot(v, v));
}

inline Vec2f normalized(const Vec2f& v)
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length};
}
inline Vec3f normalized(const Vec3f& v)
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length, v.z / length};
}
inline Vec4f normalized(const Vec4f& v)
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length, v.z / length, v.w / length};
}

using std::clamp;

inline float degreesToRadians(float degrees)
{
  return degrees * Pi / 180.f;
}

struct Mat4f
{
  static Mat4f identity() 
  {
    return 
    {{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}
    }};
  }
  static Mat4f translation(float x, float y, float z) 
  {
    return 
    {{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {   x,    y,    z, 1.0f}
    }};
  }
  static Mat4f translation(const Vec3f& by) 
  {
    return translation(by.x, by.y, by.z);
  }
  static Mat4f perspectiveProjection(
    float verticalFieldOfViewRadians, 
    float aspectRatio, 
    float nearZ, 
    float farZ
  )
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

  void translateBy(const Vec3f& translation)
  {
    values[3][0] += translation.x;
    values[3][1] += translation.y;
    values[3][2] += translation.z;
  }
  void transpose()
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

  float* operator[](int index) {return values[index];};
  const float* operator[](int index) const {return values[index];};

  float values[4][4];
};

Mat4f operator*(const Mat4f& left, const Mat4f& right);
Vec4f operator*(const Vec4f& left, const Mat4f& right);
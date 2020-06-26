#pragma once

#include <cmath>
#include <algorithm>

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

float dot(const Vec2f& v1, const Vec2f& v2)
{
  return (v1.x * v2.x) + (v1.y * v2.y);
}
float dot(const Vec3f& v1, const Vec3f& v2)
{
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
float dot(const Vec4f& v1, const Vec4f& v2)
{
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

Vec3f cross(const Vec3f& v1, const Vec3f& v2)
{
  return {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)};
}

using std::sqrt;

auto length(const Vec2f& v)
{
  return sqrt(dot(v, v));
}
auto length(const Vec3f& v)
{
  return sqrt(dot(v, v));
}
auto length(const Vec4f& v)
{
  return sqrt(dot(v, v));
}

Vec2f normalized(const Vec2f& v)
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length};
}
Vec3f normalized(const Vec3f& v)
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length, v.z / length};
}
Vec4f normalized(const Vec4f& v)
{
  const auto length = ::length(v);
  return {v.x / length, v.y / length, v.z / length, v.w / length};
}

using std::clamp;

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
      {1.0f, 0.0f, 0.0f, x},
      {0.0f, 1.0f, 0.0f, y},
      {0.0f, 0.0f, 1.0f, z},
      {0.0f, 0.0f, 0.0f, 1.0f}
    }};
  }
  static Mat4f translation(const Vec3f& by) 
  {
    return translation(by.x, by.y, by.z);
  }

  float* operator[](int index) {return values[index];};
  const float* operator[](int index) const {return values[index];};

  void translate(const Vec3f& translation)
  {
    values[0][3] = translation.x;
    values[1][3] = translation.y;
    values[2][3] = translation.z;
  }

  float values[4][4];
};

Vec4f operator*(const Mat4f& left, const Vec4f& right)
{
  return Vec4f
  {
    left[0][0]*right.x + left[0][1]*right.y + left[0][2]*right.z + left[0][3]*right.w,
    left[1][0]*right.x + left[1][1]*right.y + left[1][2]*right.z + left[1][3]*right.w,
    left[2][0]*right.x + left[2][1]*right.y + left[2][2]*right.z + left[2][3]*right.w,
    left[3][0]*right.x + left[3][1]*right.y + left[3][2]*right.z + left[3][3]*right.w
  };
}
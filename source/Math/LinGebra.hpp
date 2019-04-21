#pragma once

#include <cmath>

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

//template<int columnCountT, int rowCountT, typename T>
//struct Mat
//{
//  static constexpr int columnCount = columnCountT;
//  static constexpr int rowCount = rowCountT;
//
//  T* operator[](int index) {return values[index];};
//  const T* operator[](int index) const {return values[index];};
//
//private:
//  T values[rowCount][columnCount];
//};
//template<typename T>
//using Mat3 = Mat<3, 3, T>;
//using Mat3f = Mat3<float>;
//template<typename T>
//using Mat4 = Mat<4, 4, T>;
//using Mat4f = Mat4<float>;

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
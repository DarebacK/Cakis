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
constexpr inline Vec2f operator+(const Vec2f& left, const Vec2f& right) noexcept
{
  return Vec2f{ left.x + right.x, left.y + right.y };
}
constexpr inline Vec2f operator*(float left, const Vec2f& right) noexcept
{
  return Vec2f{ left * right.x, left * right.y };
}
constexpr inline Vec2f operator*(const Vec2f& left, float right) noexcept
{
  return right * left;
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
constexpr inline Vec3f operator+(const Vec3f& left, const Vec3f& right) noexcept
{
  return Vec3f{ left.x + right.x, left.y + right.y, left.z + right.z };
}
constexpr inline Vec3f operator*(float left, const Vec3f& right) noexcept
{
  return Vec3f{ left * right.x, left * right.y, left * right.z };
}
constexpr inline Vec3f operator*(const Vec3f& left, float right) noexcept
{
  return right * left;
}
constexpr inline bool operator==(const Vec3f& left, const Vec3f& right) noexcept
{
  return left.x == right.x && left.y == right.y && left.z == right.z;
}
constexpr inline bool operator!=(const Vec3f& left, const Vec3f& right) noexcept { return !(left == right); }
constexpr inline Vec3f lerp(const Vec3f& v1, const Vec3f& v2, float t, float oneMinusT) noexcept
{
  return
  {
    oneMinusT*v1.x + t*v2.x,
    oneMinusT*v1.y + t*v2.y,
    oneMinusT*v1.z + t*v2.z,
  };
}
constexpr inline Vec3f lerp(const Vec3f& v1, const Vec3f& v2, float t) noexcept
{
  return lerp(v1, v2, t, 1.f - t);
}
struct Vec4f
{
  float x, y, z, w;
};
constexpr inline Vec4f operator-(const Vec4f& v) noexcept { return { -v.x, -v.y, -v.z, -v.w }; }
constexpr inline Vec4f operator-(const Vec4f& left, const Vec4f& right) noexcept
{
  return Vec4f{ left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
}
constexpr inline Vec4f operator+(const Vec4f& left, const Vec4f& right) noexcept
{
  return Vec4f{ left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
}
constexpr inline Vec4f operator*(float left, const Vec4f& right) noexcept
{
  return Vec4f{ left * right.x, left * right.y, left * right.z, left * right.w };
}
constexpr inline Vec4f operator*(const Vec4f& left, float right) noexcept
{
  return right * left;
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

struct Quaternionf
{
  Vec3f v;
  float s;

  // For conversion from a matrix, check Game Engine Architecture page 399

  operator Mat4f() const noexcept
  {
    float xx = v.x * v.x;
    float xy = v.x * v.y;
    float xz = v.x * v.z;
    float xs = v.x * s;
    float yy = v.y * v.y;
    float yz = v.y * v.z;
    float ys = v.y * s;
    float zz = v.z * v.z;
    float zs = v.z * s;
    return
    {{
      {1 - 2*yy - 2*zz, 2*xy + 2*zs    , 2*xz - 2*ys    , 0.f},
      {2*xy - 2*zs    , 1 - 2*xx - 2*zz, 2*yz + 2*xs    , 0.f},
      {2*xz + 2*ys    , 2*yz - 2*xs    , 1 - 2*xx - 2*yy, 0.f},
      {0.f            , 0.f            , 0.f            , 1.f}
    }};
  }
};
/**
 * @brief Grassman product. 
 * There are multiple kinds of quaternion multiplication, but this one is used for 3D rotation.
 * @return Quaternion representing rotation right followed by rotation left
 */
inline Quaternionf operator*(const Quaternionf& left, const Quaternionf& right) noexcept
{
  return
  {
    left.s*right.v + right.s*left.v + cross(left.v, right.v),
    left.s*right.s - dot(left.v, right.v)
  };
}
inline Quaternionf operator*(float left, const Quaternionf& right) noexcept
{
  return{left*right.v, left*right.s};
}
inline Quaternionf operator+(const Quaternionf& left, const Quaternionf& right) noexcept
{
  return{left.v + right.v, left.s + right.s};
}
inline float length(const Quaternionf& q) noexcept
{
  return sqrt(q.v.x*q.v.x + q.v.y*q.v.y + q.v.z*q.v.z + q.s*q.s);
}
/**
 * @note To speed this up for renormalization, check http://allenchou.net/2014/02/game-math-fast-re-normalization-of-unit-vectors/ 
*/
inline Quaternionf normalized(const Quaternionf& q, float length) noexcept
{
  float lengthInversion = 1.f / length;
  return
  {
    {q.v.x * lengthInversion, q.v.y * lengthInversion, q.v.z * lengthInversion},
    q.s * lengthInversion
  };
}
inline Quaternionf normalized(const Quaternionf& q) noexcept
{
  return normalized(q, length(q));
}
/**
 * @note Equals to inverse if q is normalized
 */
inline Quaternionf conjugate(const Quaternionf& q) noexcept
{
  return { -q.v, q.s };
}
inline Vec3f rotated(const Vec3f& v, const Quaternionf& q, const Quaternionf& qConjugate) noexcept
{
  return (q * Quaternionf{ v, 0.f } * qConjugate).v;
}
inline Vec3f rotated(const Vec3f& v, const Quaternionf& q) noexcept
{
  return rotated(v, q, conjugate(q));
}
constexpr inline float dot(const Quaternionf& q1, const Quaternionf& q2) noexcept
{
  return (q1.v.x * q2.v.x) + (q1.v.y * q2.v.y) + (q1.v.z * q2.v.z) + (q1.s * q2.s);
}
/**
 * @brief Rotational linear interpolation. Not accurate as slerp, but faster.
 */
inline Quaternionf rlerp(const Quaternionf& q1, const Quaternionf& q2, float t) noexcept
{
  float oneMinusT = 1.f - t;
  return normalized({lerp(q1.v, q2.v, t, oneMinusT), oneMinusT*q1.s + t*q2.s});
}
/**
 * @brief Spherical linear interpolation. More accurate than rlerp, but slower.
 */
inline Quaternionf slerp(const Quaternionf& q1, const Quaternionf& q2, float t) noexcept
{
  float theta = acos(dot(q1, q2));
  float wq1 = sin(1.f - t)*theta / sin(theta);
  float wq2 = sin(t)*theta / sin(theta);
  return wq1*q1 + wq2*q2;
}
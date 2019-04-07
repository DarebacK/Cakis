#pragma once

#include "Vec.hpp"
#include "Mat.hpp"
#include <cmath>

namespace De
{
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
    return sqrt((v.x * v.x) + (v.y * v.y));
  }
  inline auto length(const Vec3f& v)
  {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
  }
  inline auto length(const Vec4f& v)
  {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
  }

  inline Vec2f normalized(const Vec2f& v)
  {
    const auto length = De::length(v);
    return {v.x / length, v.y / length};
  }
  inline Vec3f normalized(const Vec3f& v)
  {
    const auto length = De::length(v);
    return {v.x / length, v.y / length, v.z / length};
  }
  inline Vec4f normalized(const Vec4f& v)
  {
    const auto length = De::length(v);
    return {v.x / length, v.y / length, v.z / length, v.w / length};
  }
}
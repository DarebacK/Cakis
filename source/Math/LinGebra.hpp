#pragma once

#include <cmath>

namespace De
{
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

  template<int columnCountT, int rowCountT, typename T>
  struct Mat
  {
    static constexpr int columnCount = columnCountT;
    static constexpr int rowCount = rowCountT;

    T* operator[](int index) {return values[index];};
    const T* operator[](int index) const {return values[index];};

  private:
    T values[rowCount][columnCount];
  };
  template<typename T>
  using Mat3 = Mat<3, 3, T>;
  using Mat3f = Mat3<float>;
  template<typename T>
  using Mat4 = Mat<4, 4, T>;
  using Mat4f = Mat4<float>;

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
#pragma once

namespace De
{
  template<typename T>
  struct Vec2
  {
    T x, y;
  };
  using Vec2f = Vec2<float>;

  template<typename T>
  struct Vec3
  {
    T x, y, z;
  };
  using Vec3f = Vec3<float>;

  template<typename T>
  struct Vec4
  {
    T x, y, z, w;
  };
	using Vec4f = Vec4<float>;
}
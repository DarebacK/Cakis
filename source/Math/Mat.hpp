#pragma once

namespace De
{
  template<int columnCountT, int rowCountT, typename T>
  struct Mat
  {
    static constexpr int columnCount = columnCountT;
    static constexpr int rowCount = rowCountT;

    T& operator[](int index){return values[index];};
    const T& operator[](int index) const{return values[index];};

  private:
    T values[rowCount][columnCount];
  };
  template<typename T>
  using Mat3 = Mat<3, 3, T>;
  using Mat3f = Mat3<float>;
  template<typename T>
  using Mat4 = Mat<4, 4, T>;
  using Mat4f = Mat4<float>;
}
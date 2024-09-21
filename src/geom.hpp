#pragma once

#include <GL/glut.h>
#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <array>

#ifndef PI
#define PI 3.14159265358979323846
#endif

template <typename T, std::size_t dimensions>
class Vector {
 public:
  std::array<T, dimensions> _Data;

  T& operator[](std::size_t idx) { return _Data[idx]; }
  const T& operator[](std::size_t idx) const { return _Data[idx]; }

  friend std::ostream& operator<<(std::ostream& out,
                                  const Vector<T, dimensions>& classObj) {
    out << "[ ";
    for (int i = 0; i < dimensions; ++i) {
      out << classObj._Data[i];
      if (i < (dimensions - 1)) out << ", ";
    }
    out << " ]";
    return out;
  }
};

template <typename T, std::size_t dimensions>
Vector<T, dimensions> operator+(const Vector<T, dimensions>& left,
                                const Vector<T, dimensions>& right) {
  Vector<T, dimensions> result;
  for (size_t i = 0; i < dimensions; ++i) {
    result._Data[i] = left._Data[i] + right._Data[i];
  }
  return result;
}

template <typename T, std::size_t dimensions>
Vector<T, dimensions> operator-(const Vector<T, dimensions>& left,
                                const Vector<T, dimensions>& right) {
  Vector<T, dimensions> result;
  for (size_t i = 0; i < dimensions; ++i) {
    result._Data[i] = left._Data[i] - right._Data[i];
  }
  return result;
}

template <typename T, std::size_t dimensions>
Vector<T, dimensions> operator*(const Vector<T, dimensions>& left,
                                const Vector<T, dimensions>& right) {
  Vector<T, dimensions> result;
  for (size_t i = 0; i < dimensions; ++i) {
    result._Data[i] = left._Data[i] * right._Data[i];
  }
  return result;
}

template <typename T, std::size_t dimensions>
Vector<T, dimensions> operator*(const Vector<T, dimensions>& left,
                                const double right) {
  Vector<T, dimensions> result;
  for (size_t i = 0; i < dimensions; ++i) {
    result._Data[i] = left._Data[i] * right;
  }
  return result;
}

template <typename T, std::size_t dimensions>
double operator%(const Vector<T, dimensions>& left,
                const Vector<T, dimensions>& right) {
  double result = 0;
  for (size_t i = 0; i < dimensions; ++i) {
    result += left._Data[i] * right._Data[i];
  }
  return result;
}

template <typename T, std::size_t dimensions>
T operator!(const Vector<T, dimensions>& e) {
  T rst = 0;
  for (size_t i = 0; i < dimensions; i++) {
    rst += e._Data[i] * e._Data[i];
  }
  return sqrt(rst);
}

template <typename T, std::size_t dimensions>
Vector<T, dimensions> operator~(const Vector<T, dimensions>& e) {
  const T magnitude = !e;
  Vector<T, dimensions> result;
  for (size_t i = 0; i < dimensions; ++i) {
    result._Data[i] = e._Data[i] / magnitude;
  }
  return result;
}

template <typename T>
Vector<T, 3> operator^(const Vector<T, 3>& left, const Vector<T, 3>& right) {
  Vector<T, 3> result;
  result._Data = {
      (left._Data[1] * right._Data[2]) - (left._Data[2] * right._Data[1]),
      (left._Data[2] * right._Data[0]) - (left._Data[0] * right._Data[2]),
      (left._Data[0] * right._Data[1]) - (left._Data[1] * right._Data[0])};
  return result;
}

class dVector3D : public Vector<double, 3> {
 public:
  dVector3D() {}
  dVector3D(const Vector<double, 3> ref) {
    this->_Data = ref._Data;
  }
  dVector3D(double x, double y, double z) {
    this->_Data = {x, y, z};
  }

  double x() const { return this->_Data[0]; }
  void setX(const double x) { this->_Data[0] = x; }
  double y() const { return this->_Data[1]; }
  void setY(const double y) { this->_Data[1] = y; }
  double z() const { return this->_Data[2]; }
  void setZ(const double z) { this->_Data[2] = z; }
};

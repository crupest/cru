#pragma once
#include "GraphBase.hpp"

#include <cmath>

namespace cru::platform {
struct Matrix {
  float m11;
  float m12;
  float m21;
  float m22;
  float m31;
  float m32;

  Matrix() {}

  Matrix(float m11, float m12, float m21, float m22, float m31, float m32) {
    this->m11 = m11;
    this->m12 = m12;
    this->m21 = m21;
    this->m22 = m22;
    this->m31 = m31;
    this->m32 = m32;
  }

  bool IsIdentity() const {
    return m11 == 1.0f && m12 == 0.0f && m21 == 0.0f && m22 == 1.0f &&
           m31 == 0.0f && m32 == 0.0f;
  }

  Matrix& operator*=(const Matrix& matrix) {
    *this = Product(*this, matrix);
    return *this;
  }

  Matrix operator*(const Matrix& matrix) const {
    return Product(*this, matrix);
  }

  Point TransformPoint(const Point& point) const {
    return Point{point.x * m11 + point.y * m21 + m31,
                 point.x * m12 + point.y * m22 + m32};
  }

  static Matrix Identity() {
    return Matrix{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
  }

  static Matrix Translation(float x, float y) {
    return Matrix{1.0f, 0.0f, 0.0f, 1.0f, x, y};
  }

  static Matrix Scale(float sx, float sy) {
    return Matrix{sx, 0.0f, 0.0f, sy, 0.0f, 0.0f};
  }

  static Matrix Rotation(float angle) {
    float r = AngleToRadian(angle);
    float s = std::sin(r);
    float c = std::cos(r);

    return Matrix{c, s, -s, c, 0.0f, 0.0f};
  }

  static Matrix Skew(float sx, float sy) {
    return Matrix{1.0f, sx, sy, 1.0f, 0.0f, 0.0f};
  }

  static Matrix Product(const Matrix& a, const Matrix& b) {
    return Matrix{a.m11 * b.m11 + a.m12 * b.m21,
                  a.m11 * b.m12 + a.m12 * b.m22,
                  a.m21 * b.m11 + a.m22 * b.m21,
                  a.m21 * b.m12 + a.m22 * b.m22,
                  a.m31 * b.m11 + a.m32 * b.m21 + b.m31,
                  a.m31 * b.m12 + a.m32 * b.m22 + b.m32};
  }

 private:
  static constexpr float PI = 3.1415926535f;

  static float AngleToRadian(float angle) { return angle / 180.f * PI; }
};
}  // namespace cru::platform

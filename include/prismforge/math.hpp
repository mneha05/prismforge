#pragma once

#include <algorithm>
#include <cmath>

namespace prismforge {

struct Vec3 {
  double x{0.0};
  double y{0.0};
  double z{0.0};

  Vec3 operator+(const Vec3 &other) const { return {x + other.x, y + other.y, z + other.z}; }
  Vec3 operator-(const Vec3 &other) const { return {x - other.x, y - other.y, z - other.z}; }
  Vec3 operator-() const { return {-x, -y, -z}; }
  Vec3 operator*(double scalar) const { return {x * scalar, y * scalar, z * scalar}; }
  Vec3 operator/(double scalar) const { return {x / scalar, y / scalar, z / scalar}; }
  Vec3 &operator+=(const Vec3 &other) { x += other.x; y += other.y; z += other.z; return *this; }
};

inline Vec3 operator*(double scalar, const Vec3 &value) { return value * scalar; }
inline Vec3 hadamard(const Vec3 &a, const Vec3 &b) { return {a.x * b.x, a.y * b.y, a.z * b.z}; }
inline double dot(const Vec3 &a, const Vec3 &b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Vec3 cross(const Vec3 &a, const Vec3 &b) {
  return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
inline double length(const Vec3 &value) { return std::sqrt(dot(value, value)); }
inline Vec3 normalized(const Vec3 &value) {
  const double magnitude = length(value);
  return magnitude > 1e-12 ? value / magnitude : Vec3{};
}
inline Vec3 clamp01(const Vec3 &value) {
  return {std::clamp(value.x, 0.0, 1.0), std::clamp(value.y, 0.0, 1.0),
          std::clamp(value.z, 0.0, 1.0)};
}
inline Vec3 reflect(const Vec3 &incident, const Vec3 &normal) {
  return incident - normal * (2.0 * dot(incident, normal));
}

struct Ray { Vec3 origin; Vec3 direction; };

}  // namespace prismforge

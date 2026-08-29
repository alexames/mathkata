/*
 * Copyright 2026 Alexander Ames. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MATHKATA_SPHERE_H_
#define MATHKATA_SPHERE_H_

#include <cmath>
#include <numbers>

#include "mathkata/vector.h"

/// @file mathkata/sphere.h Sphere
/// @brief Sphere/Circle class and functions.
/// @addtogroup mathkata_sphere

namespace mathkata {

/// @addtogroup mathkata_sphere
/// @{
/// @class Sphere "mathkata/sphere.h"
/// @brief Sphere (or Circle) of type T in N dimensions.
///
/// Sphere stores a center point as a <b>Vector<T, N></b> and a radius of type
/// <b>T</b>. When N=2, this represents a circle. When N=3, this represents a
/// sphere.
///
/// @tparam T type of Sphere elements.
/// @tparam N number of dimensions (2 for circle, 3 for sphere).
template <class T, int N>
struct Sphere {
  /// @brief The center of the sphere.
  Vector<T, N> center;
  /// @brief The radius of the sphere.
  T radius;

  /// @brief Create a Sphere without assigning its members.
  ///
  /// Default-initialization (Sphere<T, N> sphere;) leaves the members
  /// indeterminate; value-initialization (Sphere<T, N> sphere{}) zeroes the
  /// members. A zeroed Sphere is the point sphere at the origin.
  Sphere() = default;

  /// @brief Create a sphere from a center point and radius.
  ///
  /// @param center The center point of the sphere.
  /// @param radius The radius of the sphere.
  constexpr Sphere(const Vector<T, N>& center, T radius)
      : center(center), radius(radius) {}

  /// @brief Check whether a point is inside (or on the boundary of) the sphere.
  ///
  /// @param point The point to test.
  /// @return true if the point is inside the sphere, false otherwise.
  constexpr bool contains(const Vector<T, N>& point) const {
    return Vector<T, N>::distanceSquared(center, point) <= radius * radius;
  }

  /// @brief Check whether another sphere is fully contained within this sphere.
  ///
  /// @param other The sphere to test.
  /// @return true if the other sphere is fully inside this sphere, false
  ///     otherwise.
  inline bool contains(const Sphere<T, N>& other) const {
    T dist = Vector<T, N>::distance(center, other.center);
    return dist + other.radius <= radius;
  }

  /// @brief Check whether another sphere intersects (overlaps) this sphere.
  ///
  /// @param other The sphere to test for intersection.
  /// @return true if the spheres overlap, false otherwise.
  constexpr bool intersects(const Sphere<T, N>& other) const {
    T sum_radii = radius + other.radius;
    return Vector<T, N>::distanceSquared(center, other.center)
           <= sum_radii * sum_radii;
  }

  /// @brief Calculate the area of a 2D circle (N=2).
  ///
  /// @return The area of the circle (pi * r^2).
  constexpr T area() const {
    static_assert(N == 2, "area() is only defined for 2D circles (N=2).");
    return std::numbers::pi_v<T> * radius * radius;
  }

  /// @brief Calculate the volume of a 3D sphere (N=3).
  ///
  /// @return The volume of the sphere (4/3 * pi * r^3).
  constexpr T volume() const {
    static_assert(N == 3, "volume() is only defined for 3D spheres (N=3).");
    return (static_cast<T>(4) / static_cast<T>(3)) * std::numbers::pi_v<T>
           * radius * radius * radius;
  }

  /// @brief Calculate the diameter of the sphere.
  ///
  /// @return The diameter (2 * radius).
  constexpr T diameter() const { return static_cast<T>(2) * radius; }
};
/// @}

/// @brief Check if two spheres are identical.
///
/// @param s1 Sphere to be tested.
/// @param s2 Other sphere to be tested.
template <class T, int N>
constexpr bool operator==(const Sphere<T, N>& s1, const Sphere<T, N>& s2) {
  return (s1.center == s2.center && s1.radius == s2.radius);
}

/// @brief Check if two spheres are <b>not</b> identical.
///
/// @param s1 Sphere to be tested.
/// @param s2 Other sphere to be tested.
template <class T, int N>
constexpr bool operator!=(const Sphere<T, N>& s1, const Sphere<T, N>& s2) {
  return !(s1 == s2);
}

}  // namespace mathkata

#endif  // MATHKATA_SPHERE_H_

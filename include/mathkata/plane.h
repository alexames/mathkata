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
#ifndef MATHKATA_PLANE_H_
#define MATHKATA_PLANE_H_

#include "mathkata/vector.h"

namespace mathkata {

/// @addtogroup mathkata_plane
/// @{
/// @class Plane "mathkata/plane.h"
/// @brief Plane of type T in 3D space using normal + distance representation.
///
/// Plane stores a 3D normal vector and a signed distance from the origin,
/// representing the plane equation n dot x + d = 0.
///
/// @tparam T type of Plane elements.
template <class T>
struct Plane {
  Vector<T, 3> normal;
  T distance;

  /// @brief Create an uninitialized Plane.
  ///
  /// The members of the Plane are left uninitialized and have indeterminate
  /// values. This is intentional for performance: use one of the other
  /// constructors or factory methods if you need specific values.
  Plane() {}

  /// @brief Create a plane from a normal vector and distance.
  ///
  /// @param normal The normal vector of the plane.
  /// @param distance The signed distance from the origin.
  constexpr Plane(const Vector<T, 3>& normal, T distance)
      : normal(normal), distance(distance) {}

  /// @brief Create a plane from a Vector4 (xyz = normal, w = distance).
  ///
  /// @param v Vector4 where xyz components are the normal and w is the
  ///          signed distance.
  explicit constexpr Plane(const Vector<T, 4>& v)
      : normal(v.xyz()), distance(v[3]) {}

  /// @brief Create a plane through a point with a given normal.
  ///
  /// @param point A point on the plane.
  /// @param normal The normal vector of the plane.
  /// @return A Plane passing through the given point with the given normal.
  static constexpr Plane<T> FromPointNormal(const Vector<T, 3>& point,
                                            const Vector<T, 3>& normal) {
    return Plane<T>(normal, -Vector<T, 3>::DotProduct(normal, point));
  }

  /// @brief Create a plane through three points.
  ///
  /// The normal is computed as the cross product of (b - a) and (c - a),
  /// following a counter-clockwise winding order.
  ///
  /// @param a First point on the plane.
  /// @param b Second point on the plane.
  /// @param c Third point on the plane.
  /// @return A Plane passing through the three given points.
  static inline Plane<T> FromPoints(const Vector<T, 3>& a,
                                    const Vector<T, 3>& b,
                                    const Vector<T, 3>& c) {
    const Vector<T, 3> normal =
        Vector<T, 3>::CrossProduct(b - a, c - a).Normalized();
    return FromPointNormal(a, normal);
  }

  /// @brief Compute the signed distance from a point to the plane.
  ///
  /// Positive values indicate the point is on the side of the plane the
  /// normal points toward. Negative values indicate the opposite side.
  /// Zero means the point lies on the plane.
  ///
  /// @param point The point to compute the distance for.
  /// @return The signed distance from the point to the plane.
  constexpr T SignedDistance(const Vector<T, 3>& point) const {
    return Vector<T, 3>::DotProduct(normal, point) + distance;
  }

  /// @brief Project a point onto the plane (closest point on plane).
  ///
  /// @param point The point to project.
  /// @return The closest point on the plane to the given point.
  constexpr Vector<T, 3> ProjectPoint(const Vector<T, 3>& point) const {
    return point - normal * SignedDistance(point);
  }

  /// @brief Return a plane with negated normal and distance.
  ///
  /// The resulting plane represents the same geometric surface but with the
  /// opposite facing direction.
  ///
  /// @return A new Plane with negated normal and distance.
  constexpr Plane<T> Flipped() const { return Plane<T>(-normal, -distance); }
};
/// @}

/// @brief Check if two planes are identical.
///
/// @param p1 Plane to be tested.
/// @param p2 Other plane to be tested.
template <class T>
constexpr bool operator==(const Plane<T>& p1, const Plane<T>& p2) {
  return (p1.normal == p2.normal && p1.distance == p2.distance);
}

/// @brief Check if two planes are <b>not</b> identical.
///
/// @param p1 Plane to be tested.
/// @param p2 Other plane to be tested.
template <class T>
constexpr bool operator!=(const Plane<T>& p1, const Plane<T>& p2) {
  return !(p1 == p2);
}

}  // namespace mathkata

#endif  // MATHKATA_PLANE_H_

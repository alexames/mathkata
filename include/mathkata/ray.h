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
#ifndef MATHKATA_RAY_H_
#define MATHKATA_RAY_H_

#include "mathkata/vector.h"

namespace mathkata {

/// @addtogroup mathkata_ray
/// @{
/// @class Ray "mathkata/ray.h"
/// @brief Ray defined by an origin and a direction.
///
/// Ray stores an origin point and a direction vector. The direction should be
/// unit length by convention, but this is not enforced.
///
/// @tparam T type of Ray elements.
/// @tparam Dims number of dimensions.
template <class T, int Dims>
struct Ray {
  /// @brief Create an uninitialized Ray.
  ///
  /// The members are left uninitialized and have indeterminate values.
  Ray() {}

  /// @brief Create a Ray from an origin and a direction.
  ///
  /// @param origin The origin point of the ray.
  /// @param direction The direction vector of the ray.
  constexpr Ray(const Vector<T, Dims>& origin, const Vector<T, Dims>& direction)
      : origin(origin), direction(direction) {}

  /// @brief Compute the point at parameter t along the ray.
  ///
  /// Returns origin + direction * t.
  /// @param t The parameter value.
  /// @return The point at parameter t.
  constexpr Vector<T, Dims> PointAt(T t) const {
    return origin + direction * t;
  }

  /// The origin point of the ray.
  Vector<T, Dims> origin;

  /// The direction vector of the ray.
  Vector<T, Dims> direction;
};
/// @}

/// @brief Check if two rays are identical.
///
/// @param r1 Ray to be tested.
/// @param r2 Other ray to be tested.
template <class T, int Dims>
constexpr bool operator==(const Ray<T, Dims>& r1, const Ray<T, Dims>& r2) {
  return (r1.origin == r2.origin && r1.direction == r2.direction);
}

/// @brief Check if two rays are <b>not</b> identical.
///
/// @param r1 Ray to be tested.
/// @param r2 Other ray to be tested.
template <class T, int Dims>
constexpr bool operator!=(const Ray<T, Dims>& r1, const Ray<T, Dims>& r2) {
  return !(r1 == r2);
}

/// @addtogroup mathkata_line
/// @{
/// @class Line "mathkata/ray.h"
/// @brief Infinite line defined by a point and a direction.
///
/// Line stores a point on the line and a direction vector. The direction
/// should be unit length by convention, but this is not enforced.
///
/// @tparam T type of Line elements.
/// @tparam Dims number of dimensions.
template <class T, int Dims>
struct Line {
  /// @brief Create an uninitialized Line.
  ///
  /// The members are left uninitialized and have indeterminate values.
  Line() {}

  /// @brief Create a Line from a point and a direction.
  ///
  /// @param point A point on the line.
  /// @param direction The direction vector of the line.
  constexpr Line(const Vector<T, Dims>& point, const Vector<T, Dims>& direction)
      : point(point), direction(direction) {}

  /// @brief Create a Line from two points.
  ///
  /// The direction is computed as (b - a).Normalized().
  /// @param a The first point.
  /// @param b The second point.
  /// @return A Line passing through a and b.
  static Line<T, Dims> FromPoints(const Vector<T, Dims>& a,
                                  const Vector<T, Dims>& b) {
    return Line<T, Dims>(a, (b - a).Normalized());
  }

  /// @brief Compute the point at parameter t along the line.
  ///
  /// Returns point + direction * t.
  /// @param t The parameter value.
  /// @return The point at parameter t.
  constexpr Vector<T, Dims> PointAt(T t) const { return point + direction * t; }

  /// A point on the line.
  Vector<T, Dims> point;

  /// The direction vector of the line.
  Vector<T, Dims> direction;
};
/// @}

/// @brief Check if two lines are identical.
///
/// @param l1 Line to be tested.
/// @param l2 Other line to be tested.
template <class T, int Dims>
constexpr bool operator==(const Line<T, Dims>& l1, const Line<T, Dims>& l2) {
  return (l1.point == l2.point && l1.direction == l2.direction);
}

/// @brief Check if two lines are <b>not</b> identical.
///
/// @param l1 Line to be tested.
/// @param l2 Other line to be tested.
template <class T, int Dims>
constexpr bool operator!=(const Line<T, Dims>& l1, const Line<T, Dims>& l2) {
  return !(l1 == l2);
}

/// @addtogroup mathkata_line_segment
/// @{
/// @class LineSegment "mathkata/ray.h"
/// @brief Line segment defined by two endpoints.
///
/// LineSegment stores two endpoints, start and end.
///
/// @tparam T type of LineSegment elements.
/// @tparam Dims number of dimensions.
template <class T, int Dims>
struct LineSegment {
  /// @brief Create an uninitialized LineSegment.
  ///
  /// The members are left uninitialized and have indeterminate values.
  LineSegment() {}

  /// @brief Create a LineSegment from two endpoints.
  ///
  /// @param start The start point.
  /// @param end The end point.
  constexpr LineSegment(const Vector<T, Dims>& start,
                        const Vector<T, Dims>& end)
      : start(start), end(end) {}

  /// @brief Compute the midpoint of the segment.
  ///
  /// @return The center point between start and end.
  constexpr Vector<T, Dims> Center() const {
    return (start + end) * static_cast<T>(0.5);
  }

  /// @brief Compute the length of the segment.
  ///
  /// @return The distance between start and end.
  T Length() const { return (end - start).Length(); }

  /// @brief Compute the squared length of the segment.
  ///
  /// @return The squared distance between start and end.
  constexpr T LengthSquared() const { return (end - start).LengthSquared(); }

  /// @brief Compute the normalized direction from start to end.
  ///
  /// The segment must have non-zero length.
  /// @return The unit direction vector from start to end.
  Vector<T, Dims> Direction() const { return (end - start).Normalized(); }

  /// @brief Compute the closest point on the segment to a given point.
  ///
  /// @param point The point to find the closest point to.
  /// @return The closest point on the segment to the given point.
  constexpr Vector<T, Dims> ClosestPoint(const Vector<T, Dims>& point) const {
    const Vector<T, Dims> segment = end - start;
    const T length_squared = segment.LengthSquared();
    if (length_squared == static_cast<T>(0)) {
      return start;
    }
    T t = Vector<T, Dims>::DotProduct(point - start, segment) / length_squared;
    if (t < static_cast<T>(0)) {
      t = static_cast<T>(0);
    } else if (t > static_cast<T>(1)) {
      t = static_cast<T>(1);
    }
    return start + segment * t;
  }

  /// The start point of the segment.
  Vector<T, Dims> start;

  /// The end point of the segment.
  Vector<T, Dims> end;
};
/// @}

/// @brief Check if two line segments are identical.
///
/// @param s1 LineSegment to be tested.
/// @param s2 Other LineSegment to be tested.
template <class T, int Dims>
constexpr bool operator==(const LineSegment<T, Dims>& s1,
                          const LineSegment<T, Dims>& s2) {
  return (s1.start == s2.start && s1.end == s2.end);
}

/// @brief Check if two line segments are <b>not</b> identical.
///
/// @param s1 LineSegment to be tested.
/// @param s2 Other LineSegment to be tested.
template <class T, int Dims>
constexpr bool operator!=(const LineSegment<T, Dims>& s1,
                          const LineSegment<T, Dims>& s2) {
  return !(s1 == s2);
}

}  // namespace mathkata

#endif  // MATHKATA_RAY_H_

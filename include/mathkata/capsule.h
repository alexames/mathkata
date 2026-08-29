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
#ifndef MATHKATA_CAPSULE_H_
#define MATHKATA_CAPSULE_H_

#include "mathkata/vector.h"

namespace mathkata {

/// @addtogroup mathkata_capsule
/// @{
/// @class Capsule "mathkata/capsule.h"
/// @brief Capsule of type T in N dimensions.
///
/// A capsule is the Minkowski sum of a line segment and a sphere. It is
/// defined by two endpoints (start and end) and a radius.
///
/// @tparam T type of Capsule elements.
/// @tparam N number of dimensions.
template <class T, int N>
struct Capsule {
  Vector<T, N> start;
  Vector<T, N> end;
  T radius;

 private:
  /// Selects the constructor that leaves the members unassigned. Private,
  /// so uninitialized() is the only way to reach it.
  struct UninitializedTag {};

  /// @brief Create a Capsule without assigning the members.
  ///
  /// @param tag Unused; selects this constructor.
  explicit Capsule(UninitializedTag tag)
      : start(Vector<T, N>::uninitialized()),
        end(Vector<T, N>::uninitialized()) {
    static_cast<void>(tag);
  }

 public:
  /// @brief Deleted; give the members, or call uninitialized() to skip
  ///        assigning them on purpose.
  Capsule() = delete;

  /// @brief Create a Capsule without assigning the members.
  ///
  /// Reading any of the members before assigning it is undefined behavior.
  /// This is for code that fills every one of them immediately, where zeroing
  /// first would be a wasted store.
  ///
  /// @return A Capsule with indeterminate members.
  static inline Capsule<T, N> uninitialized() {
    return Capsule<T, N>(UninitializedTag{});
  }

  /// @brief Create a Capsule from start and end points and a radius.
  ///
  /// @param start The start endpoint of the capsule's line segment.
  /// @param end The end endpoint of the capsule's line segment.
  /// @param radius The radius of the capsule.
  constexpr Capsule(const Vector<T, N>& start, const Vector<T, N>& end,
                    T radius)
      : start(start), end(end), radius(radius) {}

  /// @brief Calculate the center of the capsule.
  ///
  /// The center is the midpoint of the start and end endpoints.
  ///
  /// @return The center point of the capsule.
  constexpr Vector<T, N> center() const {
    return (start + end) * static_cast<T>(0.5);
  }

  /// @brief Calculate the length of the capsule's line segment.
  ///
  /// This is the distance from start to end, not including the radius caps.
  ///
  /// @return The length of the line segment.
  inline T length() const { return Vector<T, N>::distance(start, end); }

  /// @brief Test whether a point is inside the capsule.
  ///
  /// A point is inside the capsule if its distance to the closest point on
  /// the line segment (from start to end) is less than or equal to the radius.
  ///
  /// @param point The point to test.
  /// @return true if the point is inside the capsule, false otherwise.
  constexpr bool contains(const Vector<T, N>& point) const {
    const Vector<T, N> closest = closestPointOnSegment(point, start, end);
    return Vector<T, N>::distanceSquared(point, closest) <= radius * radius;
  }

 private:
  /// @brief Find the closest point on a line segment to a given point.
  ///
  /// @param point The query point.
  /// @param seg_start The start of the line segment.
  /// @param seg_end The end of the line segment.
  /// @return The closest point on the segment to the query point.
  static constexpr Vector<T, N> closestPointOnSegment(
      const Vector<T, N>& point, const Vector<T, N>& seg_start,
      const Vector<T, N>& seg_end) {
    const Vector<T, N> segment = seg_end - seg_start;
    const T segment_length_squared = Vector<T, N>::dotProduct(segment, segment);
    if (segment_length_squared == static_cast<T>(0)) {
      return seg_start;
    }
    const T t = clamp(Vector<T, N>::dotProduct(point - seg_start, segment)
                          / segment_length_squared,
                      static_cast<T>(0), static_cast<T>(1));
    return seg_start + segment * t;
  }
};
/// @}

/// @brief Check if two capsules are identical.
///
/// @param c1 Capsule to be tested.
/// @param c2 Other capsule to be tested.
template <class T, int N>
constexpr bool operator==(const Capsule<T, N>& c1, const Capsule<T, N>& c2) {
  return (c1.start == c2.start && c1.end == c2.end && c1.radius == c2.radius);
}

/// @brief Check if two capsules are <b>not</b> identical.
///
/// @param c1 Capsule to be tested.
/// @param c2 Other capsule to be tested.
template <class T, int N>
constexpr bool operator!=(const Capsule<T, N>& c1, const Capsule<T, N>& c2) {
  return !(c1 == c2);
}

}  // namespace mathkata

#endif  // MATHKATA_CAPSULE_H_

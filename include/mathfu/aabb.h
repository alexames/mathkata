/*
 * Copyright 2024 Google Inc. All rights reserved.
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
#ifndef MATHFU_AABB_H_
#define MATHFU_AABB_H_

#include "mathfu/vector.h"

namespace mathfu {

/// @addtogroup mathfu_aabb
/// @{
/// @class AABB "mathfu/aabb.h"
/// @brief Axis-aligned bounding box of dimension N with scalar type T.
///
/// AABB stores two N-dimensional <b>Vector</b>s of type <b>T</b> representing
/// the minimum and maximum corners of an axis-aligned bounding box.
///
/// @tparam T type of AABB elements.
/// @tparam N number of dimensions (defaults to 3).
template <class T, int N = 3>
struct AABB {
  /// Minimum corner of the bounding box.
  Vector<T, N> min;
  /// Maximum corner of the bounding box.
  Vector<T, N> max;

  /// @brief Create an uninitialized AABB.
  ///
  /// The elements of the AABB are left uninitialized and have indeterminate
  /// values. This is intentional for performance: use one of the other
  /// constructors if you need specific values.
  AABB() {}

  /// @brief Create an AABB from min and max corner vectors.
  ///
  /// @param min Minimum corner of the bounding box.
  /// @param max Maximum corner of the bounding box.
  constexpr AABB(const Vector<T, N>& min, const Vector<T, N>& max)
      : min(min), max(max) {}

  /// @brief Create an AABB from a center point and half-extents.
  ///
  /// @param center Center point of the bounding box.
  /// @param extents Half-size vector from center to max corner.
  /// @param tag Unused tag parameter to disambiguate from min/max constructor.
  struct FromCenterExtentsTag {};
  constexpr AABB(const Vector<T, N>& center, const Vector<T, N>& extents,
                 FromCenterExtentsTag)
      : min(center - extents), max(center + extents) {}

  /// @brief Create an AABB from a center point and half-extents.
  ///
  /// Factory function alternative to tagged constructor.
  /// @param center Center point of the bounding box.
  /// @param extents Half-size vector from center to max corner.
  /// @return A new AABB constructed from center and extents.
  static constexpr AABB<T, N> FromCenterExtents(const Vector<T, N>& center,
                                                const Vector<T, N>& extents) {
    return AABB<T, N>(center - extents, center + extents);
  }

  /// @brief Calculate the center point of the bounding box.
  ///
  /// @return The center point as a Vector.
  constexpr Vector<T, N> Center() const {
    return (min + max) * static_cast<T>(0.5);
  }

  /// @brief Calculate the half-size (extents) of the bounding box.
  ///
  /// @return The half-size vector from center to max corner.
  constexpr Vector<T, N> Extents() const {
    return (max - min) * static_cast<T>(0.5);
  }

  /// @brief Calculate the full size of the bounding box.
  ///
  /// @return The size vector (max - min).
  constexpr Vector<T, N> Size() const { return max - min; }

  /// @brief Test whether a point is contained within the AABB.
  ///
  /// The test is inclusive on all boundaries.
  /// @param point The point to test.
  /// @return true if the point is inside or on the boundary of the AABB.
  constexpr bool Contains(const Vector<T, N>& point) const {
    for (int i = 0; i < N; ++i) {
      if (point[i] < min[i] || point[i] > max[i]) {
        return false;
      }
    }
    return true;
  }

  /// @brief Test whether another AABB is fully contained within this AABB.
  ///
  /// @param other The AABB to test.
  /// @return true if other is entirely inside or on the boundary of this AABB.
  constexpr bool Contains(const AABB<T, N>& other) const {
    for (int i = 0; i < N; ++i) {
      if (other.min[i] < min[i] || other.max[i] > max[i]) {
        return false;
      }
    }
    return true;
  }

  /// @brief Test whether another AABB overlaps with this AABB.
  ///
  /// @param other The AABB to test for overlap.
  /// @return true if the two AABBs overlap (including touching boundaries).
  constexpr bool Intersects(const AABB<T, N>& other) const {
    for (int i = 0; i < N; ++i) {
      if (other.max[i] < min[i] || other.min[i] > max[i]) {
        return false;
      }
    }
    return true;
  }

  /// @brief Compute the intersection of this AABB with another.
  ///
  /// If the two AABBs do not overlap, the result is degenerate (min > max
  /// in one or more dimensions).
  /// @param other The AABB to intersect with.
  /// @return The intersection AABB.
  constexpr AABB<T, N> Intersection(const AABB<T, N>& other) const {
    return AABB<T, N>(Vector<T, N>::Max(min, other.min),
                      Vector<T, N>::Min(max, other.max));
  }

  /// @brief Compute the bounding union of this AABB with another.
  ///
  /// @param other The AABB to union with.
  /// @return The smallest AABB that contains both this and other.
  constexpr AABB<T, N> Union(const AABB<T, N>& other) const {
    return AABB<T, N>(Vector<T, N>::Min(min, other.min),
                      Vector<T, N>::Max(max, other.max));
  }

  /// @brief Expand this AABB to include a point.
  ///
  /// @param point The point to include.
  constexpr void Expand(const Vector<T, N>& point) {
    min = Vector<T, N>::Min(min, point);
    max = Vector<T, N>::Max(max, point);
  }

  /// @brief Expand this AABB to include another AABB.
  ///
  /// @param other The AABB to include.
  constexpr void Expand(const AABB<T, N>& other) {
    min = Vector<T, N>::Min(min, other.min);
    max = Vector<T, N>::Max(max, other.max);
  }
};
/// @}

/// @brief Check if two AABBs are identical.
///
/// @param a First AABB to compare.
/// @param b Second AABB to compare.
/// @return true if both min and max corners are equal.
template <class T, int N>
constexpr bool operator==(const AABB<T, N>& a, const AABB<T, N>& b) {
  return (a.min == b.min && a.max == b.max);
}

/// @brief Check if two AABBs are <b>not</b> identical.
///
/// @param a First AABB to compare.
/// @param b Second AABB to compare.
/// @return true if the AABBs differ.
template <class T, int N>
constexpr bool operator!=(const AABB<T, N>& a, const AABB<T, N>& b) {
  return !(a == b);
}

}  // namespace mathfu

#endif  // MATHFU_AABB_H_

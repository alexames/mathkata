/*
 * Copyright 2016 Google Inc. All rights reserved.
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
#ifndef MATHKATA_RECT_H_
#define MATHKATA_RECT_H_

#include <algorithm>

#include "mathkata/vector.h"

namespace mathkata {

/// @addtogroup mathkata_rect
/// @{
/// @class Rect "mathkata/rect.h"
/// @brief Rect of type T containing position (pos) and width.
///
/// Rect contains two 2D <b>Vector</b>s of type <b>T</b> representing position
/// (pos) and size.
///
/// @tparam T type of Rect elements.
template <class T>
struct Rect {
  Vector<T, 2> pos;
  Vector<T, 2> size;

  /// @brief Create a rect from a vector4 of the same type.
  ///
  /// @param v Vector that the data will be copied from.
  explicit constexpr Rect(const Vector<T, 4>& v)
      : pos(v.x, v.y), size(v.z, v.w) {}

  /// @brief Create a rect from x, y, width and height values.
  ///
  /// @param x the given x value.
  /// @param y the given y value.
  /// @param width the given width value.
  /// @param height the given height value.
  constexpr Rect(T x = static_cast<T>(0), T y = static_cast<T>(0),
                 T width = static_cast<T>(0), T height = static_cast<T>(0))
      : pos(x, y), size(width, height) {}

  /// @brief Create a rect from two vector2 representing position and size.
  ///
  /// @param pos Vector representing the position vector (x and y values).
  /// @param size Vector represening the size vector (width and height values).
  constexpr Rect(const Vector<T, 2>& pos, const Vector<T, 2>& size)
      : pos(pos), size(size) {}

  /// @brief Returns the center point of the rect.
  ///
  /// @return A Vector<T, 2> at the center of the rect.
  constexpr Vector<T, 2> center() const {
    return pos + size / static_cast<T>(2);
  }

  /// @brief Returns the minimum corner of the rect.
  ///
  /// This is equivalent to the position.
  ///
  /// @return A Vector<T, 2> at the minimum corner (pos).
  constexpr Vector<T, 2> min() const { return pos; }

  /// @brief Returns the maximum corner of the rect.
  ///
  /// @return A Vector<T, 2> at the maximum corner (pos + size).
  constexpr Vector<T, 2> max() const { return pos + size; }

  /// @brief Returns the area of the rect.
  ///
  /// @return The product of width and height.
  constexpr T area() const { return size.x * size.y; }

  /// @brief Tests whether a point is contained within the rect.
  ///
  /// A point on the boundary is considered contained.
  ///
  /// @param point The point to test.
  /// @return true if the point is inside or on the boundary of the rect.
  constexpr bool contains(const Vector<T, 2>& point) const {
    const Vector<T, 2> rect_max = max();
    return point.x >= pos.x && point.y >= pos.y && point.x <= rect_max.x
           && point.y <= rect_max.y;
  }

  /// @brief Tests whether another rect is entirely contained within this rect.
  ///
  /// @param other The rect to test.
  /// @return true if other is entirely inside or on the boundary of this rect.
  constexpr bool contains(const Rect<T>& other) const {
    return contains(other.min()) && contains(other.max());
  }

  /// @brief Tests whether another rect overlaps with this rect.
  ///
  /// @param other The rect to test.
  /// @return true if the rects overlap.
  constexpr bool intersects(const Rect<T>& other) const {
    const Vector<T, 2> this_max = max();
    const Vector<T, 2> other_max = other.max();
    return pos.x < other_max.x && this_max.x > other.pos.x
           && pos.y < other_max.y && this_max.y > other.pos.y;
  }

  /// @brief Returns the overlapping area of two rects.
  ///
  /// If the rects do not overlap, returns a rect with zero size.
  ///
  /// @param other The rect to intersect with.
  /// @return The intersection rect, or a zero-sized rect if no overlap.
  constexpr Rect<T> intersection(const Rect<T>& other) const {
    const Vector<T, 2> max1 = max();
    const Vector<T, 2> max2 = other.max();
    const Vector<T, 2> inter_min(std::max(pos.x, other.pos.x),
                                 std::max(pos.y, other.pos.y));
    const Vector<T, 2> inter_max(std::min(max1.x, max2.x),
                                 std::min(max1.y, max2.y));
    if (inter_min.x >= inter_max.x || inter_min.y >= inter_max.y) {
      return Rect<T>();
    }
    return Rect<T>(inter_min, inter_max - inter_min);
  }

  /// @brief Returns the bounding rect that contains both rects.
  ///
  /// @param other The rect to union with.
  /// @return The smallest rect containing both this and other.
  constexpr Rect<T> merge(const Rect<T>& other) const {
    const Vector<T, 2> union_min(std::min(pos.x, other.pos.x),
                                 std::min(pos.y, other.pos.y));
    const Vector<T, 2> union_max(std::max(max().x, other.max().x),
                                 std::max(max().y, other.max().y));
    return Rect<T>(union_min, union_max - union_min);
  }

  /// @brief Expands the rect to include the given point.
  ///
  /// If the point is already contained, the rect is unchanged.
  ///
  /// @param point The point to include.
  /// @return A new rect expanded to contain the point.
  constexpr Rect<T> expand(const Vector<T, 2>& point) const {
    const Vector<T, 2> new_min(std::min(pos.x, point.x),
                               std::min(pos.y, point.y));
    const Vector<T, 2> new_max(std::max(max().x, point.x),
                               std::max(max().y, point.y));
    return Rect<T>(new_min, new_max - new_min);
  }

  /// @brief Expands the rect by the given amount in all directions.
  ///
  /// The position is moved by (-amount, -amount) and the size is increased
  /// by (2*amount, 2*amount).
  ///
  /// @param amount The amount to expand by.
  /// @return A new rect expanded by the given amount.
  constexpr Rect<T> expand(T amount) const {
    return Rect<T>(Vector<T, 2>(pos.x - amount, pos.y - amount),
                   Vector<T, 2>(size.x + static_cast<T>(2) * amount,
                                size.y + static_cast<T>(2) * amount));
  }
};
/// @}

/// @brief Check if two rects are identical.
///
/// @param r1 Rect to be tested.
/// @param r2 Other rect to be tested.
template <class T>
constexpr bool operator==(const Rect<T>& r1, const Rect<T>& r2) {
  return (r1.pos == r2.pos && r1.size == r2.size);
}

/// @brief Check if two rects are <b>not</b> identical.
///
/// @param r1 Rect to be tested.
/// @param r2 Other rect to be tested.
template <class T>
constexpr bool operator!=(const Rect<T>& r1, const Rect<T>& r2) {
  return !(r1 == r2);
}

/// @brief Lexicographic less-than comparison for two Rects.
///
/// Compares by position first, then by size.  This provides a strict weak
/// ordering suitable for use in sorted containers such as std::set and
/// std::map.
///
/// @param r1 Rect to be tested.
/// @param r2 Other rect to be tested.
/// @return true if @p r1 is lexicographically less than @p r2.
template <class T>
constexpr bool operator<(const Rect<T>& r1, const Rect<T>& r2) {
  if (r1.pos != r2.pos) return r1.pos < r2.pos;
  return r1.size < r2.size;
}

}  // namespace mathkata

#endif  // MATHKATA_RECT_H_

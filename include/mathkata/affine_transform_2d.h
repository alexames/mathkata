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
#ifndef MATHKATA_AFFINE_TRANSFORM_2D_H_
#define MATHKATA_AFFINE_TRANSFORM_2D_H_

#include <algorithm>
#include <cmath>
#include <numbers>

#include "mathkata/rect.h"
#include "mathkata/vector.h"

namespace mathkata {

/// @addtogroup mathkata_affine_transform_2d
/// @{
/// @class AffineTransform2D "mathkata/affine_transform_2d.h"
/// @brief Represents a 2D affine transformation as a 3x3 matrix.
///
/// Internally stores the transform as a 4x4 column-major matrix for GPU
/// compatibility. The 3x3 affine portion is embedded in the 4x4 with the
/// third row and column being identity.
///
/// The 3x3 row-major representation maps to 4x4 column-major as follows:
/// @code
/// Row-major 3x3:           Column-major 4x4:
/// [a00 a01 a02]            col0: [a00, a10, 0, 0]
/// [a10 a11 a12]            col1: [a01, a11, 0, 0]
/// [a20 a21 a22]            col2: [0,   0,   1, 0]
///                          col3: [a02, a12, 0, a22]
/// @endcode
///
/// @tparam T type of AffineTransform2D elements.
template <class T>
class AffineTransform2D {
 public:
  /// @brief Create an identity transform.
  constexpr AffineTransform2D() : matrix_{} {
    // Column 0
    matrix_[0] = T(1);
    matrix_[1] = T(0);
    matrix_[2] = T(0);
    matrix_[3] = T(0);
    // Column 1
    matrix_[4] = T(0);
    matrix_[5] = T(1);
    matrix_[6] = T(0);
    matrix_[7] = T(0);
    // Column 2
    matrix_[8] = T(0);
    matrix_[9] = T(0);
    matrix_[10] = T(1);
    matrix_[11] = T(0);
    // Column 3
    matrix_[12] = T(0);
    matrix_[13] = T(0);
    matrix_[14] = T(0);
    matrix_[15] = T(1);
  }

  /// @brief Create a transform from 3x3 row-major elements.
  ///
  /// @param a00 Row 0, Column 0 (X scale / rotation).
  /// @param a01 Row 0, Column 1 (X shear / rotation).
  /// @param a02 Row 0, Column 2 (X translation).
  /// @param a10 Row 1, Column 0 (Y shear / rotation).
  /// @param a11 Row 1, Column 1 (Y scale / rotation).
  /// @param a12 Row 1, Column 2 (Y translation).
  /// @param a20 Row 2, Column 0 (projective).
  /// @param a21 Row 2, Column 1 (projective).
  /// @param a22 Row 2, Column 2 (projective).
  constexpr AffineTransform2D(T a00, T a01, T a02, T a10, T a11, T a12, T a20,
                              T a21, T a22)
      : matrix_{} {
    // Column 0
    matrix_[0] = a00;
    matrix_[1] = a10;
    matrix_[2] = T(0);
    matrix_[3] = a20;
    // Column 1
    matrix_[4] = a01;
    matrix_[5] = a11;
    matrix_[6] = T(0);
    matrix_[7] = a21;
    // Column 2
    matrix_[8] = T(0);
    matrix_[9] = T(0);
    matrix_[10] = T(1);
    matrix_[11] = T(0);
    // Column 3
    matrix_[12] = a02;
    matrix_[13] = a12;
    matrix_[14] = T(0);
    matrix_[15] = a22;
  }

  /// @brief Returns a pointer to the internal column-major 4x4 matrix data.
  ///
  /// The data is suitable for uploading directly to a GPU.
  ///
  /// @return Pointer to 16 elements in column-major order.
  constexpr const T* GetMatrix() const { return matrix_; }

  /// @brief Compute the inverse of this transform.
  ///
  /// Uses the 3x3 determinant to invert the affine portion.
  ///
  /// @return The inverse transform.
  inline AffineTransform2D GetInverse() const {
    // Extract the 3x3 row-major elements.
    T a00 = matrix_[0];
    T a01 = matrix_[4];
    T a02 = matrix_[12];
    T a10 = matrix_[1];
    T a11 = matrix_[5];
    T a12 = matrix_[13];
    T a20 = matrix_[3];
    T a21 = matrix_[7];
    T a22 = matrix_[15];

    T det = a00 * (a11 * a22 - a12 * a21) - a01 * (a10 * a22 - a12 * a20)
            + a02 * (a10 * a21 - a11 * a20);
    T inv_det = T(1) / det;

    return AffineTransform2D(
        (a11 * a22 - a12 * a21) * inv_det, (a02 * a21 - a01 * a22) * inv_det,
        (a01 * a12 - a02 * a11) * inv_det, (a12 * a20 - a10 * a22) * inv_det,
        (a00 * a22 - a02 * a20) * inv_det, (a02 * a10 - a00 * a12) * inv_det,
        (a10 * a21 - a11 * a20) * inv_det, (a01 * a20 - a00 * a21) * inv_det,
        (a00 * a11 - a01 * a10) * inv_det);
  }

  /// @brief Transform a 2D point by this affine transform.
  ///
  /// @param point The point to transform.
  /// @return The transformed point.
  constexpr Vector<T, 2> TransformPoint(const Vector<T, 2>& point) const {
    return Vector<T, 2>(
        matrix_[0] * point.x + matrix_[4] * point.y + matrix_[12],
        matrix_[1] * point.x + matrix_[5] * point.y + matrix_[13]);
  }

  /// @brief Transform a rect and return the axis-aligned bounding box.
  ///
  /// All four corners of the input rect are transformed, and the AABB
  /// enclosing all four transformed points is returned.
  ///
  /// @param rect The rect to transform.
  /// @return The axis-aligned bounding box of the transformed rect.
  constexpr Rect<T> TransformRect(const Rect<T>& rect) const {
    Vector<T, 2> p0 = TransformPoint(rect.pos);
    Vector<T, 2> p1 =
        TransformPoint(Vector<T, 2>(rect.pos.x + rect.size.x, rect.pos.y));
    Vector<T, 2> p2 =
        TransformPoint(Vector<T, 2>(rect.pos.x, rect.pos.y + rect.size.y));
    Vector<T, 2> p3 = TransformPoint(
        Vector<T, 2>(rect.pos.x + rect.size.x, rect.pos.y + rect.size.y));

    T min_x = std::min({p0.x, p1.x, p2.x, p3.x});
    T min_y = std::min({p0.y, p1.y, p2.y, p3.y});
    T max_x = std::max({p0.x, p1.x, p2.x, p3.x});
    T max_y = std::max({p0.y, p1.y, p2.y, p3.y});

    return Rect<T>(min_x, min_y, max_x - min_x, max_y - min_y);
  }

  /// @brief Apply a translation to this transform.
  ///
  /// @param offset Translation vector.
  /// @return Reference to this transform for chaining.
  constexpr AffineTransform2D& Translate(const Vector<T, 2>& offset) {
    return Translate(offset.x, offset.y);
  }

  /// @brief Apply a translation to this transform.
  ///
  /// @param x Translation along the X axis.
  /// @param y Translation along the Y axis.
  /// @return Reference to this transform for chaining.
  constexpr AffineTransform2D& Translate(T x, T y) {
    AffineTransform2D translation(T(1), T(0), x, T(0), T(1), y, T(0), T(0),
                                  T(1));
    return Combine(translation);
  }

  /// @brief Apply a rotation to this transform.
  ///
  /// @param angle_degrees Rotation angle in degrees (counter-clockwise).
  /// @return Reference to this transform for chaining.
  inline AffineTransform2D& Rotate(T angle_degrees) {
    T radians = angle_degrees * std::numbers::pi_v<T> / T(180);
    T cos_a = std::cos(radians);
    T sin_a = std::sin(radians);
    AffineTransform2D rotation(cos_a, -sin_a, T(0), sin_a, cos_a, T(0), T(0),
                               T(0), T(1));
    return Combine(rotation);
  }

  /// @brief Apply a rotation around a center point to this transform.
  ///
  /// @param angle_degrees Rotation angle in degrees (counter-clockwise).
  /// @param center The center of rotation.
  /// @return Reference to this transform for chaining.
  inline AffineTransform2D& Rotate(T angle_degrees,
                                   const Vector<T, 2>& center) {
    Translate(center.x, center.y);
    Rotate(angle_degrees);
    return Translate(-center.x, -center.y);
  }

  /// @brief Apply a scale to this transform.
  ///
  /// @param factors Scale factors for each axis.
  /// @return Reference to this transform for chaining.
  constexpr AffineTransform2D& Scale(const Vector<T, 2>& factors) {
    return Scale(factors.x, factors.y);
  }

  /// @brief Apply a scale around a center point to this transform.
  ///
  /// @param factors Scale factors for each axis.
  /// @param center The center of scaling.
  /// @return Reference to this transform for chaining.
  constexpr AffineTransform2D& Scale(const Vector<T, 2>& factors,
                                     const Vector<T, 2>& center) {
    return Scale(factors.x, factors.y, center);
  }

  /// @brief Apply a scale to this transform.
  ///
  /// @param sx Scale factor along the X axis.
  /// @param sy Scale factor along the Y axis.
  /// @return Reference to this transform for chaining.
  constexpr AffineTransform2D& Scale(T sx, T sy) {
    AffineTransform2D scaling(sx, T(0), T(0), T(0), sy, T(0), T(0), T(0), T(1));
    return Combine(scaling);
  }

  /// @brief Apply a scale around a center point to this transform.
  ///
  /// @param sx Scale factor along the X axis.
  /// @param sy Scale factor along the Y axis.
  /// @param center The center of scaling.
  /// @return Reference to this transform for chaining.
  constexpr AffineTransform2D& Scale(T sx, T sy, const Vector<T, 2>& center) {
    Translate(center.x, center.y);
    Scale(sx, sy);
    return Translate(-center.x, -center.y);
  }

  /// @brief Combine this transform with another transform.
  ///
  /// The other transform is applied after this transform. This is equivalent
  /// to multiplying the matrices: this = this * other.
  ///
  /// @param other The transform to combine with.
  /// @return Reference to this transform for chaining.
  constexpr AffineTransform2D& Combine(const AffineTransform2D& other) {
    const T* a = matrix_;
    const T* b = other.matrix_;

    // Multiply the two 4x4 column-major matrices, but since we know the
    // structure (row 2 is [0,0,1,0], certain elements are always 0 or 1),
    // we only need to compute the relevant elements of the 3x3 affine
    // portion embedded in the 4x4.
    T new00 = a[0] * b[0] + a[4] * b[1];
    T new01 = a[0] * b[4] + a[4] * b[5];
    T new02 = a[0] * b[12] + a[4] * b[13] + a[12];
    T new10 = a[1] * b[0] + a[5] * b[1];
    T new11 = a[1] * b[4] + a[5] * b[5];
    T new12 = a[1] * b[12] + a[5] * b[13] + a[13];

    // For a standard affine transform, the bottom row is [0, 0, 1].
    // We preserve it through multiplication.
    T new20 = a[3] * b[0] + a[7] * b[1] + a[15] * b[3];
    T new21 = a[3] * b[4] + a[7] * b[5] + a[15] * b[7];
    T new22 = a[3] * b[12] + a[7] * b[13] + a[15] * b[15];

    matrix_[0] = new00;
    matrix_[1] = new10;
    matrix_[4] = new01;
    matrix_[5] = new11;
    matrix_[12] = new02;
    matrix_[13] = new12;
    matrix_[3] = new20;
    matrix_[7] = new21;
    matrix_[15] = new22;

    return *this;
  }

  /// @brief Returns an identity transform.
  ///
  /// @return An AffineTransform2D representing the identity transformation.
  static constexpr AffineTransform2D Identity() { return AffineTransform2D(); }

 private:
  T matrix_[16];
};
/// @}

/// @brief Check if two 2D affine transforms are identical.
///
/// @param t1 Transform to be tested.
/// @param t2 Other transform to be tested.
template <class T>
constexpr bool operator==(const AffineTransform2D<T>& t1,
                          const AffineTransform2D<T>& t2) {
  const T* a = t1.GetMatrix();
  const T* b = t2.GetMatrix();
  for (int i = 0; i < 16; ++i) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

/// @brief Check if two 2D affine transforms are <b>not</b> identical.
///
/// @param t1 Transform to be tested.
/// @param t2 Other transform to be tested.
template <class T>
constexpr bool operator!=(const AffineTransform2D<T>& t1,
                          const AffineTransform2D<T>& t2) {
  return !(t1 == t2);
}

/// @brief Compose two 2D affine transforms.
///
/// @param lhs Left-hand side transform (applied second).
/// @param rhs Right-hand side transform (applied first).
/// @return The composed transform.
template <class T>
constexpr AffineTransform2D<T> operator*(const AffineTransform2D<T>& lhs,
                                         const AffineTransform2D<T>& rhs) {
  AffineTransform2D<T> result = lhs;
  result.Combine(rhs);
  return result;
}

/// @brief Compose two 2D affine transforms in-place.
///
/// @param lhs Left-hand side transform, modified in-place.
/// @param rhs Right-hand side transform (applied first).
/// @return Reference to the modified left-hand side transform.
template <class T>
constexpr AffineTransform2D<T>& operator*=(AffineTransform2D<T>& lhs,
                                           const AffineTransform2D<T>& rhs) {
  return lhs.Combine(rhs);
}

}  // namespace mathkata

#endif  // MATHKATA_AFFINE_TRANSFORM_2D_H_

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
#ifndef MATHKATA_TRANSFORM_H_
#define MATHKATA_TRANSFORM_H_

#include "mathkata/matrix.h"
#include "mathkata/quaternion.h"
#include "mathkata/vector.h"

/// @file mathkata/transform.h Transform
/// @brief Transform class and functions.
/// @addtogroup mathkata_transform

namespace mathkata {

/// @addtogroup mathkata_transform
/// @{
/// @class Transform "mathkata/transform.h"
/// @brief Represents a 3D rigid body transform as position, rotation
/// (quaternion), and scale.
///
/// Transform stores a position (Vector<T, 3>), rotation (Quaternion<T>),
/// and scale (Vector<T, 3>) and provides operations for composing,
/// inverting, and converting to a 4x4 matrix.
///
/// @tparam T type of Transform elements.
template <class T>
struct Transform {
  /// @brief Position component.
  Vector<T, 3> position;

  /// @brief Rotation component (unit quaternion).
  Quaternion<T> rotation;

  /// @brief Scale component.
  Vector<T, 3> scale;

  /// @brief Create an identity transform.
  ///
  /// Position is zero, rotation is identity, scale is one.
  inline Transform()
      : position(T(0), T(0), T(0)),
        rotation(Quaternion<T>::identity),
        scale(T(1), T(1), T(1)) {}

  /// @brief Create a transform from position, rotation, and scale.
  ///
  /// @param position Position vector.
  /// @param rotation Rotation quaternion.
  /// @param scale Scale vector.
  constexpr Transform(const Vector<T, 3>& position,
                      const Quaternion<T>& rotation, const Vector<T, 3>& scale)
      : position(position), rotation(rotation), scale(scale) {}

  /// @brief Create a transform from position only.
  ///
  /// Rotation is identity and scale is one.
  ///
  /// @param position Position vector.
  explicit inline Transform(const Vector<T, 3>& position)
      : position(position),
        rotation(Quaternion<T>::identity),
        scale(T(1), T(1), T(1)) {}

  /// @brief Create a transform from position and rotation.
  ///
  /// Scale is one.
  ///
  /// @param position Position vector.
  /// @param rotation Rotation quaternion.
  constexpr Transform(const Vector<T, 3>& position,
                      const Quaternion<T>& rotation)
      : position(position), rotation(rotation), scale(T(1), T(1), T(1)) {}

  /// @brief Convert this transform to a 4x4 matrix.
  ///
  /// The matrix is constructed in TRS order (scale, then rotate, then
  /// translate), which is the standard convention for 3D transforms.
  ///
  /// @return 4x4 transformation matrix.
  inline Matrix<T, 4> ToMatrix() const {
    return Matrix<T, 4>::Transform(position, rotation.ToMatrix(), scale);
  }

  /// @brief Apply the full transform to a point.
  ///
  /// The point is first scaled, then rotated, then translated.
  ///
  /// @param point The point to transform.
  /// @return The transformed point.
  constexpr Vector<T, 3> TransformPoint(const Vector<T, 3>& point) const {
    return rotation.Rotate(Vector<T, 3>(point.x * scale.x, point.y * scale.y,
                                        point.z * scale.z))
           + position;
  }

  /// @brief Apply rotation and scale to a direction vector.
  ///
  /// Translation is not applied.
  ///
  /// @param direction The direction vector to transform.
  /// @return The transformed direction.
  constexpr Vector<T, 3> TransformDirection(
      const Vector<T, 3>& direction) const {
    return rotation.Rotate(Vector<T, 3>(
        direction.x * scale.x, direction.y * scale.y, direction.z * scale.z));
  }

  /// @brief Compute the inverse of this transform.
  ///
  /// The inverse satisfies:
  /// <code>t.Inverse().TransformPoint(t.TransformPoint(v)) == v</code>
  ///
  /// @note Composition and inversion are exact for uniform scale. With
  /// non-uniform scale combined with rotation, a small amount of error
  /// is introduced because TRS decomposition is lossy for sheared matrices.
  ///
  /// @return The inverse transform.
  constexpr Transform<T> Inverse() const {
    Quaternion<T> inv_rotation = rotation.Inverse();
    Vector<T, 3> inv_scale(T(1) / scale.x, T(1) / scale.y, T(1) / scale.z);
    // The inverse of T*R*S applied to a point:
    //   forward:  r * (s .* v) + p
    //   inverse:  (1/s) .* (r^{-1} * (v - p))
    //
    // In TRS form with r' = r^{-1} and s' = 1/s:
    //   r' * (s' .* v) + p'
    //
    // Position: p' = r' * (s' .* (-p)) = r^{-1} * ((1/s) .* (-p))
    Vector<T, 3> neg_p(-position.x, -position.y, -position.z);
    Vector<T, 3> scaled_neg_p(neg_p.x * inv_scale.x, neg_p.y * inv_scale.y,
                              neg_p.z * inv_scale.z);
    Vector<T, 3> inv_position = inv_rotation.Rotate(scaled_neg_p);
    return Transform<T>(inv_position, inv_rotation, inv_scale);
  }

  /// @brief Compose this transform with another transform.
  ///
  /// The right-hand side transform is applied first, then this transform.
  ///
  /// @note Composition is exact for uniform scale. With non-uniform scale
  /// combined with rotation, a small amount of error is introduced because
  /// TRS decomposition is lossy for sheared matrices.
  ///
  /// @param rhs Transform to compose with.
  /// @return The composed transform.
  constexpr Transform<T> operator*(const Transform<T>& rhs) const {
    // Composing T1*R1*S1 with T2*R2*S2:
    //   position: r1 * (s1 .* rhs.position) + p1
    //   rotation: r1 * r2
    //   scale:    s1 .* s2
    Vector<T, 3> scaled_rhs_pos(rhs.position.x * scale.x,
                                rhs.position.y * scale.y,
                                rhs.position.z * scale.z);
    Vector<T, 3> new_position = rotation.Rotate(scaled_rhs_pos) + position;
    Quaternion<T> new_rotation = rotation * rhs.rotation;
    Vector<T, 3> new_scale(scale.x * rhs.scale.x, scale.y * rhs.scale.y,
                           scale.z * rhs.scale.z);
    return Transform<T>(new_position, new_rotation, new_scale);
  }

  /// @brief Returns the identity transform.
  ///
  /// @return Transform with zero position, identity rotation, and unit scale.
  static inline Transform<T> Identity() { return Transform<T>(); }
};

/// @brief Check if two transforms are identical.
///
/// @param t1 Transform to be tested.
/// @param t2 Other transform to be tested.
template <class T>
constexpr bool operator==(const Transform<T>& t1, const Transform<T>& t2) {
  return t1.position == t2.position && t1.rotation == t2.rotation
         && t1.scale == t2.scale;
}

/// @brief Check if two transforms are not identical.
///
/// @param t1 Transform to be tested.
/// @param t2 Other transform to be tested.
template <class T>
constexpr bool operator!=(const Transform<T>& t1, const Transform<T>& t2) {
  return !(t1 == t2);
}
/// @}

}  // namespace mathkata

#endif  // MATHKATA_TRANSFORM_H_

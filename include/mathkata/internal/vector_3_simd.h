/*
 * Copyright 2014 Google Inc. All rights reserved.
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
#ifndef MATHKATA_VECTOR_3_SIMD_H_
#define MATHKATA_VECTOR_3_SIMD_H_

#include <cmath>

#include "mathkata/internal/vector_3.h"
#include "mathkata/utilities.h"

#ifdef MATHKATA_COMPILE_WITH_SIMD
#include "mathkata/internal/simd_helpers.h"
#endif

/// @file mathkata/internal/vector_3_simd.h MathKata Vector<T, 3> Specialization
/// @brief 3-dimensional specialization of mathkata::Vector for SIMD optimized
/// builds.
/// @see mathkata::Vector

/// @cond MATHKATA_INTERNAL
/// Add macros to account for both the case where the vector is stored as a
/// simd intrinsic using 4 elements or as 3 values of type T.
/// MATHKATA_VECTOR3_STORE3/MATHKATA_VECTOR3_LOAD3 are additional operations
/// used to load/store the non simd values from and to simd datatypes. If
/// intrinsics are used these amount to essentially noops.
/// MATHKATA_VECTOR3_INIT3 either creates a simd datatype if the intrinsic is
/// used or sets the T values if not.
#ifdef MATHKATA_COMPILE_WITH_PADDING
#define MATHKATA_VECTOR3_STORE3(simd_to_store, data) \
  {                                                  \
    (data).simd3 = simd4f_zero_w(simd_to_store);     \
  }
#define MATHKATA_VECTOR3_LOAD3(data) (data).simd3
#define MATHKATA_VECTOR3_INIT3(data, v1, v2, v3) \
  {                                              \
    (data).simd3 = simd4f_create(v1, v2, v3, 0); \
  }
#else
#define MATHKATA_VECTOR3_STORE3(simd_to_store, data) \
  {                                                  \
    simd4f_ustore3(simd_to_store, (data).data_);     \
  }
#define MATHKATA_VECTOR3_LOAD3(data) simd4f_uload3((data).data_)
#define MATHKATA_VECTOR3_INIT3(data, v1, v2, v3) \
  {                                              \
    (data).data_[0] = v1;                        \
    (data).data_[1] = v2;                        \
    (data).data_[2] = v3;                        \
  }
#endif  // MATHKATA_COMPILE_WITH_PADDING
/// @endcond

namespace mathkata {

#ifdef MATHKATA_COMPILE_WITH_SIMD
/// @cond MATHKATA_INTERNAL
// This class should remain plain old data.
template <>
class Vector<float, 3> {
 public:
  typedef float Scalar;
  static constexpr int kDims = 3;

  /// @brief Create a Vector without assigning its elements.
  ///
  /// Default-initialization (Vector<float, 3> v;) leaves the elements
  /// indeterminate; value-initialization (Vector<float, 3> v{}) zeroes the
  /// elements. Under MATHKATA_COMPILE_WITH_PADDING the zeroing covers the
  /// fourth lane as well, so simd3 reads as four zeroes.
  Vector() = default;

  inline Vector(const Vector<float, 3>& v) {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    simd3 = v.simd3;
#else
    MATHKATA_VECTOR3_INIT3(*this, v[0], v[1], v[2])
#endif  // MATHKATA_COMPILE_WITH_PADDING
  }

  inline Vector<float, 3>& operator=(const Vector<float, 3>& v) {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    simd3 = v.simd3;
#else
    MATHKATA_VECTOR3_INIT3(*this, v[0], v[1], v[2])
#endif  // MATHKATA_COMPILE_WITH_PADDING
    return *this;
  }

  explicit inline Vector(const Vector<int, 3>& v) {
    MATHKATA_VECTOR3_INIT3(*this, static_cast<float>(v[0]),
                           static_cast<float>(v[1]), static_cast<float>(v[2]))
  }

  explicit inline Vector(const simd4f& v) { MATHKATA_VECTOR3_STORE3(v, *this) }

  explicit inline Vector(const float& s) {
    MATHKATA_VECTOR3_INIT3(*this, s, s, s)
  }

  inline Vector(const float& v1, const float& v2, const float& v3) {
    MATHKATA_VECTOR3_INIT3(*this, v1, v2, v3)
  }

  inline Vector(const Vector<float, 2>& v12, const float& v3) {
    MATHKATA_VECTOR3_INIT3(*this, v12[0], v12[1], v3)
  }

  explicit inline Vector(const float* v) {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    simd3 = simd4f_uload3(v);
#else
    MATHKATA_VECTOR3_INIT3(*this, v[0], v[1], v[2])
#endif  // MATHKATA_COMPILE_WITH_PADDING
  }

  explicit inline Vector(const VectorPacked<float, 3>& vector) {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    simd3 = simd4f_uload3(vector.data_);
#else
    MATHKATA_VECTOR3_INIT3(*this, vector.data_[0], vector.data_[1],
                           vector.data_[2])
#endif  // MATHKATA_COMPILE_WITH_PADDING
  }

  inline float& operator()(const int i) { return data_[i]; }

  inline const float& operator()(const int i) const { return data_[i]; }

  inline float& operator[](const int i) { return data_[i]; }

  inline const float& operator[](const int i) const { return data_[i]; }

  /// GLSL style multi-component accessors.
  inline Vector<float, 3> xyz() { return Vector<float, 3>(x, y, z); }
  inline const Vector<float, 3> xyz() const {
    return Vector<float, 3>(x, y, z);
  }

  inline Vector<float, 2> xy() { return Vector<float, 2>(x, y); }
  inline const Vector<float, 2> xy() const { return Vector<float, 2>(x, y); }

  inline void pack(VectorPacked<float, 3>* const vector) const {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    simd4f_ustore3(simd3, vector->data_);
#else
    vector->data_[0] = data_[0];
    vector->data_[1] = data_[1];
    vector->data_[2] = data_[2];
#endif  // MATHKATA_COMPILE_WITH_PADDING
  }

  inline Vector<float, 3> operator-() const {
    return Vector<float, 3>(
        simd4f_sub(simd4f_zero(), MATHKATA_VECTOR3_LOAD3(*this)));
  }

  inline Vector<float, 3> operator+(const Vector<float, 3>& v) const {
    return Vector<float, 3>(
        simd4f_add(MATHKATA_VECTOR3_LOAD3(*this), MATHKATA_VECTOR3_LOAD3(v)));
  }

  inline Vector<float, 3> operator-(const Vector<float, 3>& v) const {
    return Vector<float, 3>(
        simd4f_sub(MATHKATA_VECTOR3_LOAD3(*this), MATHKATA_VECTOR3_LOAD3(v)));
  }

  inline Vector<float, 3> operator*(const float& s) const {
    return Vector<float, 3>(
        simd4f_mul(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(s)));
  }

  friend inline Vector<float, 3> operator*(const float& s,
                                           const Vector<float, 3>& v) {
    return Vector<float, 3>(
        simd4f_mul(simd4f_splat(s), MATHKATA_VECTOR3_LOAD3(v)));
  }

  inline Vector<float, 3> operator/(const float& s) const {
    return Vector<float, 3>(
        simd4f_div(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(s)));
  }

  inline Vector<float, 3> operator+(const float& s) const {
    return Vector<float, 3>(
        simd4f_add(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(s)));
  }

  friend inline Vector<float, 3> operator+(const float& s,
                                           const Vector<float, 3>& v) {
    return Vector<float, 3>(
        simd4f_add(simd4f_splat(s), MATHKATA_VECTOR3_LOAD3(v)));
  }

  inline Vector<float, 3> operator-(const float& s) const {
    return Vector<float, 3>(
        simd4f_sub(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(s)));
  }

  friend inline Vector<float, 3> operator-(const float& s,
                                           const Vector<float, 3>& v) {
    return Vector<float, 3>(
        simd4f_sub(simd4f_splat(s), MATHKATA_VECTOR3_LOAD3(v)));
  }

  inline Vector<float, 3>& operator+=(const Vector<float, 3>& v) {
    MATHKATA_VECTOR3_STORE3(
        simd4f_add(MATHKATA_VECTOR3_LOAD3(*this), MATHKATA_VECTOR3_LOAD3(v)),
        *this)
    return *this;
  }

  inline Vector<float, 3>& operator-=(const Vector<float, 3>& v) {
    MATHKATA_VECTOR3_STORE3(
        simd4f_sub(MATHKATA_VECTOR3_LOAD3(*this), MATHKATA_VECTOR3_LOAD3(v)),
        *this)
    return *this;
  }

  inline Vector<float, 3>& operator*=(const float& s) {
    MATHKATA_VECTOR3_STORE3(
        simd4f_mul(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(s)), *this)
    return *this;
  }

  inline Vector<float, 3>& operator/=(const float& s) {
    MATHKATA_VECTOR3_STORE3(
        simd4f_div(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(s)), *this)
    return *this;
  }

  inline Vector<float, 3>& operator+=(const float& s) {
    MATHKATA_VECTOR3_STORE3(
        simd4f_add(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(s)), *this)
    return *this;
  }

  inline Vector<float, 3>& operator-=(const float& s) {
    MATHKATA_VECTOR3_STORE3(
        simd4f_sub(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(s)), *this)
    return *this;
  }

  inline bool operator==(const Vector<float, 3>& v) const {
    for (int i = 0; i < 3; ++i) {
      if ((*this)[i] != v[i]) return false;
    }
    return true;
  }

  inline bool operator!=(const Vector<float, 3>& v) const {
    return !operator==(v);
  }

  inline float lengthSquared() const {
    return simd4f_dot3_scalar(MATHKATA_VECTOR3_LOAD3(*this),
                              MATHKATA_VECTOR3_LOAD3(*this));
  }

  inline float length() const {
    return simd4f_get_x(simd4f_length3(MATHKATA_VECTOR3_LOAD3(*this)));
  }

  /// @brief normalize this vector in-place.
  ///
  /// The vector must have non-zero length. Normalizing a zero-length vector
  /// produces undefined results.
  ///
  /// @return The length of this vector.
  inline float normalize() {
    const float len = length();
    MATHKATA_VECTOR3_STORE3(
        simd4f_mul(MATHKATA_VECTOR3_LOAD3(*this), simd4f_splat(1 / len)), *this)
    return len;
  }

  /// @brief Calculate the normalized version of this vector.
  ///
  /// The vector must have non-zero length. Normalizing a zero-length vector
  /// produces undefined results.
  ///
  /// @return The normalized vector.
  inline Vector<float, 3> normalized() const {
    return Vector<float, 3>(simd4f_normalize3(MATHKATA_VECTOR3_LOAD3(*this)));
  }

  template <typename CompatibleT>
  static inline Vector<float, 3> fromType(const CompatibleT& compatible) {
    return fromTypeHelper<float, 3, CompatibleT>(compatible);
  }

  template <typename CompatibleT>
  static inline CompatibleT toType(const Vector<float, 3>& v) {
    return toTypeHelper<float, 3, CompatibleT>(v);
  }

  static inline float dotProduct(const Vector<float, 3>& v1,
                                 const Vector<float, 3>& v2) {
    return simd4f_dot3_scalar(MATHKATA_VECTOR3_LOAD3(v1),
                              MATHKATA_VECTOR3_LOAD3(v2));
  }

  static inline Vector<float, 3> crossProduct(const Vector<float, 3>& v1,
                                              const Vector<float, 3>& v2) {
    return Vector<float, 3>(
        simd4f_cross3(MATHKATA_VECTOR3_LOAD3(v1), MATHKATA_VECTOR3_LOAD3(v2)));
  }

  static inline Vector<float, 3> hadamardProduct(const Vector<float, 3>& v1,
                                                 const Vector<float, 3>& v2) {
    return Vector<float, 3>(
        simd4f_mul(MATHKATA_VECTOR3_LOAD3(v1), MATHKATA_VECTOR3_LOAD3(v2)));
  }

  static inline Vector<float, 3> hadamardDivide(const Vector<float, 3>& v1,
                                                const Vector<float, 3>& v2) {
    return Vector<float, 3>(
        simd4f_div(MATHKATA_VECTOR3_LOAD3(v1), MATHKATA_VECTOR3_LOAD3(v2)));
  }

  static inline Vector<float, 3> lerp(const Vector<float, 3>& v1,
                                      const Vector<float, 3>& v2,
                                      float percent) {
    const simd4f percentv = simd4f_splat(percent);
    const simd4f v1s = MATHKATA_VECTOR3_LOAD3(v1);
    const simd4f v2s = MATHKATA_VECTOR3_LOAD3(v2);
    return Vector<float, 3>(
        simd4f_add(v1s, simd4f_mul(simd4f_sub(v2s, v1s), percentv)));
  }

  static inline bool inRange(const Vector<float, 3>& val,
                             const Vector<float, 3>& range_start,
                             const Vector<float, 3>& range_end) {
    return inRangeHelper(val, range_start, range_end);
  }

  static inline Vector<float, 3> max(const Vector<float, 3>& v1,
                                     const Vector<float, 3>& v2) {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    return Vector<float, 3>(
        simd4f_max(MATHKATA_VECTOR3_LOAD3(v1), MATHKATA_VECTOR3_LOAD3(v2)));
#else
    return Vector<float, 3>(std::max(v1[0], v2[0]), std::max(v1[1], v2[1]),
                            std::max(v1[2], v2[2]));
#endif  // MATHKATA_COMPILE_WITH_PADDING
  }

  static inline Vector<float, 3> min(const Vector<float, 3>& v1,
                                     const Vector<float, 3>& v2) {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    return Vector<float, 3>(
        simd4f_min(MATHKATA_VECTOR3_LOAD3(v1), MATHKATA_VECTOR3_LOAD3(v2)));
#else
    return Vector<float, 3>(std::min(v1[0], v2[0]), std::min(v1[1], v2[1]),
                            std::min(v1[2], v2[2]));
#endif  // MATHKATA_COMPILE_WITH_PADDING
  }

  static inline float distance(const Vector<float, 3>& v1,
                               const Vector<float, 3>& v2) {
    return (v1 - v2).length();
  }

  static inline float distanceSquared(const Vector<float, 3>& v1,
                                      const Vector<float, 3>& v2) {
    return (v1 - v2).lengthSquared();
  }

  static inline float angle(const Vector<float, 3>& v1,
                            const Vector<float, 3>& v2) {
    return angleHelper(v1, v2);
  }

  static inline Vector<float, 3> project(const Vector<float, 3>& v,
                                         const Vector<float, 3>& onto) {
    return projectHelper(v, onto);
  }

  static inline Vector<float, 3> reject(const Vector<float, 3>& v,
                                        const Vector<float, 3>& from) {
    return rejectHelper(v, from);
  }

  static inline Vector<float, 3> reflect(const Vector<float, 3>& incident,
                                         const Vector<float, 3>& normal) {
    return reflectHelper(incident, normal);
  }

  static inline Vector<float, 3> refract(const Vector<float, 3>& incident,
                                         const Vector<float, 3>& normal,
                                         float eta) {
    return refractHelper(incident, normal, eta);
  }

  MATHKATA_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

#include "mathkata/internal/disable_warnings_begin.h"
  union {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    simd4f simd3;
    float data_[4];
#else
    float data_[3];
#endif  // MATHKATA_COMPILE_WITH_PADDING

    struct {
      float x;
      float y;
      float z;
    };
  };
#include "mathkata/internal/disable_warnings_end.h"
};
/// @endcond
#endif  // MATHKATA_COMPILE_WITH_SIMD

}  // namespace mathkata

#endif  // MATHKATA_VECTOR_3_SIMD_H_

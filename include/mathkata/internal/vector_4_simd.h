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
#ifndef MATHKATA_VECTOR_4_SIMD_H_
#define MATHKATA_VECTOR_4_SIMD_H_

#include <cmath>

#include "mathkata/internal/vector_4.h"
#include "mathkata/utilities.h"

#ifdef MATHKATA_COMPILE_WITH_SIMD
#include "mathkata/internal/simd_helpers.h"
#endif

/// @file mathkata/internal/vector_4_simd.h MathKata Vector<T, 4> Specialization
/// @brief 4-dimensional specialization of mathkata::Vector for SIMD optimized
/// builds.
/// @see mathkata::Vector

namespace mathkata {

#ifdef MATHKATA_COMPILE_WITH_SIMD

template <>
class Vector<float, 4> {
 public:
  typedef float Scalar;
  static constexpr int kDims = 4;

 private:
  /// Selects the constructor that leaves the elements unassigned. Private,
  /// so uninitialized() is the only way to reach it.
  struct UninitializedTag {};

  /// @brief Create a Vector without assigning the elements.
  ///
  /// @param tag Unused; selects this constructor.
  explicit Vector(UninitializedTag tag) { static_cast<void>(tag); }

 public:
  /// @brief Deleted; give the elements, or call uninitialized() to skip
  ///        assigning them on purpose.
  Vector() = delete;

  /// @brief Create a Vector without assigning the elements.
  ///
  /// Reading any of the elements before assigning it is undefined behavior.
  /// This is for code that fills every one of them immediately, where zeroing
  /// first would be a wasted store.
  ///
  /// @return A Vector with indeterminate elements.
  static inline Vector<float, 4> uninitialized() {
    return Vector<float, 4>(UninitializedTag{});
  }

  inline Vector(const Vector<float, 4>& v) { simd4 = v.simd4; }

  inline Vector<float, 4>& operator=(const Vector<float, 4>& v) {
    simd4 = v.simd4;
    return *this;
  }

  explicit inline Vector(const Vector<int, 4>& v) {
    data_[0] = static_cast<float>(v[0]);
    data_[1] = static_cast<float>(v[1]);
    data_[2] = static_cast<float>(v[2]);
    data_[3] = static_cast<float>(v[3]);
  }

  explicit inline Vector(const simd4f& v) { simd4 = v; }

  explicit inline Vector(const float& s) { simd4 = simd4f_splat(s); }

  inline Vector(const float& s1, const float& s2, const float& s3,
                const float& s4) {
    simd4 = simd4f_create(s1, s2, s3, s4);
  }

  explicit inline Vector(const float* v) { simd4 = simd4f_uload4(v); }

  inline Vector(const Vector<float, 3>& vector3, const float& value) {
#ifdef MATHKATA_COMPILE_WITH_PADDING
    simd4 = vector3.simd3;
    (*this)[3] = value;
#else
    simd4 = simd4f_create(vector3[0], vector3[1], vector3[2], value);
#endif  // MATHKATA_COMPILE_WITH_PADDING
  }

  inline Vector(const Vector<float, 2>& vector12,
                const Vector<float, 2>& vector34) {
    simd4 = simd4f_create(vector12[0], vector12[1], vector34[0], vector34[1]);
  }

  explicit inline Vector(const VectorPacked<float, 4>& vector) {
    simd4 = simd4f_uload4(vector.data_);
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

  inline Vector<float, 2> zw() { return Vector<float, 2>(z, w); }
  inline const Vector<float, 2> zw() const { return Vector<float, 2>(z, w); }

  inline void pack(VectorPacked<float, 4>* const vector) const {
    simd4f_ustore4(simd4, vector->data_);
  }

  inline Vector<float, 4> operator-() const {
    return Vector<float, 4>(simd4f_sub(simd4f_zero(), simd4));
  }

  inline Vector<float, 4> operator+(const Vector<float, 4>& v) const {
    return Vector<float, 4>(simd4f_add(simd4, v.simd4));
  }

  inline Vector<float, 4> operator-(const Vector<float, 4>& v) const {
    return Vector<float, 4>(simd4f_sub(simd4, v.simd4));
  }

  inline Vector<float, 4> operator*(const float& s) const {
    return Vector<float, 4>(simd4f_mul(simd4, simd4f_splat(s)));
  }

  inline Vector<float, 4> operator/(const float& s) const {
    return Vector<float, 4>(simd4f_div(simd4, simd4f_splat(s)));
  }

  inline Vector<float, 4> operator+(const float& s) const {
    return Vector<float, 4>(simd4f_add(simd4, simd4f_splat(s)));
  }

  inline Vector<float, 4> operator-(const float& s) const {
    return Vector<float, 4>(simd4f_sub(simd4, simd4f_splat(s)));
  }

  inline Vector<float, 4>& operator+=(const Vector<float, 4>& v) {
    simd4 = simd4f_add(simd4, v.simd4);
    return *this;
  }

  inline Vector<float, 4>& operator-=(const Vector<float, 4>& v) {
    simd4 = simd4f_sub(simd4, v.simd4);
    return *this;
  }

  inline Vector<float, 4>& operator*=(const float& s) {
    simd4 = simd4f_mul(simd4, simd4f_splat(s));
    return *this;
  }

  inline Vector<float, 4>& operator/=(const float& s) {
    simd4 = simd4f_div(simd4, simd4f_splat(s));
    return *this;
  }

  inline Vector<float, 4>& operator+=(const float& s) {
    simd4 = simd4f_add(simd4, simd4f_splat(s));
    return *this;
  }

  inline Vector<float, 4>& operator-=(const float& s) {
    simd4 = simd4f_sub(simd4, simd4f_splat(s));
    return *this;
  }

  inline bool operator==(const Vector<float, 4>& v) const {
    for (int i = 0; i < 4; ++i) {
      if ((*this)[i] != v[i]) return false;
    }
    return true;
  }

  inline bool operator!=(const Vector<float, 4>& v) const {
    return !operator==(v);
  }

  inline float lengthSquared() const {
    return simd4f_get_x(simd4f_dot4(simd4, simd4));
  }

  inline float length() const { return simd4f_get_x(simd4f_length4(simd4)); }

  /// @brief normalize this vector in-place.
  ///
  /// The vector must have non-zero length. Normalizing a zero-length vector
  /// produces undefined results.
  ///
  /// @return The length of this vector.
  inline float normalize() {
    const float len = length();
    simd4 = simd4f_mul(simd4, simd4f_splat(1 / len));
    return len;
  }

  /// @brief Calculate the normalized version of this vector.
  ///
  /// The vector must have non-zero length. Normalizing a zero-length vector
  /// produces undefined results.
  ///
  /// @return The normalized vector.
  inline Vector<float, 4> normalized() const {
    return Vector<float, 4>(simd4f_normalize4(simd4));
  }

  template <typename CompatibleT>
  static inline Vector<float, 4> fromType(const CompatibleT& compatible) {
    return fromTypeHelper<float, 4, CompatibleT>(compatible);
  }

  template <typename CompatibleT>
  static inline CompatibleT toType(const Vector<float, 4>& v) {
    return toTypeHelper<float, 4, CompatibleT>(v);
  }

  static inline float dotProduct(const Vector<float, 4>& v1,
                                 const Vector<float, 4>& v2) {
    return simd4f_get_x(simd4f_dot4(v1.simd4, v2.simd4));
  }

  static inline Vector<float, 4> hadamardProduct(const Vector<float, 4>& v1,
                                                 const Vector<float, 4>& v2) {
    return Vector<float, 4>(simd4f_mul(v1.simd4, v2.simd4));
  }

  static inline Vector<float, 4> hadamardDivide(const Vector<float, 4>& v1,
                                                const Vector<float, 4>& v2) {
    return Vector<float, 4>(simd4f_div(v1.simd4, v2.simd4));
  }

  static inline Vector<float, 4> lerp(const Vector<float, 4>& v1,
                                      const Vector<float, 4>& v2,
                                      float percent) {
    const simd4f percentv = simd4f_splat(percent);
    return Vector<float, 4>(simd4f_add(
        v1.simd4, simd4f_mul(simd4f_sub(v2.simd4, v1.simd4), percentv)));
  }

  static inline bool inRange(const Vector<float, 4>& val,
                             const Vector<float, 4>& range_start,
                             const Vector<float, 4>& range_end) {
    return inRangeHelper(val, range_start, range_end);
  }

  static inline Vector<float, 4> max(const Vector<float, 4>& v1,
                                     const Vector<float, 4>& v2) {
    return Vector<float, 4>(simd4f_max(v1.simd4, v2.simd4));
  }

  static inline Vector<float, 4> min(const Vector<float, 4>& v1,
                                     const Vector<float, 4>& v2) {
    return Vector<float, 4>(simd4f_min(v1.simd4, v2.simd4));
  }

  static inline float distance(const Vector<float, 4>& v1,
                               const Vector<float, 4>& v2) {
    return (v1 - v2).length();
  }

  static inline float distanceSquared(const Vector<float, 4>& v1,
                                      const Vector<float, 4>& v2) {
    return (v1 - v2).lengthSquared();
  }

  static inline float angle(const Vector<float, 4>& v1,
                            const Vector<float, 4>& v2) {
    return angleHelper(v1, v2);
  }

  static inline Vector<float, 4> project(const Vector<float, 4>& v,
                                         const Vector<float, 4>& onto) {
    return projectHelper(v, onto);
  }

  static inline Vector<float, 4> reject(const Vector<float, 4>& v,
                                        const Vector<float, 4>& from) {
    return rejectHelper(v, from);
  }

  static inline Vector<float, 4> reflect(const Vector<float, 4>& incident,
                                         const Vector<float, 4>& normal) {
    return reflectHelper(incident, normal);
  }

  static inline Vector<float, 4> refract(const Vector<float, 4>& incident,
                                         const Vector<float, 4>& normal,
                                         float eta) {
    return refractHelper(incident, normal, eta);
  }

  MATHKATA_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

#include "mathkata/internal/disable_warnings_begin.h"
  union {
    simd4f simd4;
    float data_[4];
    struct {
      float x;
      float y;
      float z;
      float w;
    };
  };
#include "mathkata/internal/disable_warnings_end.h"
};
/// @endcond
#endif  // MATHKATA_COMPILE_WITH_SIMD

}  // namespace mathkata

#endif  // MATHKATA_VECTOR_4_SIMD_H_

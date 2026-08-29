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
#ifndef MATHKATA_INTERNAL_VECTOR_4_H_
#define MATHKATA_INTERNAL_VECTOR_4_H_

// Prefer including vector.h directly, since it includes specializations.
#include "mathkata/vector.h"

namespace mathkata {

template <class T>
class Vector<T, 4> {
 public:
  typedef T Scalar;
  static constexpr int Dims = 4;
  static constexpr int kDims = 4;

 private:
  /// Selects the constructor that leaves the elements unassigned. Private,
  /// so uninitialized() is the only way to reach it.
  struct UninitializedTag {};

  /// @brief Create a Vector without assigning the elements.
  ///
  /// @param tag Unused; selects this constructor.
  explicit constexpr Vector(UninitializedTag tag) { static_cast<void>(tag); }

 public:
  /// @brief Deleted; give the elements, or call uninitialized() to skip
  ///        assigning them on purpose.
  Vector() = delete;

  /// @brief Create a Vector without assigning the elements.
  ///
  /// Reading any of the elements before assigning it is undefined behavior.
  /// This is for code that fills every one of them immediately, where zeroing
  /// first would be a wasted store. It is constexpr so that constexpr functions
  /// can call it, but the result can never be a constant itself: reading an
  /// unassigned element in a constant expression is ill-formed.
  ///
  /// @return A Vector with indeterminate elements.
  static constexpr Vector<T, 4> uninitialized() {
    return Vector<T, 4>(UninitializedTag{});
  }

  constexpr Vector(const Vector<T, 4>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

  constexpr Vector<T, 4>& operator=(const Vector<T, 4>& v) = default;

  explicit constexpr Vector(const VectorPacked<T, 4>& v)
      : x(v.x), y(v.y), z(v.z), w(v.w) {}

  explicit constexpr Vector(const T* a) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}

  explicit constexpr Vector(T s) : x(s), y(s), z(s), w(s) {}

  constexpr Vector(T s1, T s2, T s3, T s4) : x(s1), y(s2), z(s3), w(s4) {}

  constexpr Vector(const Vector<T, 3>& v123, T s4)
      : x(v123.x), y(v123.y), z(v123.z), w(s4) {}

  constexpr Vector(const Vector<T, 2>& v12, const Vector<T, 2>& v34)
      : x(v12.x), y(v12.y), z(v34.x), w(v34.y) {}

  template <typename U>
  explicit constexpr Vector(const Vector<U, 4>& v)
      : x(static_cast<T>(v.x)),
        y(static_cast<T>(v.y)),
        z(static_cast<T>(v.z)),
        w(static_cast<T>(v.w)) {}

  constexpr T& operator()(const int i) { return data_[i]; }

  constexpr const T& operator()(const int i) const { return data_[i]; }

  constexpr T& operator[](const int i) { return data_[i]; }

  constexpr const T& operator[](const int i) const { return data_[i]; }

  constexpr Vector<T, 3> xyz() { return Vector<T, 3>(x, y, z); }

  constexpr const Vector<T, 3> xyz() const { return Vector<T, 3>(x, y, z); }

  constexpr Vector<T, 2> xy() { return Vector<T, 2>(x, y); }

  constexpr const Vector<T, 2> xy() const { return Vector<T, 2>(x, y); }

  constexpr Vector<T, 2> zw() { return Vector<T, 2>(z, w); }

  constexpr const Vector<T, 2> zw() const { return Vector<T, 2>(z, w); }

  constexpr void pack(VectorPacked<T, 4>* const vector) const {
    vector->x = x;
    vector->y = y;
    vector->z = z;
    vector->w = w;
  }

  constexpr T lengthSquared() const { return lengthSquaredHelper(*this); }

  inline T length() const { return lengthHelper(*this); }

  inline T normalize() { return normalizeHelper(*this); }

  inline Vector<T, 4> normalized() const { return normalizedHelper(*this); }

  template <typename CompatibleT>
  static inline Vector<T, 4> fromType(const CompatibleT& compatible) {
    return fromTypeHelper<T, Dims, CompatibleT>(compatible);
  }

  template <typename CompatibleT>
  static inline CompatibleT toType(const Vector<T, 4>& v) {
    return toTypeHelper<T, Dims, CompatibleT>(v);
  }

  static constexpr T dotProduct(const Vector<T, 4>& v1,
                                const Vector<T, 4>& v2) {
    return dotProductHelper(v1, v2);
  }

  static constexpr Vector<T, 4> hadamardProduct(const Vector<T, 4>& v1,
                                                const Vector<T, 4>& v2) {
    return hadamardProductHelper(v1, v2);
  }

  static constexpr Vector<T, 4> hadamardDivide(const Vector<T, 4>& v1,
                                               const Vector<T, 4>& v2) {
    return hadamardDivideHelper(v1, v2);
  }

  static constexpr Vector<T, 4> lerp(const Vector<T, 4>& v1,
                                     const Vector<T, 4>& v2, const T percent) {
    return lerpHelper(v1, v2, percent);
  }

  static constexpr bool inRange(const Vector<T, 4>& val,
                                const Vector<T, 4>& range_start,
                                const Vector<T, 4>& range_end) {
    return inRangeHelper(val, range_start, range_end);
  }

  static constexpr Vector<T, 4> max(const Vector<T, 4>& v1,
                                    const Vector<T, 4>& v2) {
    return maxHelper(v1, v2);
  }

  static constexpr Vector<T, 4> min(const Vector<T, 4>& v1,
                                    const Vector<T, 4>& v2) {
    return minHelper(v1, v2);
  }

  static inline T distance(const Vector<T, 4>& v1, const Vector<T, 4>& v2) {
    return (v1 - v2).length();
  }

  static constexpr T distanceSquared(const Vector<T, 4>& v1,
                                     const Vector<T, 4>& v2) {
    return (v1 - v2).lengthSquared();
  }

  static inline T angle(const Vector<T, 4>& v1, const Vector<T, 4>& v2) {
    return angleHelper(v1, v2);
  }

  static constexpr Vector<T, 4> project(const Vector<T, 4>& v,
                                        const Vector<T, 4>& onto) {
    return projectHelper(v, onto);
  }

  static constexpr Vector<T, 4> reject(const Vector<T, 4>& v,
                                       const Vector<T, 4>& from) {
    return rejectHelper(v, from);
  }

  static constexpr Vector<T, 4> reflect(const Vector<T, 4>& incident,
                                        const Vector<T, 4>& normal) {
    return reflectHelper(incident, normal);
  }

  static inline Vector<T, 4> refract(const Vector<T, 4>& incident,
                                     const Vector<T, 4>& normal, T eta) {
    return refractHelper(incident, normal, eta);
  }

  MATHKATA_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

#include "mathkata/internal/disable_warnings_begin.h"
  union {
    T data_[4];
    struct {
      T x;
      T y;
      T z;
      T w;
    };
  };
#include "mathkata/internal/disable_warnings_end.h"
};

template <class T>
struct VectorPacked<T, 4> {
 private:
  /// Selects the constructor that leaves the elements unassigned. Private,
  /// so uninitialized() is the only way to reach it.
  struct UninitializedTag {};

  /// @brief Create a VectorPacked without assigning the elements.
  ///
  /// @param tag Unused; selects this constructor.
  explicit VectorPacked(UninitializedTag tag) { static_cast<void>(tag); }

 public:
  /// @brief Deleted; give the elements, or call uninitialized() to skip
  ///        assigning them on purpose.
  VectorPacked() = delete;

  /// @brief Create a VectorPacked without assigning the elements.
  ///
  /// Reading any of the elements before assigning it is undefined behavior.
  /// This is for code that fills every one of them immediately, where zeroing
  /// first would be a wasted store.
  ///
  /// @return A VectorPacked with indeterminate elements.
  static inline VectorPacked<T, 4> uninitialized() {
    return VectorPacked<T, 4>(UninitializedTag{});
  }

  /// Create a VectorPacked from a Vector.
  ///
  /// Both VectorPacked and Vector must have the same number of dimensions.
  /// @param vector Vector to create the VectorPacked from.
  explicit VectorPacked(const Vector<T, 4>& vector) { vector.pack(this); }

  /// Copy a Vector to a VectorPacked.
  ///
  /// Both VectorPacked and Vector must have the same number of dimensions.
  /// @param vector Vector to copy to the VectorPacked.
  /// @returns A reference to this VectorPacked.
  VectorPacked& operator=(const Vector<T, 4>& vector) {
    vector.pack(this);
    return *this;
  }

#include "mathkata/internal/disable_warnings_begin.h"
  /// Elements of the packed vector one per dimension.
  union {
    T data_[4];
    struct {
      T x;
      T y;
      T z;
      T w;
    };
  };
#include "mathkata/internal/disable_warnings_end.h"
};

}  // namespace mathkata

#endif  // MATHKATA_INTERNAL_VECTOR_4_H_

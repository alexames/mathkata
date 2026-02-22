/*
 * Copyright 2016 Google Inc. All rights reserved.
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
#ifndef MATHKATA_INTERNAL_VECTOR_3_H_
#define MATHKATA_INTERNAL_VECTOR_3_H_

#include "mathkata/vector.h"

namespace mathkata {

template <class T>
class Vector<T, 3> {
 public:
  typedef T Scalar;
  static constexpr int Dims = 3;
  static constexpr int kDims = 3;

  constexpr Vector() {}

  constexpr Vector(const Vector<T, 3>& v) : x(v.x), y(v.y), z(v.z) {}

  constexpr Vector<T, 3>& operator=(const Vector<T, 3>& v) = default;

  explicit constexpr Vector(const VectorPacked<T, 3>& v)
      : x(v.x), y(v.y), z(v.z) {}

  explicit constexpr Vector(const T* a) : x(a[0]), y(a[1]), z(a[2]) {}

  explicit constexpr Vector(T s) : x(s), y(s), z(s) {}

  constexpr Vector(T s1, T s2, T s3) : x(s1), y(s2), z(s3) {}

  constexpr Vector(const Vector<T, 2>& v12, T s3) : x(v12.x), y(v12.y), z(s3) {}

  template <typename U>
  explicit constexpr Vector(const Vector<U, 3>& v)
      : x(static_cast<T>(v.x)),
        y(static_cast<T>(v.y)),
        z(static_cast<T>(v.z)) {}

  constexpr T& operator()(const int i) { return data_[i]; }

  constexpr const T& operator()(const int i) const { return data_[i]; }

  constexpr T& operator[](const int i) { return data_[i]; }

  constexpr const T& operator[](const int i) const { return data_[i]; }

  constexpr Vector<T, 3> xyz() { return Vector<T, 3>(x, y, z); }

  constexpr const Vector<T, 3> xyz() const { return Vector<T, 3>(x, y, z); }

  constexpr Vector<T, 2> xy() { return Vector<T, 2>(x, y); }

  constexpr const Vector<T, 2> xy() const { return Vector<T, 2>(x, y); }

  constexpr void Pack(VectorPacked<T, 3>* const vector) const {
    vector->x = x;
    vector->y = y;
    vector->z = z;
  }

  constexpr T LengthSquared() const { return LengthSquaredHelper(*this); }

  inline T Length() const { return LengthHelper(*this); }

  inline T Normalize() { return NormalizeHelper(*this); }

  inline Vector<T, 3> Normalized() const { return NormalizedHelper(*this); }

  template <typename CompatibleT>
  static inline Vector<T, 3> FromType(const CompatibleT& compatible) {
    return FromTypeHelper<T, Dims, CompatibleT>(compatible);
  }

  template <typename CompatibleT>
  static inline CompatibleT ToType(const Vector<T, 3>& v) {
    return ToTypeHelper<T, Dims, CompatibleT>(v);
  }

  static constexpr T DotProduct(const Vector<T, 3>& v1,
                                const Vector<T, 3>& v2) {
    return DotProductHelper(v1, v2);
  }

  static constexpr Vector<T, 3> HadamardProduct(const Vector<T, 3>& v1,
                                                const Vector<T, 3>& v2) {
    return HadamardProductHelper(v1, v2);
  }

  static constexpr Vector<T, 3> HadamardDivide(const Vector<T, 3>& v1,
                                               const Vector<T, 3>& v2) {
    return HadamardDivideHelper(v1, v2);
  }

  static constexpr Vector<T, 3> CrossProduct(const Vector<T, 3>& v1,
                                             const Vector<T, 3>& v2) {
    return CrossProductHelper(v1, v2);
  }

  static constexpr Vector<T, 3> Lerp(const Vector<T, 3>& v1,
                                     const Vector<T, 3>& v2, const T percent) {
    return LerpHelper(v1, v2, percent);
  }

  static constexpr bool InRange(const Vector<T, 3>& val,
                                const Vector<T, 3>& range_start,
                                const Vector<T, 3>& range_end) {
    return InRangeHelper(val, range_start, range_end);
  }

  static constexpr Vector<T, 3> Max(const Vector<T, 3>& v1,
                                    const Vector<T, 3>& v2) {
    return MaxHelper(v1, v2);
  }

  static constexpr Vector<T, 3> Min(const Vector<T, 3>& v1,
                                    const Vector<T, 3>& v2) {
    return MinHelper(v1, v2);
  }

  static inline T Distance(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
    return (v1 - v2).Length();
  }

  static constexpr T DistanceSquared(const Vector<T, 3>& v1,
                                     const Vector<T, 3>& v2) {
    return (v1 - v2).LengthSquared();
  }

  static inline T Angle(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
    return AngleHelper(v1, v2);
  }

  static constexpr Vector<T, 3> Project(const Vector<T, 3>& v,
                                        const Vector<T, 3>& onto) {
    return ProjectHelper(v, onto);
  }

  static constexpr Vector<T, 3> Reject(const Vector<T, 3>& v,
                                       const Vector<T, 3>& from) {
    return RejectHelper(v, from);
  }

  static constexpr Vector<T, 3> Reflect(const Vector<T, 3>& incident,
                                        const Vector<T, 3>& normal) {
    return ReflectHelper(incident, normal);
  }

  static inline Vector<T, 3> Refract(const Vector<T, 3>& incident,
                                     const Vector<T, 3>& normal, T eta) {
    return RefractHelper(incident, normal, eta);
  }

  MATHKATA_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

#include "mathkata/internal/disable_warnings_begin.h"
  union {
    T data_[3];
    struct {
      T x;
      T y;
      T z;
    };
  };
#include "mathkata/internal/disable_warnings_end.h"
};

template <class T>
struct VectorPacked<T, 3> {
  /// Create an uninitialized VectorPacked.
  VectorPacked() {}

  /// Create a VectorPacked from a Vector.
  ///
  /// Both VectorPacked and Vector must have the same number of dimensions.
  /// @param vector Vector to create the VectorPacked from.
  explicit VectorPacked(const Vector<T, 3>& vector) { vector.Pack(this); }

  /// Copy a Vector to a VectorPacked.
  ///
  /// Both VectorPacked and Vector must have the same number of dimensions.
  /// @param vector Vector to copy to the VectorPacked.
  /// @returns A reference to this VectorPacked.
  VectorPacked& operator=(const Vector<T, 3>& vector) {
    vector.Pack(this);
    return *this;
  }

#include "mathkata/internal/disable_warnings_begin.h"
  /// Elements of the packed vector one per dimension.
  union {
    T data_[3];
    struct {
      T x;
      T y;
      T z;
    };
  };
#include "mathkata/internal/disable_warnings_end.h"
};

}  //  namespace mathkata

#endif  // MATHKATA_INTERNAL_VECTOR_3_H_

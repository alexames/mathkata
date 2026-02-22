/*
 * Copyright 2014 Google Inc. All rights reserved.
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
#ifndef MATHKATA_QUATERNION_H_
#define MATHKATA_QUATERNION_H_

#include <cmath>
#include <limits>
#include <numbers>

#include "mathkata/matrix.h"
#include "mathkata/vector.h"

/// @file mathkata/quaternion.h
/// @brief Quaternion class and functions.
/// @addtogroup mathkata_quaternion
///
/// MathKata provides a Quaternion class that utilizes SIMD optimized
/// Matrix and Vector classes.

namespace mathkata {

/// @addtogroup mathkata_quaternion
/// @{
/// @class Quaternion
///
/// @brief Stores a Quaternion of type T and provides a set of utility
/// operations on each Quaternion.
/// @tparam T Type of each element in the Quaternion.
template <class T>
class Quaternion {
 public:
  /// @brief Construct an uninitialized Quaternion.
  ///
  /// The scalar and vector components of the Quaternion are left uninitialized
  /// and have indeterminate values. This is intentional for performance: use
  /// Quaternion(T, T, T, T) or Quaternion::identity if you need specific
  /// values.
  constexpr Quaternion() {}

  /// @brief Construct a Quaternion from a copy.
  /// @param q Quaternion to copy.
  constexpr Quaternion(const Quaternion<T>& q) : data_(q.data_) {}

  /// @brief Construct a Quaternion using scalar values to initialize each
  /// element.
  ///
  /// @param s1 Scalar component.
  /// @param qs1 First element of the Vector component.
  /// @param qs2 Second element of the Vector component.
  /// @param qs3 Third element of the Vector component.
  constexpr Quaternion(T s1, T qs1, T qs2, T qs3) : data_(s1, qs1, qs2, qs3) {}

  /// @brief Construct a quaternion from a scalar and 3-dimensional Vector.
  ///
  /// @param s1 Scalar component.
  /// @param v1 Vector component.
  constexpr Quaternion(T s1, const Vector<T, 3>& v1)
      : data_(s1, v1[0], v1[1], v1[2]) {}

  /// @brief Return the scalar component of the quaternion.
  ///
  /// @return The scalar component
  constexpr T scalar() const { return data_[0]; }

  /// @brief Set the scalar component of the quaternion.
  ///
  /// @param s Scalar component.
  constexpr void set_scalar(T s) { data_[0] = s; }

  /// @brief Return the vector component of the quaternion.
  ///
  /// @return The vector component
  constexpr Vector<T, 3> vector() const {
    return Vector<T, 3>(data_[1], data_[2], data_[3]);
  }

  /// @brief Set the vector component of the quaternion.
  ///
  /// @param v Vector component.
  constexpr void set_vector(const Vector<T, 3>& v) {
    data_[1] = v[0];
    data_[2] = v[1];
    data_[3] = v[2];
  }

  /// @brief Calculate the conjugate of this Quaternion.
  ///
  /// The conjugate negates the vector component of the quaternion.
  /// For unit quaternions, the conjugate is equal to the inverse.
  ///
  /// @return Quaternion containing the result.
  constexpr Quaternion<T> Conjugate() const {
    return Quaternion<T>(data_[0], -data_[1], -data_[2], -data_[3]);
  }

  /// @brief Calculate the inverse Quaternion.
  ///
  /// This calculates the inverse such that
  /// <code>q * q.Inverse() == Quaternion::identity</code>
  /// for any non-zero quaternion.
  ///
  /// @return Quaternion containing the result.
  constexpr Quaternion<T> Inverse() const {
    T norm_sq = Vector<T, 4>::DotProduct(data_, data_);
    T inv_norm_sq = T(1) / norm_sq;
    return Quaternion<T>(data_[0] * inv_norm_sq, -data_[1] * inv_norm_sq,
                         -data_[2] * inv_norm_sq, -data_[3] * inv_norm_sq);
  }

  /// @brief Add this Quaternion to another Quaternion.
  ///
  /// @param q Quaternion to add.
  /// @return Quaternion containing the result.
  constexpr Quaternion<T> operator+(const Quaternion<T>& q) const {
    return Quaternion<T>(data_ + q.data_);
  }

  /// @brief Add another Quaternion to this, and store the result.
  ///
  /// @param q Quaternion to add.
  /// @return Quaternion containing the result.
  constexpr Quaternion<T>& operator+=(const Quaternion<T>& q) {
    data_ += q.data_;
    return *this;
  }

  /// @brief Compare two Quaternions for equality.
  ///
  /// @param q Quaternion to compare against.
  /// @return true if the scalar and vector components are equal.
  constexpr bool operator==(const Quaternion<T>& q) const {
    return data_ == q.data_;
  }

  /// @brief Compare two Quaternions for inequality.
  ///
  /// @param q Quaternion to compare against.
  /// @return true if the scalar or vector components differ.
  constexpr bool operator!=(const Quaternion<T>& q) const {
    return !(*this == q);
  }

  /// @brief Multiply this Quaternion with another Quaternion.
  ///
  /// @note This is equivalent to
  /// <code>FromMatrix(ToMatrix() * q.ToMatrix()).</code>
  /// @param q Quaternion to multiply with.
  /// @return Quaternion containing the result.
  constexpr Quaternion<T> operator*(const Quaternion<T>& q) const {
    const Vector<T, 3> v1 = vector();
    const Vector<T, 3> v2 = q.vector();
    return Quaternion<T>(
        data_[0] * q.data_[0] - Vector<T, 3>::DotProduct(v1, v2),
        data_[0] * v2 + q.data_[0] * v1 + Vector<T, 3>::CrossProduct(v1, v2));
  }

  /// @brief Multiply this Quaternion by a scalar (component-wise).
  ///
  /// Scales each component of the quaternion (both scalar and vector parts)
  /// by the given factor. This operation is associative and commutative with
  /// respect to real-number multiplication.
  ///
  /// @note This does NOT scale the rotation angle. To scale the rotation
  /// angle, use @ref ScaleAngle instead.
  ///
  /// @param s1 Scalar to multiply with.
  /// @return Quaternion containing the result.
  constexpr Quaternion<T> operator*(T s1) const {
    return Quaternion<T>(data_ * s1);
  }

  /// @brief Multiply this Quaternion by a scalar (component-wise), in-place.
  ///
  /// Scales each component of the quaternion (both scalar and vector parts)
  /// by the given factor.
  ///
  /// @param s1 Scalar to multiply with.
  /// @return Reference to this Quaternion.
  constexpr Quaternion<T>& operator*=(T s1) {
    data_ *= s1;
    return *this;
  }

  /// @brief Scale the rotation angle of this Quaternion by a scalar factor.
  ///
  /// This conditions the Quaternion to be a rotation <= 180 degrees, then
  /// multiplies the angle of the rotation by a scalar factor.
  ///
  /// @warning This operation is NOT associative:
  /// <code>q.ScaleAngle(a).ScaleAngle(b)</code> may differ from
  /// <code>q.ScaleAngle(a * b)</code>.
  ///
  /// @pre The quaternion must be non-zero (normalizable to angle-axis form).
  /// @param s1 Factor to scale the rotation angle by.
  /// @return Quaternion containing the result.
  inline Quaternion<T> ScaleAngle(T s1) const {
    T angle;
    Vector<T, 3> axis;
    ToAngleAxis(&angle, &axis);
    angle *= s1;
    const T half_angle = static_cast<T>(0.5) * angle;
    // The axis coming from ToAngleAxis() is already normalized, but
    // ToAngleAxis may return slightly non-normal axes in unstable cases.
    // It should arguably handle that internally, allowing us to remove
    // the Normalized() here.
    return Quaternion<T>(cos(half_angle),
                         axis.Normalized() * static_cast<T>(sin(half_angle)));
  }

  /// @brief Rotate a Vector by this Quaternion.
  ///
  /// This will rotate the specified vector by the rotation specified by this
  /// Quaternion.
  ///
  /// @param v1 Vector to rotate.
  /// @return Rotated Vector.
  constexpr Vector<T, 3> Rotate(const Vector<T, 3>& v1) const {
    const Vector<T, 3> v = vector();
    T ss = data_[0] + data_[0];
    return ss * Vector<T, 3>::CrossProduct(v, v1) + (ss * data_[0] - T(1)) * v1
           + T(2) * Vector<T, 3>::DotProduct(v, v1) * v;
  }

  /// @brief Normalize this quaternion (in-place).
  ///
  /// The quaternion must have non-zero length. Normalizing a zero-length
  /// quaternion produces undefined results.
  ///
  /// @return Length of the quaternion.
  inline T Normalize() {
    T length = sqrt(Vector<T, 4>::DotProduct(data_, data_));
    T scale = (T(1) / length);
    data_ *= scale;
    return length;
  }

  /// @brief Calculate the normalized version of this quaternion.
  ///
  /// The quaternion must have non-zero length. Normalizing a zero-length
  /// quaternion produces undefined results.
  ///
  /// @return The normalized quaternion.
  inline Quaternion<T> Normalized() const {
    Quaternion<T> q(*this);
    q.Normalize();
    return q;
  }

  /// @brief Convert this Quaternion to a shortest-path angle and axis.
  ///
  /// The resulting angle-axis is guaranteed to have have angle <= 180 and
  /// represents the same orientation as *this, but it may not convert back to
  /// *this.
  ///
  /// For example, if *this represents "Rotate 350 degrees left", you will
  /// get the angle-axis "Rotate 10 degrees right".
  ///
  /// @param angle Receives the angle, in the range [0, pi].
  /// @param axis Receives the normalized axis.
  inline void ToAngleAxis(T* out_angle, Vector<T, 3>* out_axis) const {
    const Quaternion<T> q =
        (data_[0] > 0)
            ? *this
            : Quaternion<T>(-data_[0], -data_[1], -data_[2], -data_[3]);
    q.ToAngleAxisFull(out_angle, out_axis);
  }

  /// @brief Convert this Quaternion to an angle and axis.
  ///
  /// The resulting angle-axis uses the full range of angles supported by
  /// quaternions, and will convert back to the original Quaternion.
  ///
  /// @param angle Receives the angle, in the range [0, 2pi).
  /// @param axis Receives the normalized axis.
  inline void ToAngleAxisFull(T* out_angle, Vector<T, 3>* out_axis) const {
    Vector<T, 3> axis = vector();
    const T axis_length = axis.Normalize();
    if (axis_length < std::numeric_limits<T>::epsilon()) {
      // Normalize has left NaNs in axis.  This happens at angle = 0 and 360.
      // All axes are correct, so any will do.
      *out_axis = Vector<T, 3>(1, 0, 0);
    } else {
      *out_axis = axis;
    }
    *out_angle = T(2) * atan2(axis_length, data_[0]);
  }

  /// @brief Convert this Quaternion to 3 Euler Angles.
  ///
  /// @return 3-dimensional Vector where each element is a angle of rotation
  /// (in radians) around the x, y, and z axes.
  inline Vector<T, 3> ToEulerAngles() const {
    Matrix<T, 3> m(ToMatrix());
    T cos2 = m[0] * m[0] + m[1] * m[1];
    if (cos2 < static_cast<T>(1e-6)) {
      return Vector<T, 3>(0,
                          m[2] < 0
                              ? static_cast<T>(0.5) * std::numbers::pi_v<T>
                              : static_cast<T>(-0.5) * std::numbers::pi_v<T>,
                          -std::atan2(m[3], m[4]));
    } else {
      return Vector<T, 3>(std::atan2(m[5], m[8]),
                          std::atan2(-m[2], std::sqrt(cos2)),
                          std::atan2(m[1], m[0]));
    }
  }

  /// @brief Convert to a 3x3 Matrix.
  ///
  /// @return 3x3 rotation Matrix.
  inline Matrix<T, 3> ToMatrix() const {
    const T x2 = data_[1] * data_[1], y2 = data_[2] * data_[2],
            z2 = data_[3] * data_[3];
    const T sx = data_[0] * data_[1], sy = data_[0] * data_[2],
            sz = data_[0] * data_[3];
    const T xz = data_[1] * data_[3], yz = data_[2] * data_[3],
            xy = data_[1] * data_[2];
    return Matrix<T, 3>(
        T(1) - T(2) * (y2 + z2), T(2) * (xy + sz), T(2) * (xz - sy),
        T(2) * (xy - sz), T(1) - T(2) * (x2 + z2), T(2) * (sx + yz),
        T(2) * (sy + xz), T(2) * (yz - sx), T(1) - T(2) * (x2 + y2));
  }

  /// @brief Convert to a 4x4 Matrix.
  ///
  /// @return 4x4 transform Matrix.
  inline Matrix<T, 4> ToMatrix4() const {
    const T x2 = data_[1] * data_[1], y2 = data_[2] * data_[2],
            z2 = data_[3] * data_[3];
    const T sx = data_[0] * data_[1], sy = data_[0] * data_[2],
            sz = data_[0] * data_[3];
    const T xz = data_[1] * data_[3], yz = data_[2] * data_[3],
            xy = data_[1] * data_[2];
    return Matrix<T, 4>(T(1) - T(2) * (y2 + z2), T(2) * (xy + sz),
                        T(2) * (xz - sy), T(0), T(2) * (xy - sz),
                        T(1) - T(2) * (x2 + z2), T(2) * (sx + yz), T(0),
                        T(2) * (sy + xz), T(2) * (yz - sx),
                        T(1) - T(2) * (x2 + y2), T(0), T(0), T(0), T(0), T(1));
  }

  /// @brief Create a Quaternion from an angle and axis.
  ///
  /// @param angle Angle in radians to rotate by.
  /// @param axis Axis in 3D space to rotate around.
  /// @return Quaternion containing the result.
  static Quaternion<T> FromAngleAxis(T angle, const Vector<T, 3>& axis) {
    const T halfAngle = static_cast<T>(0.5) * angle;
    Vector<T, 3> localAxis(axis);
    return Quaternion<T>(cos(halfAngle), localAxis.Normalized()
                                             * static_cast<T>(sin(halfAngle)));
  }

  /// @brief Create a quaternion from 3 euler angles.
  ///
  /// @param angles 3-dimensional Vector where each element contains an
  /// angle in radians to rotate by about the x, y and z axes.
  /// @return Quaternion containing the result.
  static Quaternion<T> FromEulerAngles(const Vector<T, 3>& angles) {
    const Vector<T, 3> halfAngles(static_cast<T>(0.5) * angles[0],
                                  static_cast<T>(0.5) * angles[1],
                                  static_cast<T>(0.5) * angles[2]);
    const T sinx = std::sin(halfAngles[0]);
    const T cosx = std::cos(halfAngles[0]);
    const T siny = std::sin(halfAngles[1]);
    const T cosy = std::cos(halfAngles[1]);
    const T sinz = std::sin(halfAngles[2]);
    const T cosz = std::cos(halfAngles[2]);
    return Quaternion<T>(cosx * cosy * cosz + sinx * siny * sinz,
                         sinx * cosy * cosz - cosx * siny * sinz,
                         cosx * siny * cosz + sinx * cosy * sinz,
                         cosx * cosy * sinz - sinx * siny * cosz);
  }

  /// @brief Create a quaternion from 3 euler angles.
  ///
  /// @param x_rotation angle in radians to rotate by about the x axis.
  /// @param y_rotation angle in radians to rotate by about the y axis.
  /// @param z_rotation angle in radians to rotate by about the z axis.
  /// @return Quaternion containing the result.
  static Quaternion<T> FromEulerAngles(T x_rotation, T y_rotation,
                                       T z_rotation) {
    return FromEulerAngles(Vector<T, 3>(x_rotation, y_rotation, z_rotation));
  }

  /// @brief Create a quaternion from a rotation Matrix.
  ///
  /// @param m 3x3 rotation Matrix.
  /// @return Quaternion containing the result.
  static Quaternion<T> FromMatrix(const Matrix<T, 3>& m) {
    const T trace = m(0, 0) + m(1, 1) + m(2, 2);
    if (trace > 0) {
      const T s = sqrt(trace + T(1)) * T(2);
      const T oneOverS = T(1) / s;
      return Quaternion<T>(T(0.25) * s, (m[5] - m[7]) * oneOverS,
                           (m[6] - m[2]) * oneOverS, (m[1] - m[3]) * oneOverS);
    } else if (m[0] > m[4] && m[0] > m[8]) {
      const T s = sqrt(m[0] - m[4] - m[8] + T(1)) * T(2);
      const T oneOverS = T(1) / s;
      return Quaternion<T>((m[5] - m[7]) * oneOverS, T(0.25) * s,
                           (m[3] + m[1]) * oneOverS, (m[6] + m[2]) * oneOverS);
    } else if (m[4] > m[8]) {
      const T s = sqrt(m[4] - m[0] - m[8] + T(1)) * T(2);
      const T oneOverS = T(1) / s;
      return Quaternion<T>((m[6] - m[2]) * oneOverS, (m[3] + m[1]) * oneOverS,
                           T(0.25) * s, (m[5] + m[7]) * oneOverS);
    } else {
      const T s = sqrt(m[8] - m[0] - m[4] + T(1)) * T(2);
      const T oneOverS = T(1) / s;
      return Quaternion<T>((m[1] - m[3]) * oneOverS, (m[6] + m[2]) * oneOverS,
                           (m[5] + m[7]) * oneOverS, T(0.25) * s);
    }
  }

  /// @brief Create a quaternion from the upper-left 3x3 roation Matrix of a 4x4
  /// Matrix.
  ///
  /// @param m 4x4 Matrix.
  /// @return Quaternion containing the result.
  static Quaternion<T> FromMatrix(const Matrix<T, 4>& m) {
    const T trace = m(0, 0) + m(1, 1) + m(2, 2);
    if (trace > 0) {
      const T s = sqrt(trace + T(1)) * T(2);
      const T oneOverS = T(1) / s;
      return Quaternion<T>(T(0.25) * s, (m[6] - m[9]) * oneOverS,
                           (m[8] - m[2]) * oneOverS, (m[1] - m[4]) * oneOverS);
    } else if (m[0] > m[5] && m[0] > m[10]) {
      const T s = sqrt(m[0] - m[5] - m[10] + T(1)) * T(2);
      const T oneOverS = T(1) / s;
      return Quaternion<T>((m[6] - m[9]) * oneOverS, T(0.25) * s,
                           (m[4] + m[1]) * oneOverS, (m[8] + m[2]) * oneOverS);
    } else if (m[5] > m[10]) {
      const T s = sqrt(m[5] - m[0] - m[10] + T(1)) * T(2);
      const T oneOverS = T(1) / s;
      return Quaternion<T>((m[8] - m[2]) * oneOverS, (m[4] + m[1]) * oneOverS,
                           T(0.25) * s, (m[6] + m[9]) * oneOverS);
    } else {
      const T s = sqrt(m[10] - m[0] - m[5] + T(1)) * T(2);
      const T oneOverS = T(1) / s;
      return Quaternion<T>((m[1] - m[4]) * oneOverS, (m[8] + m[2]) * oneOverS,
                           (m[6] + m[9]) * oneOverS, T(0.25) * s);
    }
  }

  /// @brief Calculate the dot product of two Quaternions.
  ///
  /// @param q1 First quaternion.
  /// @param q2 Second quaternion
  /// @return The scalar dot product of both Quaternions.
  static constexpr T DotProduct(const Quaternion<T>& q1,
                                const Quaternion<T>& q2) {
    return Vector<T, 4>::DotProduct(q1.data_, q2.data_);
  }

  /// @brief Calculate the shortest-path spherical linear interpolation between
  /// two orientations.
  ///
  /// This method always gives you the "short way around" interpolation. If you
  /// need mathematical Slerp(), use ToAngleAxisFull() and FromAngleAxis().
  ///
  /// @note Both input quaternions must be unit quaternions (normalized).
  /// Results are undefined for non-unit quaternions. Call Normalize() on each
  /// quaternion before passing it to Slerp if unsure.
  ///
  /// @param q1 Start Quaternion (must be normalized).
  /// @param q2 End Quaternion (must be normalized).
  /// @param s1 The scalar value determining how far from q1 and q2 the
  /// resulting quaternion should be.  A value of 0 corresponds to q1 and a
  /// value of 1 corresponds to q2.
  /// @result Quaternion containing the result.
  static inline Quaternion<T> Slerp(const Quaternion<T>& q1,
                                    const Quaternion<T>& q2, T s1) {
    if (Vector<T, 4>::DotProduct(q1.data_, q2.data_) > static_cast<T>(0.9999)) {
      return Quaternion<T>(q1.data_ * (T(1) - s1) + q2.data_ * s1).Normalized();
    }
    return q1 * (q1.Conjugate() * q2).ScaleAngle(s1);
  }

  /// @brief Access an element of the quaternion.
  ///
  /// @param i Index of the element to access.
  /// @return A reference to the accessed element.
  constexpr T& operator[](const int i) { return data_[i]; }

  /// @brief Access an element of the quaternion.
  ///
  /// @param i Index of the element to access.
  /// @return A const reference to the accessed element.
  constexpr const T& operator[](const int i) const { return data_[i]; }

  /// @brief Returns a vector that is perpendicular to the supplied vector.
  ///
  /// @param v1 An arbitrary vector
  /// @return A vector perpendicular to v1.  Normally this will just be
  /// the cross product of v1, v2.  If they are parallel or opposite though,
  /// the routine will attempt to pick a vector.
  static constexpr Vector<T, 3> PerpendicularVector(const Vector<T, 3>& v) {
    // We start out by taking the cross product of the vector and the x-axis to
    // find something parallel to the input vectors.  If that cross product
    // turns out to be length 0 (i. e. the vectors already lie along the x axis)
    // then we use the y-axis instead.
    Vector<T, 3> axis = Vector<T, 3>::CrossProduct(
        Vector<T, 3>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)),
        v);
    // We use a fairly high epsilon here because we know that if this number
    // is too small, the axis we'll get from a cross product with the y axis
    // will be much better and more numerically stable.
    if (axis.LengthSquared() < static_cast<T>(0.05)) {
      axis = Vector<T, 3>::CrossProduct(
          Vector<T, 3>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)),
          v);
    }
    return axis;
  }

  /// @brief Returns the a Quaternion that rotates from start to end.
  ///
  /// @param v1 The starting vector
  /// @param v2 The vector to rotate to
  /// @param preferred_axis the axis to use, if v1 and v2 are parallel.
  /// @return A Quaternion describing the rotation from v1 to v2
  /// See the comment on RotateFromTo for an explanation of the math.
  static inline Quaternion<T> RotateFromToWithAxis(
      const Vector<T, 3>& v1, const Vector<T, 3>& v2,
      const Vector<T, 3>& preferred_axis) {
    return RotateFromToImpl(v1.Normalized(), v2.Normalized(), preferred_axis);
  }

  /// @brief Returns the a Quaternion that rotates from start to end.
  ///
  /// @param v1 The starting vector
  /// @param v2 The vector to rotate to
  /// @return A Quaternion describing the rotation from v1 to v2.  In the case
  /// where the vectors are parallel, it returns the identity.  In the case
  /// where
  /// they point in opposite directions, it picks an arbitrary axis.  (Since
  /// there
  /// are technically infinite possible quaternions to represent a 180 degree
  /// rotation.)
  ///
  /// The final equation used here is fairly elegant, but its derivation is
  /// not obvious:  We want to find the quaternion that represents the angle
  /// between Start and End.
  ///
  /// The angle can be expressed as a quaternion with the values:
  ///  angle: ArcCos(dotproduct(start, end) / (|start|*|end|)
  ///  axis: crossproduct(start, end).normalized * sin(angle/2)
  ///
  /// or written as:
  ///  quaternion(cos(angle/2), axis * sin(angle/2))
  ///
  /// Using the trig identity:
  ///  sin(angle * 2) = 2 * sin(angle) * cos*angle)
  /// Via substitution, we can turn this into:
  ///  sin(angle/2) = 0.5 * sin(angle)/cos(angle/2)
  ///
  /// Using this substitution, we get:
  ///  quaternion( cos(angle/2),
  ///             0.5 * crossproduct(start, end).normalized
  ///                 * sin(angle) / cos(angle/2))
  ///
  /// If we scale the whole thing up by 2 * cos(angle/2) then we get:
  ///  quaternion(2 * cos(angle/2) * cos(angle/2),
  ///             crossproduct(start, end).normalized * sin(angle))
  ///
  /// (Note that the quaternion is no longer normalized after this scaling)
  ///
  /// Another trig identity:
  ///   cos(angle/2) = sqrt((1 + cos(angle) / 2)
  ///
  /// Substituting this in, we can simplify the quaternion scalar:
  ///
  ///  quaternion(1 + cos(angle),
  ///             crossproduct(start, end).normalized * sin(angle))
  ///
  /// Because cross(start, end) has a magnitude of |start|*|end|*sin(angle),
  ///  crossproduct(start,end).normalized
  /// is equivalent to
  ///  crossproduct(start,end) / |start| * |end| * sin(angle)
  /// So after that substitution:
  ///
  ///  quaternion(1 + cos(angle),
  ///             crossproduct(start, end) / (|start| * |end|))
  ///
  /// dotproduct(start, end) has the value of |start| * |end| * cos(angle),
  /// so by algebra,
  ///  cos(angle) = dotproduct(start, end) / (|start| * |end|)
  /// we can replace our quaternion scalar here also:
  ///
  ///  quaternion(1 + dotproduct(start, end) / (|start| * |end|),
  ///             crossproduct(start, end) / (|start| * |end|))
  ///
  /// If start and end are normalized, then |start| * |end| = 1, giving us a
  /// final quaternion of:
  ///
  /// quaternion(1 + dotproduct(start, end), crossproduct(start, end))
  static inline Quaternion<T> RotateFromTo(const Vector<T, 3>& v1,
                                           const Vector<T, 3>& v2) {
    Vector<T, 3> start = v1.Normalized();
    return RotateFromToImpl(start, v2.Normalized(), PerpendicularVector(start));
  }

  /// @brief Returns a quaternion looking at forward vector with an up vector.
  ///
  /// @tparam H Coordinate system handedness
  ///           (Handedness::kRightHanded or Handedness::kLeftHanded).
  /// @param forward The forward vector (Vector to face).
  /// @param up The up vector.
  /// @param Forward and up do not have to be orthogonal.
  /// @param Forward and up cannot be parallel.
  /// @param Forward and up cannot be zero vectors.
  ///
  /// @return A Quaternion looking at forward vector with an up vector.
  ///
  /// Uses Matrix::LookAt to get the Rotation Matrix, then convert it to Quat.
  /// Matrix::LookAt takes destination and source as first and second param.
  /// The params can be represented with zero-vector as source and
  /// forward-vector as destination.
  template <Handedness H = Handedness::kRightHanded>
  static inline Quaternion<T> LookAt(const Vector<T, 3>& forward,
                                     const Vector<T, 3>& up) {
    // Matrix::LookAt produces a view matrix (world-to-camera transform).
    // Its rotation part is the inverse of the camera's world orientation.
    // Since the inverse of a unit quaternion is its conjugate, we conjugate
    // the result to obtain the camera's orientation quaternion.
    return FromMatrix(Matrix<T, 4>::template LookAt<H>(
                          forward, Vector<T, 3>(static_cast<T>(0)), up))
        .Conjugate();
  }

  /// @brief Contains a quaternion doing the identity transform.
  static Quaternion<T> identity;

  MATHKATA_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

 private:
  /// @brief Construct a Quaternion directly from a Vector<T, 4>.
  ///
  /// The layout is [scalar, x, y, z].
  explicit Quaternion(const Vector<T, 4>& data) : data_(data) {}

  /// @brief Shared implementation for RotateFromTo variants.
  ///
  /// @param start Normalized starting direction.
  /// @param end Normalized target direction.
  /// @param fallback_axis Axis to use for 180-degree rotation when vectors
  ///        are anti-parallel.
  static inline Quaternion<T> RotateFromToImpl(
      const Vector<T, 3>& start, const Vector<T, 3>& end,
      const Vector<T, 3>& fallback_axis) {
    T dot_product = Vector<T, 3>::DotProduct(start, end);
    // Any rotation < 0.1 degrees is treated as no rotation
    // in order to avoid division by zero errors.
    // So we early-out in cases where it's less than 0.1 degrees.
    // cos( 0.1 degrees) = 0.99999847691
    if (dot_product >= static_cast<T>(0.99999847691)) {
      return Quaternion<T>::identity;
    }
    // If the vectors point in opposite directions, return a 180 degree
    // rotation on the fallback axis.
    if (dot_product <= static_cast<T>(-0.99999847691)) {
      return Quaternion<T>(static_cast<T>(0), fallback_axis);
    }
    // Degenerate cases have been handled, so if we're here, we have to
    // actually compute the angle we want:
    Vector<T, 3> cross_product = Vector<T, 3>::CrossProduct(start, end);
    return Quaternion<T>(T(1) + dot_product, cross_product).Normalized();
  }

  Vector<T, 4> data_;
};

template <typename T>
Quaternion<T> Quaternion<T>::identity = Quaternion<T>(1, 0, 0, 0);
/// @}

/// @addtogroup mathkata_quaternion
/// @{

/// @brief Multiply a Quaternion by a scalar (component-wise).
///
/// Scales each component of the quaternion (both scalar and vector parts)
/// by the given factor.
///
/// @param s Scalar to multiply with.
/// @param q Quaternion to scale.
/// @return Quaternion containing the result.
///
/// @related Quaternion
template <class T>
constexpr Quaternion<T> operator*(T s, const Quaternion<T>& q) {
  return q * s;
}
/// @}

}  // namespace mathkata
#endif  // MATHKATA_QUATERNION_H_

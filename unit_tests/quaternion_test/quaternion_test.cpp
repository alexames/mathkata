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
#include "mathkata/quaternion.h"

#include <cmath>
#include <numbers>

#include "gtest/gtest.h"
#include "mathkata/constants.h"
#include "mathkata/io.h"
#include "precision.h"

namespace {

using ::testing::AssertionResult;

class QuaternionTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Verify that Quaternion<float> is 16 bytes (4 floats) and
// Quaternion<double> is 32 bytes (4 doubles), not inflated by SIMD padding.
static_assert(sizeof(mathkata::Quaternion<float>) == 4 * sizeof(float));
static_assert(sizeof(mathkata::Quaternion<double>) == 4 * sizeof(double));

// This will automatically generate tests for each template parameter.
#define TEST_ALL_F(MY_TEST)                   \
  TEST_F(QuaternionTests, MY_TEST) {          \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

#define EXPECT_NEAR_VEC3(v1, v2, abs_error) \
  EXPECT_TRUE(IsNearVector((v1), (v2), (abs_error))) << v1 << "\n" << v2 << "\n"

#define EXPECT_EQ_QUAT(q1, q2)               \
  {                                          \
    EXPECT_EQ((q1).scalar(), (q2).scalar()); \
    EXPECT_EQ((q1).vector(), (q2).vector()); \
  }

#define EXPECT_NEAR_QUAT(q1, q2, abs_error) \
  EXPECT_TRUE(IsNearQuat((q1), (q2), (abs_error))) << q1 << "\n" << q2 << "\n"

#define EXPECT_NEAR_ORIENTATION(q1, q2, abs_error)                      \
  EXPECT_TRUE(IsNearOrientation((q1), (q2), (abs_error))) << q1 << "\n" \
                                                          << q2 << "\n"

AssertionResult IsNearDouble(double val1, double val2, double abs_error) {
  const double diff = std::fabs(val1 - val2);
  if (diff <= abs_error) {
    return ::testing::AssertionSuccess();
  }
  return ::testing::AssertionFailure()
         << "The difference between " << val1 << " and " << val2 << " is "
         << diff << ", which exceeds " << abs_error << ".";
}

template <class T, int d>
AssertionResult IsNearVector(mathkata::Vector<T, d> v1,
                             mathkata::Vector<T, d> v2, double abs_error) {
  for (int i = 0; i < d; ++i) {
    AssertionResult result = IsNearDouble(v1[i], v2[i], abs_error);
    if (!result) {
      return result << " at v[" << i << "]";
    }
  }
  return ::testing::AssertionSuccess();
}

template <class T>
AssertionResult IsNearQuat(mathkata::Quaternion<T> q1,
                           mathkata::Quaternion<T> q2, double abs_error) {
  {
    AssertionResult result = IsNearDouble(q1.scalar(), q2.scalar(), abs_error);
    if (!result) {
      return result << " at .scalar() " << q1 << " " << q2;
    }
  }
  {
    AssertionResult result = IsNearVector(q1.vector(), q2.vector(), abs_error);
    if (!result) {
      return result << " at .vector() " << q1 << " " << q2;
    }
  }
  return ::testing::AssertionSuccess();
}

// Unlike IsNearQuat, this test considers q and -q to be equivalent.
// This is appropriate when treating quats as orientations (rather than
// rotations).
template <class T>
AssertionResult IsNearOrientation(mathkata::Quaternion<T> q1,
                                  mathkata::Quaternion<T> q2,
                                  double abs_error) {
  // Put them both into the same hemisphere.
  if (mathkata::Quaternion<T>::dotProduct(q1, q2) < 0) {
    q2 = mathkata::Quaternion<T>(-q2.scalar(), -q2.vector());
  }
  return IsNearQuat(q1, q2, abs_error);
}

// Test our test helpers.
template <class T>
void TestHelpers_Test(const T& precision) {
  (void)precision;
  using Quaternion = mathkata::Quaternion<T>;
  const double epsilon = 1e-5;

  EXPECT_NEAR_QUAT(Quaternion(1, 0, 0, 1e-6f), Quaternion::identity, epsilon);

  // Test that opposing quats are !IsNearQuat and IsNearOrientation.
  const Quaternion q2 = Quaternion(3, 4, 5, 6).normalized();
  const Quaternion q2_negated(-q2.scalar(), -q2.vector());
  EXPECT_FALSE(IsNearQuat(q2, q2_negated, epsilon));
  EXPECT_TRUE(IsNearOrientation(q2, q2_negated, epsilon));
  EXPECT_NEAR_ORIENTATION(q2, q2_negated, epsilon);
}
TEST_ALL_F(TestHelpers)

// Test accessing elements of the quaternion using the const array accessor.
template <class T>
void ConstAccessor_Test(const T& precision) {
  (void)precision;
  const mathkata::Quaternion<T> quaternion(
      static_cast<T>(0.50), static_cast<T>(0.76), static_cast<T>(0.38),
      static_cast<T>(0.19));
  EXPECT_EQ(static_cast<T>(0.50), quaternion[0]);
  EXPECT_EQ(static_cast<T>(0.76), quaternion[1]);
  EXPECT_EQ(static_cast<T>(0.38), quaternion[2]);
  EXPECT_EQ(static_cast<T>(0.19), quaternion[3]);
}
TEST_ALL_F(ConstAccessor)

// Test reading and writing elements via operator[].
// Index 0 maps to the scalar (s_), indices 1-3 map to the vector (v_[0-2]).
template <class T>
void MutableAccessor_Test(const T& precision) {
  (void)precision;
  mathkata::Quaternion<T> quaternion(static_cast<T>(0.50), static_cast<T>(0.76),
                                     static_cast<T>(0.38),
                                     static_cast<T>(0.19));

  // Verify reading via operator[] matches the scalar and vector accessors.
  EXPECT_EQ(quaternion.scalar(), quaternion[0]);
  EXPECT_EQ(quaternion.vector()[0], quaternion[1]);
  EXPECT_EQ(quaternion.vector()[1], quaternion[2]);
  EXPECT_EQ(quaternion.vector()[2], quaternion[3]);

  // Modify the scalar component via operator[].
  quaternion[0] = static_cast<T>(1.0);
  EXPECT_EQ(static_cast<T>(1.0), quaternion[0]);
  EXPECT_EQ(static_cast<T>(1.0), quaternion.scalar());

  // Modify vector components via operator[].
  quaternion[1] = static_cast<T>(2.0);
  quaternion[2] = static_cast<T>(3.0);
  quaternion[3] = static_cast<T>(4.0);
  EXPECT_EQ(static_cast<T>(2.0), quaternion[1]);
  EXPECT_EQ(static_cast<T>(3.0), quaternion[2]);
  EXPECT_EQ(static_cast<T>(4.0), quaternion[3]);
  EXPECT_EQ(static_cast<T>(2.0), quaternion.vector()[0]);
  EXPECT_EQ(static_cast<T>(3.0), quaternion.vector()[1]);
  EXPECT_EQ(static_cast<T>(4.0), quaternion.vector()[2]);
}
TEST_ALL_F(MutableAccessor)

// Test equality operator for quaternions.
template <class T>
void Equality_Test(const T& precision) {
  (void)precision;
  const mathkata::Quaternion<T> q1(static_cast<T>(0.50), static_cast<T>(0.76),
                                   static_cast<T>(0.38), static_cast<T>(0.19));
  const mathkata::Quaternion<T> q2(static_cast<T>(0.50), static_cast<T>(0.76),
                                   static_cast<T>(0.38), static_cast<T>(0.19));
  const mathkata::Quaternion<T> q3(static_cast<T>(0.10), static_cast<T>(0.76),
                                   static_cast<T>(0.38), static_cast<T>(0.19));
  // Identical quaternions should be equal.
  EXPECT_TRUE(q1 == q2);
  EXPECT_FALSE(q1 != q2);
  // Quaternions with different scalar parts should not be equal.
  EXPECT_FALSE(q1 == q3);
  EXPECT_TRUE(q1 != q3);
}
TEST_ALL_F(Equality)

// Test inequality operator for quaternions with differing vector components.
template <class T>
void Inequality_Test(const T& precision) {
  (void)precision;
  const mathkata::Quaternion<T> q1(static_cast<T>(1), static_cast<T>(2),
                                   static_cast<T>(3), static_cast<T>(4));
  // Differ in first vector element.
  const mathkata::Quaternion<T> q2(static_cast<T>(1), static_cast<T>(9),
                                   static_cast<T>(3), static_cast<T>(4));
  // Differ in second vector element.
  const mathkata::Quaternion<T> q3(static_cast<T>(1), static_cast<T>(2),
                                   static_cast<T>(9), static_cast<T>(4));
  // Differ in third vector element.
  const mathkata::Quaternion<T> q4(static_cast<T>(1), static_cast<T>(2),
                                   static_cast<T>(3), static_cast<T>(9));
  EXPECT_TRUE(q1 != q2);
  EXPECT_TRUE(q1 != q3);
  EXPECT_TRUE(q1 != q4);
  EXPECT_FALSE(q1 == q2);
  EXPECT_FALSE(q1 == q3);
  EXPECT_FALSE(q1 == q4);
  // identity should equal itself.
  EXPECT_TRUE(mathkata::Quaternion<T>::identity
              == mathkata::Quaternion<T>::identity);
  EXPECT_FALSE(mathkata::Quaternion<T>::identity
               != mathkata::Quaternion<T>::identity);
}
TEST_ALL_F(Inequality)

// Test accessing the scalar component of the quaternion using the scalar
// accessor.
template <class T>
void ScalarAccessor_Test(const T& precision) {
  (void)precision;
  mathkata::Quaternion<T> quaternion(static_cast<T>(0.50), static_cast<T>(0.76),
                                     static_cast<T>(0.38),
                                     static_cast<T>(0.19));
  EXPECT_EQ(static_cast<T>(0.50), quaternion.scalar());
}
TEST_ALL_F(ScalarAccessor)

// Test accessing the scalar component of the quaternion using the const scalar
// accessor.
template <class T>
void ConstScalarAccessor_Test(const T& precision) {
  (void)precision;
  const mathkata::Quaternion<T> quaternion(
      static_cast<T>(0.50), static_cast<T>(0.76), static_cast<T>(0.38),
      static_cast<T>(0.19));
  EXPECT_EQ(static_cast<T>(0.50), quaternion.scalar());
}
TEST_ALL_F(ConstScalarAccessor)

// Test mutating the scalar component of the quaternion using the scalar
// mutator.
template <class T>
void ScalarMutator_Test(const T& precision) {
  (void)precision;
  auto quaternion = mathkata::Quaternion<T>::uninitialized();
  quaternion.set_scalar(static_cast<T>(0.38));
  EXPECT_EQ(static_cast<T>(0.38), quaternion[0]);
}
TEST_ALL_F(ScalarMutator)

// Test accessing elements of the quaternion using the vector accessor.
template <class T>
void VectorAccessor_Test(const T& precision) {
  (void)precision;
  mathkata::Quaternion<T> quaternion(static_cast<T>(0.50), static_cast<T>(0.76),
                                     static_cast<T>(0.38),
                                     static_cast<T>(0.19));
  EXPECT_EQ(static_cast<T>(0.76), quaternion.vector()[0]);
  EXPECT_EQ(static_cast<T>(0.38), quaternion.vector()[1]);
  EXPECT_EQ(static_cast<T>(0.19), quaternion.vector()[2]);
}
TEST_ALL_F(VectorAccessor)

// Test accessing elements of the quaternion using the const vector accessor.
template <class T>
void ConstVectorAccessor_Test(const T& precision) {
  (void)precision;
  const mathkata::Quaternion<T> quaternion(
      static_cast<T>(0.50), static_cast<T>(0.76), static_cast<T>(0.38),
      static_cast<T>(0.19));
  EXPECT_EQ(static_cast<T>(0.76), quaternion.vector()[0]);
  EXPECT_EQ(static_cast<T>(0.38), quaternion.vector()[1]);
  EXPECT_EQ(static_cast<T>(0.19), quaternion.vector()[2]);
}
TEST_ALL_F(ConstVectorAccessor)

// Test mutating the vector component of the quaternion using the vector
// mutator.
template <class T>
void VectorMutator_Test(const T& precision) {
  (void)precision;
  auto quaternion = mathkata::Quaternion<T>::uninitialized();
  quaternion.set_vector(mathkata::Vector<T, 3>(
      static_cast<T>(0.38), static_cast<T>(0.76), static_cast<T>(0.50)));
  EXPECT_EQ(static_cast<T>(0.38), quaternion.vector()[0]);
  EXPECT_EQ(static_cast<T>(0.76), quaternion.vector()[1]);
  EXPECT_EQ(static_cast<T>(0.50), quaternion.vector()[2]);
}
TEST_ALL_F(VectorMutator)

// This will test converting a Quaternion to and from angle/Axis,
// Euler Angles, and Matrices
template <class T>
void Conversion_Test(const T& precision) {
  mathkata::Vector<T, 3> angles(static_cast<T>(1.5), static_cast<T>(2.3),
                                static_cast<T>(0.6));
  // This will create a Quaternion from Euler Angles, convert back to
  // Euler Angles, and verify that they match
  mathkata::Quaternion<T> qea(mathkata::Quaternion<T>::fromEulerAngles(angles));
  mathkata::Vector<T, 3> convertedAngles(qea.toEulerAngles());
  EXPECT_NEAR(angles[0], std::numbers::pi_v<T> + convertedAngles[0], precision);
  EXPECT_NEAR(angles[1], std::numbers::pi_v<T> - convertedAngles[1], precision);
  EXPECT_NEAR(angles[2], std::numbers::pi_v<T> + convertedAngles[2], precision);
  // This will create a Quaternion from Axis angle, convert back to
  // Axis angle, and verify that they match.
  mathkata::Vector<T, 3> axis(static_cast<T>(4.3), static_cast<T>(7.6),
                              static_cast<T>(1.2));
  axis.normalize();
  T angle = static_cast<T>(1.2);
  mathkata::Quaternion<T> qaa(
      mathkata::Quaternion<T>::fromAngleAxis(angle, axis));
  auto convertedAxis = mathkata::Vector<T, 3>::uninitialized();
  T convertedAngle;
  qaa.toAngleAxis(&convertedAngle, &convertedAxis);
  EXPECT_NEAR(angle, convertedAngle, precision);
  EXPECT_NEAR(axis[0], convertedAxis[0], precision);
  EXPECT_NEAR(axis[1], convertedAxis[1], precision);
  EXPECT_NEAR(axis[2], convertedAxis[2], precision);
  // This will create a Quaternion from a 3x3 Matrix, convert back to a Matrix,
  // and verify that they match.
  mathkata::Matrix<T, 3> rx(1, 0, 0, 0, cos(angles[0]), sin(angles[0]), 0,
                            -sin(angles[0]), cos(angles[0]));
  mathkata::Matrix<T, 3> ry(cos(angles[1]), 0, -sin(angles[1]), 0, 1, 0,
                            sin(angles[1]), 0, cos(angles[1]));
  mathkata::Matrix<T, 3> rz(cos(angles[2]), sin(angles[2]), 0, -sin(angles[2]),
                            cos(angles[2]), 0, 0, 0, 1);
  mathkata::Matrix<T, 3> m(rz * ry * rx);
  mathkata::Quaternion<T> qm(mathkata::Quaternion<T>::fromMatrix(m));
  mathkata::Matrix<T, 3> convertedM(qm.toMatrix());
  for (int i = 0; i < 9; ++i) EXPECT_NEAR(m[i], convertedM[i], precision);
  // This will create a Quaternion from a 4x4 Matrix, convert back to a Matrix,
  // and verify that they match.
  // Recycling the 3x3 matrix from before.
  mathkata::Matrix<T, 4> m4 = mathkata::Matrix<T, 4>::fromRotationMatrix(m);
  mathkata::Quaternion<T> qm4(mathkata::Quaternion<T>::fromMatrix(m4));
  mathkata::Matrix<T, 4> convertedM4(qm4.toMatrix4());
  for (int i = 0; i < 15; ++i) EXPECT_NEAR(m4[i], convertedM4[i], precision);
}
TEST_ALL_F(Conversion)

// This will test the conjugate of a quaternion and verify that it negates the
// vector part while preserving the scalar part.
template <class T>
void conjugate_Test(const T& precision) {
  (void)precision;
  mathkata::Quaternion<T> q(static_cast<T>(1.4), static_cast<T>(6.3),
                            static_cast<T>(8.5), static_cast<T>(5.9));
  mathkata::Quaternion<T> conj = q.conjugate();
  EXPECT_EQ(q.scalar(), conj.scalar());
  EXPECT_EQ(-q.vector()[0], conj.vector()[0]);
  EXPECT_EQ(-q.vector()[1], conj.vector()[1]);
  EXPECT_EQ(-q.vector()[2], conj.vector()[2]);
}
TEST_ALL_F(conjugate)

// This will test inverting a quaternion and verify that q * q.inverse() yields
// the identity quaternion for both unit and non-unit quaternions.
template <class T>
void inverse_Test(const T& precision) {
  const double epsilon = static_cast<double>(precision) * 10;

  // Test with a non-unit quaternion.
  mathkata::Quaternion<T> q1(static_cast<T>(1.4), static_cast<T>(6.3),
                             static_cast<T>(8.5), static_cast<T>(5.9));
  mathkata::Quaternion<T> product1 = q1 * q1.inverse();
  EXPECT_NEAR_QUAT(mathkata::Quaternion<T>::identity, product1, epsilon);

  // Also test q.inverse() * q.
  mathkata::Quaternion<T> product2 = q1.inverse() * q1;
  EXPECT_NEAR_QUAT(mathkata::Quaternion<T>::identity, product2, epsilon);

  // Test with a unit quaternion.
  mathkata::Vector<T, 3> axis(static_cast<T>(4.3), static_cast<T>(7.6),
                              static_cast<T>(1.2));
  axis.normalize();
  mathkata::Quaternion<T> q2 =
      mathkata::Quaternion<T>::fromAngleAxis(static_cast<T>(1.2), axis);
  mathkata::Quaternion<T> product3 = q2 * q2.inverse();
  EXPECT_NEAR_QUAT(mathkata::Quaternion<T>::identity, product3, epsilon);
}
TEST_ALL_F(inverse)

// This will test the multiplication of quaternions.
template <class T>
void Mult_Test(const T& precision) {
  mathkata::Vector<T, 3> axis(static_cast<T>(4.3), static_cast<T>(7.6),
                              static_cast<T>(1.2));
  axis.normalize();
  T angle1 = static_cast<T>(1.2), angle2 = static_cast<T>(0.7),
    angle3 = angle2 + precision * 10;
  mathkata::Quaternion<T> qaa1(
      mathkata::Quaternion<T>::fromAngleAxis(angle1, axis));
  mathkata::Quaternion<T> qaa2(
      mathkata::Quaternion<T>::fromAngleAxis(angle2, axis));
  mathkata::Quaternion<T> qaa3(
      mathkata::Quaternion<T>::fromAngleAxis(angle3, axis));
  auto convertedAxis = mathkata::Vector<T, 3>::uninitialized();
  T convertedAngle;
  // This will verify that multiplying two quaternions corresponds to the sum
  // of the rotations.
  (qaa1 * qaa2).toAngleAxis(&convertedAngle, &convertedAxis);
  EXPECT_NEAR(angle1 + angle2, convertedAngle, precision);
  // This will verify that scaleAngle on a quaternion corresponds
  // to scaling the rotation.
  qaa1.scaleAngle(2).toAngleAxis(&convertedAngle, &convertedAxis);
  EXPECT_NEAR(angle1 * 2, convertedAngle, precision);
  mathkata::Vector<T, 3> v(3.5f, 6.4f, 7.0f);
  mathkata::Vector<T, 4> v4(3.5f, 6.4f, 7.0f, 0.0f);
  // This will verify that multiplying by a vector corresponds to applying
  // the rotation to that vector.
  mathkata::Vector<T, 3> quatRotatedV(qaa1.rotate(v));
  mathkata::Vector<T, 3> matRotatedV(qaa1.toMatrix() * v);
  mathkata::Vector<T, 4> mat4RotatedV(qaa1.toMatrix4() * v4);
  EXPECT_NEAR(quatRotatedV[0], matRotatedV[0], 10 * precision);
  EXPECT_NEAR(quatRotatedV[1], matRotatedV[1], 10 * precision);
  EXPECT_NEAR(quatRotatedV[2], matRotatedV[2], 10 * precision);

  EXPECT_NEAR(quatRotatedV[0], mat4RotatedV[0], 10 * precision);
  EXPECT_NEAR(quatRotatedV[1], mat4RotatedV[1], 10 * precision);
  EXPECT_NEAR(quatRotatedV[2], mat4RotatedV[2], 10 * precision);
  // This will verify that interpolating two quaternions corresponds to
  // interpolating the angle.
  mathkata::Quaternion<T> slerp1(
      mathkata::Quaternion<T>::slerp(qaa1, qaa2, 0.5));
  slerp1.toAngleAxis(&convertedAngle, &convertedAxis);
  EXPECT_NEAR(.5 * (angle1 + angle2), convertedAngle, precision);
  mathkata::Quaternion<T> slerp2(
      mathkata::Quaternion<T>::slerp(qaa2, qaa3, 0.5));
  slerp2.toAngleAxis(&convertedAngle, &convertedAxis);
  EXPECT_NEAR(.5 * (angle2 + angle3), convertedAngle, precision);
  mathkata::Quaternion<T> slerp3(
      mathkata::Quaternion<T>::slerp(qaa2, qaa2, 0.5));
  slerp3.toAngleAxis(&convertedAngle, &convertedAxis);
  EXPECT_NEAR(angle2, convertedAngle, precision);
}
TEST_ALL_F(Mult)

// This tests that quat * scalar performs component-wise scaling, and is
// therefore associative and commutative with respect to real-number
// multiplication.
template <class T>
void MultQuatScalarComponentWise_Test(const T& precision) {
  (void)precision;
  using Quaternion = mathkata::Quaternion<T>;
  const double epsilon = 1e-5;

  const Quaternion q(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3),
                     static_cast<T>(4));

  // Component-wise scaling.
  const Quaternion expected(static_cast<T>(3), static_cast<T>(6),
                            static_cast<T>(9), static_cast<T>(12));
  EXPECT_NEAR_QUAT(expected, q * static_cast<T>(3), epsilon);

  // Commutativity: q * s == s * q.
  EXPECT_NEAR_QUAT(q * static_cast<T>(3), static_cast<T>(3) * q, epsilon);

  // Associativity: (q * a) * b == q * (a * b).
  EXPECT_NEAR_QUAT((q * static_cast<T>(2)) * static_cast<T>(3),
                   q * static_cast<T>(6), epsilon);

  // operator*= consistency.
  Quaternion q_mut = q;
  q_mut *= static_cast<T>(3);
  EXPECT_NEAR_QUAT(expected, q_mut, epsilon);
}
TEST_ALL_F(MultQuatScalarComponentWise)

// This tests that scaleAngle preserves the old angle-scaling behavior.
template <class T>
void scaleAngle_Test(const T& precision) {
  (void)precision;
  using Quaternion = mathkata::Quaternion<T>;
  using Vector3 = mathkata::Vector<T, 3>;
  const double epsilon = 1e-5;
  const Vector3 up(0, 1, 0);

  // scaleAngle(1) on a big quaternion should condition it to the short path.
  const Quaternion bigQuat =
      Quaternion::fromAngleAxis(static_cast<T>(mathkata::kPi * 1.5), up);
  EXPECT_NEAR_QUAT(Quaternion(-bigQuat.scalar(), -bigQuat.vector()),
                   bigQuat.scaleAngle(1), epsilon);

  // scaleAngle is not associative for factors > 1.
  const Quaternion base =
      Quaternion::fromAngleAxis(static_cast<T>(mathkata::kPi * .75), up);
  const Quaternion q1 = base.scaleAngle(2).scaleAngle(static_cast<T>(.5));
  const Quaternion q2 = base.scaleAngle(static_cast<T>(2 * .5));
  EXPECT_FALSE(IsNearOrientation(q1, q2, epsilon));

  // scaleAngle(0) should give identity.
  const Quaternion rot = Quaternion::fromAngleAxis(static_cast<T>(1.2), up);
  EXPECT_NEAR_QUAT(Quaternion::identity, rot.scaleAngle(0), epsilon);
}
TEST_ALL_F(scaleAngle)

// This tests that scalar multiplication distributes over quaternion addition.
template <class T>
void ScalarMultDistributesOverAdd_Test(const T& precision) {
  (void)precision;
  using Quaternion = mathkata::Quaternion<T>;
  const double epsilon = 1e-5;

  const Quaternion q1(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3),
                      static_cast<T>(4));
  const Quaternion q2(static_cast<T>(5), static_cast<T>(6), static_cast<T>(7),
                      static_cast<T>(8));
  const T s = static_cast<T>(2.5);

  // s * (q1 + q2) == s * q1 + s * q2
  EXPECT_NEAR_QUAT((q1 + q2) * s, q1 * s + q2 * s, epsilon);
}
TEST_ALL_F(ScalarMultDistributesOverAdd)

// This will test the dot product of quaternions.
template <class T>
void Dot_Test(const T& precision) {
  mathkata::Vector<T, 3> axis(static_cast<T>(4.3), static_cast<T>(7.6),
                              static_cast<T>(1.2));
  axis.normalize();
  T angle1 = static_cast<T>(1.2),
    angle2 = static_cast<T>(angle1 + std::numbers::pi_v<T> / 2),
    angle3 = static_cast<T>(angle1 + std::numbers::pi_v<T>),
    angle4 = static_cast<T>(0.7);
  mathkata::Quaternion<T> qaa1(
      mathkata::Quaternion<T>::fromAngleAxis(angle1, axis));
  mathkata::Quaternion<T> qaa2(
      mathkata::Quaternion<T>::fromAngleAxis(angle2, axis));
  mathkata::Quaternion<T> qaa3(
      mathkata::Quaternion<T>::fromAngleAxis(angle3, axis));
  mathkata::Quaternion<T> qaa4(
      mathkata::Quaternion<T>::fromAngleAxis(angle4, axis));

  // This will verify that Dotting two quaternions works correctly.
  EXPECT_NEAR(mathkata::Quaternion<T>::dotProduct(qaa1, qaa1), 1.0, precision);
  EXPECT_NEAR(mathkata::Quaternion<T>::dotProduct(qaa1, qaa2), sqrt(2.0) / 2.0,
              precision);
  EXPECT_NEAR(mathkata::Quaternion<T>::dotProduct(qaa1, qaa3), 0.0, precision);
  // 2 x acos(dot) should be the angle between two quaternions:
  EXPECT_NEAR(acos(mathkata::Quaternion<T>::dotProduct(qaa1, qaa4)) * 2.0,
              angle1 - angle4, precision);
}
TEST_ALL_F(Dot)

// This will test normalization of quaternions.
template <class T>
void normalize_Test(const T& precision) {
  mathkata::Quaternion<T> quat_1(static_cast<T>(12), static_cast<T>(0),
                                 static_cast<T>(0), static_cast<T>(0));
  const mathkata::Quaternion<T> const_quat_1 = quat_1;
  const mathkata::Quaternion<T> normalized_quat_1 = const_quat_1.normalized();
  quat_1.normalize();
  mathkata::Quaternion<T> reference_quat_1(static_cast<T>(1), static_cast<T>(0),
                                           static_cast<T>(0),
                                           static_cast<T>(0));
  EXPECT_NEAR(reference_quat_1[0], quat_1[0], precision);
  EXPECT_NEAR(reference_quat_1[1], quat_1[1], precision);
  EXPECT_NEAR(reference_quat_1[2], quat_1[2], precision);
  EXPECT_NEAR(reference_quat_1[3], quat_1[3], precision);
  EXPECT_NEAR(reference_quat_1[0], normalized_quat_1[0], precision);
  EXPECT_NEAR(reference_quat_1[1], normalized_quat_1[1], precision);
  EXPECT_NEAR(reference_quat_1[2], normalized_quat_1[2], precision);
  EXPECT_NEAR(reference_quat_1[3], normalized_quat_1[3], precision);

  mathkata::Quaternion<T> quat_2(static_cast<T>(123), static_cast<T>(123),
                                 static_cast<T>(123), static_cast<T>(123));
  mathkata::Quaternion<T> normalized_quat_2 = quat_2.normalized();
  quat_2.normalize();
  mathkata::Quaternion<T> reference_quat_2(
      static_cast<T>(sqrt(.25)), static_cast<T>(sqrt(.25)),
      static_cast<T>(sqrt(.25)), static_cast<T>(sqrt(.25)));
  EXPECT_NEAR(reference_quat_2[0], quat_2[0], precision);
  EXPECT_NEAR(reference_quat_2[1], quat_2[1], precision);
  EXPECT_NEAR(reference_quat_2[2], quat_2[2], precision);
  EXPECT_NEAR(reference_quat_2[3], quat_2[3], precision);
  EXPECT_NEAR(reference_quat_2[0], normalized_quat_2[0], precision);
  EXPECT_NEAR(reference_quat_2[1], normalized_quat_2[1], precision);
  EXPECT_NEAR(reference_quat_2[2], normalized_quat_2[2], precision);
  EXPECT_NEAR(reference_quat_2[3], normalized_quat_2[3], precision);
}
TEST_ALL_F(normalize)

// This tests that toAngleAxis returns angle <= 180 degrees, even if the
// Quaternion had a larger angle.
template <class T>
void ToAngleAxisReturnsSmallQuat_Test(const T& precision) {
  (void)precision;
  using Quaternion = mathkata::Quaternion<T>;
  using Vector3 = mathkata::Vector<T, 3>;
  const float epsilon = 1e-5f;

  // Test the specific example called out in the documentation:
  // "For example, if *this represents "rotate 350 degrees left", you will
  //  get the angle-axis "rotate 10 degrees right"."
  const Vector3 kUp(0, 1, 0);
  const float k350Degrees = 350 * mathkata::kDegreesToRadians;
  const Quaternion k350Left = Quaternion::fromAngleAxis(k350Degrees, kUp);

  const Vector3 kDown(0, -1, 0);
  const float k10Degrees = 10 * mathkata::kDegreesToRadians;
  const Quaternion k10Right = Quaternion::fromAngleAxis(k10Degrees, kDown);

  {
    T angle;
    auto axis = Vector3::uninitialized();
    k350Left.toAngleAxis(&angle, &axis);
    EXPECT_NEAR(k10Degrees, angle, epsilon);
    EXPECT_NEAR_VEC3(kDown, axis, epsilon);
    EXPECT_NEAR_QUAT(k10Right, Quaternion::fromAngleAxis(angle, axis), epsilon);
  }
  {
    T angle;
    auto axis = Vector3::uninitialized();
    k350Left.toAngleAxisFull(&angle, &axis);
    EXPECT_NEAR(k350Degrees, angle, epsilon);
    EXPECT_NEAR_VEC3(kUp, axis, epsilon);
    EXPECT_NEAR_QUAT(k350Left, Quaternion::fromAngleAxis(angle, axis), epsilon);
  }
}
TEST_ALL_F(ToAngleAxisReturnsSmallQuat)

// This will test normalization of quaternions.
template <class T>
void rotateFromTo_Test(const T& precision) {
  mathkata::Vector<T, 3> x_axis = mathkata::Vector<T, 3>(
      static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));
  mathkata::Vector<T, 3> y_axis = mathkata::Vector<T, 3>(
      static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));
  mathkata::Vector<T, 3> z_axis = mathkata::Vector<T, 3>(
      static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

  mathkata::Quaternion<T> x_to_y =
      mathkata::Quaternion<T>::rotateFromTo(x_axis, y_axis);
  mathkata::Quaternion<T> y_to_z =
      mathkata::Quaternion<T>::rotateFromTo(y_axis, z_axis);
  mathkata::Quaternion<T> z_to_x =
      mathkata::Quaternion<T>::rotateFromTo(z_axis, x_axis);

  // Check some axis rotations:
  // By definition, rotateFromTo(v1, v2) * v2 should always equal v2.
  // if v1 and v2 are 90 degrees apart (as they are in the case of axes)
  // then applying the same rotation twice should invert the vector.
  mathkata::Vector<T, 3> x_to_y_result = x_to_y.rotate(x_axis);
  mathkata::Vector<T, 3> x_to_y_twice_result = (x_to_y * x_to_y).rotate(x_axis);
  EXPECT_NEAR_VEC3(x_to_y_result, y_axis, precision);
  EXPECT_NEAR_VEC3(x_to_y_twice_result, -x_axis, precision);

  mathkata::Vector<T, 3> y_to_z_result = y_to_z.rotate(y_axis);
  mathkata::Vector<T, 3> y_to_z_twice_result = (y_to_z * y_to_z).rotate(y_axis);
  EXPECT_NEAR_VEC3(y_to_z_result, z_axis, precision);
  EXPECT_NEAR_VEC3(y_to_z_twice_result, -y_axis, precision);

  mathkata::Vector<T, 3> z_to_x_result = z_to_x.rotate(z_axis);
  mathkata::Vector<T, 3> z_to_x_twice_result = (z_to_x * z_to_x).rotate(z_axis);
  EXPECT_NEAR_VEC3(z_to_x_result, x_axis, precision);
  EXPECT_NEAR_VEC3(z_to_x_twice_result, -z_axis, precision);

  // Try some weirder vectors:
  mathkata::Vector<T, 3> arbitrary_1 = mathkata::Vector<T, 3>(
      static_cast<T>(2), static_cast<T>(-5), static_cast<T>(9));
  mathkata::Vector<T, 3> arbitrary_2 = mathkata::Vector<T, 3>(
      static_cast<T>(-1), static_cast<T>(3), static_cast<T>(16));

  mathkata::Quaternion<T> arbitrary_to_arbitrary =
      mathkata::Quaternion<T>::rotateFromTo(arbitrary_1, arbitrary_2);

  mathkata::Vector<T, 3> arbitrary_1_to_2 =
      arbitrary_to_arbitrary.rotate(arbitrary_1);
  arbitrary_1_to_2.normalize();
  mathkata::Vector<T, 3> arbitrary_2_normalized = arbitrary_2.normalized();

  EXPECT_NEAR_VEC3(arbitrary_1_to_2, arbitrary_2_normalized, precision);

  // Using rotateFromTo on one vector should give us the identity quaternion:
  mathkata::Quaternion<T> identity =
      mathkata::Quaternion<T>::rotateFromTo(arbitrary_1, arbitrary_1);

  mathkata::Vector<T, 3> arbitrary_2_identity = identity.rotate(arbitrary_2);
  EXPECT_NEAR_VEC3(arbitrary_2_identity, arbitrary_2, precision);

  // Using rotateFromTo on an inverted vector should give a 180 degree rotation:
  mathkata::Quaternion<T> reverse =
      mathkata::Quaternion<T>::rotateFromTo(arbitrary_1, -arbitrary_1);

  // Relaxing the precision slightly, because there are a lot of chained
  // float operations in here.
  mathkata::Vector<T, 3> arbitrary_1_reversed = reverse.rotate(arbitrary_1);
  EXPECT_NEAR_VEC3(arbitrary_1_reversed, -arbitrary_1, precision * 2.0);
}
TEST_ALL_F(rotateFromTo)

// Test the compilation of basic quaternion operations given in the sample
// file. This will test interpolating two rotations.
TEST_F(QuaternionTests, QuaternionSample) {
  using namespace mathkata;
  /// @doxysnippetstart Chapter03_Quaternions.md Quaternion_Sample
  // Use radians for angles
  Vector<float, 3> angles1(0.66f, 1.3f, 0.76f);
  Vector<float, 3> angles2(0.85f, 0.33f, 1.6f);

  Quaternion<float> quat1 = Quaternion<float>::fromEulerAngles(angles1);
  Quaternion<float> quat2 = Quaternion<float>::fromEulerAngles(angles2);

  Quaternion<float> quatSlerp = Quaternion<float>::slerp(quat1, quat2, 0.5);
  Vector<float, 3> angleSlerp = quatSlerp.toEulerAngles();
  /// @doxysnippetend
  const float precision = 1e-2f;
  EXPECT_NEAR(0.93f, angleSlerp[0], precision);
  EXPECT_NEAR(0.82f, angleSlerp[1], precision);
  EXPECT_NEAR(1.33f, angleSlerp[2], precision);
}

// Test that the quaternion identity constants give the identity transform.
TEST_F(QuaternionTests, IdentityConst) {
  EXPECT_EQ_QUAT(mathkata::kQuatIdentityf,
                 mathkata::Quaternion<float>::identity);
  EXPECT_EQ_QUAT(mathkata::kQuatIdentityf,
                 mathkata::Quaternion<float>(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(mathkata::kQuatIdentityf.toEulerAngles(), mathkata::kZeros3f);

  EXPECT_EQ_QUAT(mathkata::kQuatIdentityd,
                 mathkata::Quaternion<double>::identity);
  EXPECT_EQ_QUAT(mathkata::kQuatIdentityd,
                 mathkata::Quaternion<double>(1.0, 0.0, 0.0, 0.0));
  EXPECT_EQ(mathkata::kQuatIdentityd.toEulerAngles(), mathkata::kZeros3d);
}

template <class T>
void OutputStream_Test(const T&) {
  mathkata::Quaternion<T> q =
      mathkata::Quaternion<T>(static_cast<T>(1), static_cast<T>(2),
                              static_cast<T>(3), static_cast<T>(4));
  std::stringstream ss;
  ss << q;
  EXPECT_EQ("(1, 2, 3, 4)", ss.str());
}
TEST_ALL_F(OutputStream)

template <class T>
void lookAt_Test(const T& precision) {
  using Quaternion = mathkata::Quaternion<T>;
  using Vector3 = mathkata::Vector<T, 3>;
  constexpr auto kRH = mathkata::Handedness::kRightHanded;
  constexpr auto kLH = mathkata::Handedness::kLeftHanded;
  const T one = static_cast<T>(1);
  const T neg_one = static_cast<T>(-1);
  const T zero = static_cast<T>(0);
  const double epsilon = static_cast<double>(precision) * 10;
  const Vector3 up(zero, one, zero);

  // The default forward direction for right-handed coordinates is -Z,
  // and for left-handed coordinates is +Z.

  // ---- Right-handed (default) ----

  // Looking along -Z (RH default forward) should give identity.
  {
    const Quaternion q =
        Quaternion::template lookAt<kRH>(Vector3(zero, zero, neg_one), up);
    EXPECT_NEAR_ORIENTATION(Quaternion::identity, q, epsilon);
  }

  // Looking along +Z (opposite of RH default forward) should give a
  // 180-degree rotation around the Y axis.
  {
    const Quaternion q =
        Quaternion::template lookAt<kRH>(Vector3(zero, zero, one), up);
    const Quaternion expected =
        Quaternion::fromAngleAxis(std::numbers::pi_v<T>, up);
    EXPECT_NEAR_ORIENTATION(expected, q, epsilon);
  }

  // Looking along +X should give a 90-degree rotation around Y (turning
  // from -Z forward to +X).
  {
    const Quaternion q =
        Quaternion::template lookAt<kRH>(Vector3(one, zero, zero), up);
    const Quaternion expected =
        Quaternion::fromAngleAxis(-std::numbers::pi_v<T> / 2, up);
    EXPECT_NEAR_ORIENTATION(expected, q, epsilon);
  }

  // Explicit right-handed should match the default (no handedness argument).
  {
    const Quaternion q_default =
        Quaternion::lookAt(Vector3(one, zero, zero), up);
    const Quaternion q_explicit =
        Quaternion::template lookAt<kRH>(Vector3(one, zero, zero), up);
    EXPECT_NEAR_QUAT(q_default, q_explicit, epsilon);
  }

  // ---- Left-handed ----

  // Looking along +Z (LH default forward) should give identity.
  {
    const Quaternion q =
        Quaternion::template lookAt<kLH>(Vector3(zero, zero, one), up);
    EXPECT_NEAR_ORIENTATION(Quaternion::identity, q, epsilon);
  }

  // Looking along -Z (opposite of LH default forward) should give a
  // 180-degree rotation around the Y axis.
  {
    const Quaternion q =
        Quaternion::template lookAt<kLH>(Vector3(zero, zero, neg_one), up);
    const Quaternion expected =
        Quaternion::fromAngleAxis(std::numbers::pi_v<T>, up);
    EXPECT_NEAR_ORIENTATION(expected, q, epsilon);
  }

  // Looking along +X (LH) should give a 90-degree rotation around Y (turning
  // from +Z forward to +X).
  {
    const Quaternion q =
        Quaternion::template lookAt<kLH>(Vector3(one, zero, zero), up);
    const Quaternion expected =
        Quaternion::fromAngleAxis(std::numbers::pi_v<T> / 2, up);
    EXPECT_NEAR_ORIENTATION(expected, q, epsilon);
  }

  // ---- Functional tests: applying the quaternion to the default forward
  //      direction should yield the target forward direction ----

  // RH default forward is -Z.
  {
    const Vector3 rh_forward(zero, zero, neg_one);
    const Vector3 target(one, zero, zero);
    const Quaternion q = Quaternion::template lookAt<kRH>(target, up);
    const Vector3 result = q.rotate(rh_forward);
    EXPECT_NEAR_VEC3(target, result, epsilon);
  }

  // LH default forward is +Z.
  {
    const Vector3 lh_forward(zero, zero, one);
    const Vector3 target(one, zero, zero);
    const Quaternion q = Quaternion::template lookAt<kLH>(target, up);
    const Vector3 result = q.rotate(lh_forward);
    EXPECT_NEAR_VEC3(target, result, epsilon);
  }

  // ---- Unit length: the result should always be a unit quaternion ----
  {
    const Vector3 directions[] = {
        Vector3(one, zero, zero),
        Vector3(zero, zero, one),
        Vector3(zero, zero, neg_one),
        Vector3(neg_one, zero, zero),
        Vector3(one, one, zero).normalized(),
        Vector3(one, zero, one).normalized(),
    };
    for (const auto& dir : directions) {
      const Quaternion q_rh = Quaternion::template lookAt<kRH>(dir, up);
      T length_rh = Quaternion::dotProduct(q_rh, q_rh);
      EXPECT_NEAR(static_cast<T>(1), length_rh, epsilon);

      const Quaternion q_lh = Quaternion::template lookAt<kLH>(dir, up);
      T length_lh = Quaternion::dotProduct(q_lh, q_lh);
      EXPECT_NEAR(static_cast<T>(1), length_lh, epsilon);
    }
  }
}
TEST_ALL_F(lookAt)

template <class T>
void FromEulerAnglesSplit_Test(const T& precision) {
  mathkata::Vector<T, 3> eulers(static_cast<T>(0.1), static_cast<T>(0.2),
                                static_cast<T>(0.3));
  EXPECT_NEAR_QUAT(
      mathkata::Quaternion<T>::fromEulerAngles(eulers),
      mathkata::Quaternion<T>::fromEulerAngles(eulers[0], eulers[1], eulers[2]),
      precision);
}
TEST_ALL_F(FromEulerAnglesSplit)

const float kSlerpTestAnglesInDegrees[]{
    // slerp algorithms commonly have trouble with angles near zero.
    // To give a sense of what that means for common quaternion-dot cutoffs:
    // - Quaternion dot of .99999 = .512 degrees
    // - Quaternion dot of .9999 = 1.62 degrees
    // - Quaternion dot of .9995 = 3.62 degrees
    0,
    .5f,
    1.5f,
    3.5f,
    80,
    // 180 has no numerical problems, unless there's a bug. But worth checking.
    179,
    180,
    181,
    // slerp is ill-defined at angles near 360.
    359,
    359.5f,
    360,
    360.5f,
    361,
};

// Tests that slerp returns unit-length quaternions.
template <class T>
void SlerpResultIsUnit_Test(const T& precision) {
  (void)precision;
  using Quaternion = mathkata::Quaternion<T>;

  const mathkata::Vector<T, 3> axis(0, 1, 0);
  const float kLengthEpsilon = 5e-6f;

  for (float angle : kSlerpTestAnglesInDegrees) {
    const Quaternion q2 =
        Quaternion::fromAngleAxis(angle * mathkata::kDegreesToRadians, axis);

    Quaternion slerp_result = Quaternion::slerp(Quaternion::identity, q2, .5f);
    const T slerp_length = slerp_result.normalize();
    EXPECT_NEAR(1.0f, slerp_length, kLengthEpsilon) << " for angle " << angle;

    // Alternate spelling for slerp
    Quaternion scale_result = q2.scaleAngle(static_cast<T>(.5));
    const T scale_length = scale_result.normalize();
    EXPECT_NEAR(1.0f, scale_length, kLengthEpsilon) << " for angle " << angle;
  }
}
TEST_ALL_F(SlerpResultIsUnit)

// Checks equality of
// - quat(<some axis>, expected_angle) vs
// - slerp(identity, quat(<some axis>, angle), t) vs
// - slerp(quat(<some axis>, angle), identity, 1-t)
// Angles are in degrees.
template <class T>
void CheckSlerp(float angle, float t, float expected_angle) {
  using Quaternion = mathkata::Quaternion<T>;
  using Vector3 = mathkata::Vector<T, 3>;

  // Transcendentals are involved, so be lenient on the epsilon.
  const T epsilon = 1e-6f;
  const Vector3 up(0, 1, 0);  // Could be any axis, really.
  const Quaternion original =
      Quaternion::fromAngleAxis(angle * mathkata::kDegreesToRadians, up);
  const Quaternion expected = Quaternion::fromAngleAxis(
      expected_angle * mathkata::kDegreesToRadians, up);

  // These are looser EXPECT_NEAR_ORIENTATION checks because slerp() treats
  // quats as orientations. For checking a mathematical slerp(), they can
  // (and should) be tightened back to EXPECT_NEAR_QUAT.

  Quaternion slerp_result =
      Quaternion::slerp(Quaternion::identity, original, t);
  EXPECT_NEAR_ORIENTATION(expected, slerp_result, epsilon)
      << " for angle " << angle << " and t " << t;

  // Apply the invariant that slerp(a, b, t) == slerp(b, a, 1-t).
  Quaternion slerp_backwards_result =
      Quaternion::slerp(original, Quaternion::identity, 1 - t);
  EXPECT_NEAR_ORIENTATION(expected, slerp_backwards_result, epsilon)
      << " for angle " << angle << " and t " << t;

  Quaternion scale_result = original.scaleAngle(t);
  EXPECT_NEAR_ORIENTATION(expected, scale_result, epsilon)
      << " for angle " << angle << " and t " << t;
}

// This doubles as a test of both slerp() and operator*(quat, float),
// since the two are pretty much the same operation with different spelling.
template <class T>
void slerp_Test(const T& precision) {
  (void)precision;
  // Easy and unambiguous cases.
  CheckSlerp<T>(+160, 0.375f, +60);
  CheckSlerp<T>(-160, 0.375f, -60);

  // Shortening a "long way around" (> 180 degree) rotation
  // NOTE: These results are different from the mathematical quat slerp
  CheckSlerp<T>(+320, 0.375f, -15);  // Mathematically, should be +120
  CheckSlerp<T>(-320, 0.375f, +15);  // Mathematically, should be -120

  // Lengthening a "long way around" rotation
  CheckSlerp<T>(320, 1.5f, -60);  // Mathematically, should be 480 (ie -240)

  // Lengthening to a "long way around" (> 180 degree) rotation
  CheckSlerp<T>(+70, 3, +210);
  CheckSlerp<T>(-70, 3, -210);

  // An edge case that often causes NaNs
  CheckSlerp<T>(0, .5f, 0);

  // This edge case is ill-defined for "intuitive" slerp and can't be tested.
  // CheckSlerp<T>(180, .25f, 45);

  // Conversely, this edge case is well-defined for "intuitive" slerp.
  // For mathematical slerp, the axis is ill-defined and can take many values.
  CheckSlerp<T>(360, .25f, 0);
}
TEST_ALL_F(slerp)

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

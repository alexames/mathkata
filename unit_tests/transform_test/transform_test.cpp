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
#include "mathkata/transform.h"

#include <cmath>
#include <numbers>

#include "gtest/gtest.h"
#include "precision.h"

namespace {

class TransformTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// This will automatically generate tests for each template parameter.
#define TEST_ALL_F(MY_TEST)                   \
  TEST_F(TransformTests, MY_TEST) {           \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

// Helper: check if two vectors are approximately equal.
template <class T>
bool NearVector(const mathkata::Vector<T, 3>& v1,
                const mathkata::Vector<T, 3>& v2, T abs_error) {
  for (int i = 0; i < 3; ++i) {
    if (std::fabs(static_cast<double>(v1[i] - v2[i]))
        > static_cast<double>(abs_error)) {
      return false;
    }
  }
  return true;
}

// Helper: check if two 4x4 matrices are approximately equal.
template <class T>
bool NearMatrix(const mathkata::Matrix<T, 4>& m1,
                const mathkata::Matrix<T, 4>& m2, T abs_error) {
  for (int i = 0; i < 16; ++i) {
    if (std::fabs(static_cast<double>(m1[i] - m2[i]))
        > static_cast<double>(abs_error)) {
      return false;
    }
  }
  return true;
}

// Test: Default constructor produces identity transform.
template <class T>
void DefaultConstruction_Test(T precision) {
  mathkata::Transform<T> t;
  EXPECT_TRUE(NearVector(t.position, mathkata::Vector<T, 3>(T(0), T(0), T(0)),
                         precision))
      << "Default position should be zero.";
  EXPECT_EQ(t.rotation, mathkata::Quaternion<T>::identity)
      << "Default rotation should be identity.";
  EXPECT_TRUE(
      NearVector(t.scale, mathkata::Vector<T, 3>(T(1), T(1), T(1)), precision))
      << "Default scale should be one.";
}
TEST_ALL_F(DefaultConstruction)

// Test: Position-only constructor.
template <class T>
void PositionOnlyConstruction_Test(T precision) {
  mathkata::Vector<T, 3> pos(T(1), T(2), T(3));
  mathkata::Transform<T> t(pos);
  EXPECT_TRUE(NearVector(t.position, pos, precision));
  EXPECT_EQ(t.rotation, mathkata::Quaternion<T>::identity);
  EXPECT_TRUE(
      NearVector(t.scale, mathkata::Vector<T, 3>(T(1), T(1), T(1)), precision));
}
TEST_ALL_F(PositionOnlyConstruction)

// Test: Position + rotation constructor.
template <class T>
void PositionRotationConstruction_Test(T precision) {
  mathkata::Vector<T, 3> pos(T(4), T(5), T(6));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 4, mathkata::Vector<T, 3>(T(0), T(1), T(0)));
  mathkata::Transform<T> t(pos, rot);
  EXPECT_TRUE(NearVector(t.position, pos, precision));
  EXPECT_EQ(t.rotation, rot);
  EXPECT_TRUE(
      NearVector(t.scale, mathkata::Vector<T, 3>(T(1), T(1), T(1)), precision));
}
TEST_ALL_F(PositionRotationConstruction)

// Test: Full constructor (position + rotation + scale).
template <class T>
void FullConstruction_Test(T precision) {
  mathkata::Vector<T, 3> pos(T(1), T(2), T(3));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 2, mathkata::Vector<T, 3>(T(0), T(0), T(1)));
  mathkata::Vector<T, 3> scl(T(2), T(3), T(4));
  mathkata::Transform<T> t(pos, rot, scl);
  EXPECT_TRUE(NearVector(t.position, pos, precision));
  EXPECT_EQ(t.rotation, rot);
  EXPECT_TRUE(NearVector(t.scale, scl, precision));
}
TEST_ALL_F(FullConstruction)

// Test: ToMatrix produces the expected TRS matrix.
template <class T>
void ToMatrix_Test(T precision) {
  mathkata::Vector<T, 3> pos(T(10), T(20), T(30));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 2, mathkata::Vector<T, 3>(T(0), T(1), T(0)));
  mathkata::Vector<T, 3> scl(T(2), T(3), T(4));
  mathkata::Transform<T> t(pos, rot, scl);

  mathkata::Matrix<T, 4> result = t.ToMatrix();
  mathkata::Matrix<T, 4> expected =
      mathkata::Matrix<T, 4>::Transform(pos, rot.ToMatrix(), scl);
  EXPECT_TRUE(NearMatrix(result, expected, precision))
      << "ToMatrix should match Matrix::Transform.";
}
TEST_ALL_F(ToMatrix)

// Test: TransformPoint applies scale, rotate, then translate.
template <class T>
void TransformPoint_Test(T precision) {
  // A transform that scales by 2 along x, rotates 90 degrees around Z,
  // and translates by (10, 0, 0).
  mathkata::Vector<T, 3> pos(T(10), T(0), T(0));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 2, mathkata::Vector<T, 3>(T(0), T(0), T(1)));
  mathkata::Vector<T, 3> scl(T(2), T(1), T(1));
  mathkata::Transform<T> t(pos, rot, scl);

  // Point (1, 0, 0):
  // 1. Scale: (2, 0, 0)
  // 2. Rotate 90 deg around Z: (0, 2, 0)
  // 3. Translate: (10, 2, 0)
  mathkata::Vector<T, 3> point(T(1), T(0), T(0));
  mathkata::Vector<T, 3> result = t.TransformPoint(point);
  mathkata::Vector<T, 3> expected(T(10), T(2), T(0));
  EXPECT_TRUE(NearVector(result, expected, precision))
      << "TransformPoint should apply scale, rotate, translate.";
}
TEST_ALL_F(TransformPoint)

// Test: TransformDirection applies rotation and scale but not translation.
template <class T>
void TransformDirection_Test(T precision) {
  mathkata::Vector<T, 3> pos(T(100), T(200), T(300));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 2, mathkata::Vector<T, 3>(T(0), T(0), T(1)));
  mathkata::Vector<T, 3> scl(T(2), T(1), T(1));
  mathkata::Transform<T> t(pos, rot, scl);

  // Direction (1, 0, 0):
  // 1. Scale: (2, 0, 0)
  // 2. Rotate 90 deg around Z: (0, 2, 0)
  // No translation applied.
  mathkata::Vector<T, 3> dir(T(1), T(0), T(0));
  mathkata::Vector<T, 3> result = t.TransformDirection(dir);
  mathkata::Vector<T, 3> expected(T(0), T(2), T(0));
  EXPECT_TRUE(NearVector(result, expected, precision))
      << "TransformDirection should not apply translation.";
}
TEST_ALL_F(TransformDirection)

// Test: Inverse produces a transform that composes to identity (uniform scale).
template <class T>
void Inverse_Test(T precision) {
  mathkata::Vector<T, 3> pos(T(3), T(-1), T(7));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 3, mathkata::Vector<T, 3>(T(0), T(1), T(0)));
  mathkata::Vector<T, 3> scl(T(2), T(2), T(2));
  mathkata::Transform<T> t(pos, rot, scl);

  mathkata::Transform<T> inv = t.Inverse();
  mathkata::Transform<T> composed = t * inv;

  // The composed transform should be approximately identity.
  mathkata::Vector<T, 3> zero(T(0), T(0), T(0));
  mathkata::Vector<T, 3> one(T(1), T(1), T(1));
  T relaxed = precision * T(100);
  EXPECT_TRUE(NearVector(composed.position, zero, relaxed))
      << "t * t.Inverse() position should be zero.";
  EXPECT_TRUE(NearVector(composed.scale, one, relaxed))
      << "t * t.Inverse() scale should be one.";

  // Check rotation is identity (scalar ~1, vector ~0).
  EXPECT_NEAR(static_cast<double>(std::fabs(composed.rotation.scalar())), 1.0,
              static_cast<double>(relaxed));
  EXPECT_NEAR(static_cast<double>(composed.rotation.vector()[0]), 0.0,
              static_cast<double>(relaxed));
  EXPECT_NEAR(static_cast<double>(composed.rotation.vector()[1]), 0.0,
              static_cast<double>(relaxed));
  EXPECT_NEAR(static_cast<double>(composed.rotation.vector()[2]), 0.0,
              static_cast<double>(relaxed));
}
TEST_ALL_F(Inverse)

// Test: Inverse via point round-trip (uniform scale).
template <class T>
void InversePointRoundTrip_Test(T precision) {
  mathkata::Vector<T, 3> pos(T(5), T(-2), T(8));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 6, mathkata::Vector<T, 3>(T(1), T(0), T(0)));
  mathkata::Vector<T, 3> scl(T(3), T(3), T(3));
  mathkata::Transform<T> t(pos, rot, scl);
  mathkata::Transform<T> inv = t.Inverse();

  mathkata::Vector<T, 3> original(T(7), T(-3), T(4));
  mathkata::Vector<T, 3> transformed = t.TransformPoint(original);
  mathkata::Vector<T, 3> recovered = inv.TransformPoint(transformed);

  T relaxed = precision * T(100);
  EXPECT_TRUE(NearVector(recovered, original, relaxed))
      << "Inverse transform should recover the original point.";
}
TEST_ALL_F(InversePointRoundTrip)

// Test: Inverse with unit scale and rotation.
template <class T>
void InverseUnitScale_Test(T precision) {
  mathkata::Vector<T, 3> pos(T(10), T(-5), T(3));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 4,
      mathkata::Vector<T, 3>(T(1), T(1), T(0)).Normalized());
  mathkata::Transform<T> t(pos, rot);
  mathkata::Transform<T> inv = t.Inverse();

  mathkata::Vector<T, 3> original(T(1), T(2), T(3));
  mathkata::Vector<T, 3> transformed = t.TransformPoint(original);
  mathkata::Vector<T, 3> recovered = inv.TransformPoint(transformed);

  T relaxed = precision * T(10);
  EXPECT_TRUE(NearVector(recovered, original, relaxed))
      << "Inverse with unit scale should recover the original point.";
}
TEST_ALL_F(InverseUnitScale)

// Test: Composition (operator*) applies rhs first, then lhs.
template <class T>
void Composition_Test(T precision) {
  // Create two transforms and compose them.
  mathkata::Vector<T, 3> pos1(T(1), T(0), T(0));
  mathkata::Quaternion<T> rot1 = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 2, mathkata::Vector<T, 3>(T(0), T(0), T(1)));
  mathkata::Transform<T> t1(pos1, rot1);

  mathkata::Vector<T, 3> pos2(T(0), T(1), T(0));
  mathkata::Transform<T> t2(pos2);

  // t1 * t2 should first apply t2, then t1.
  mathkata::Transform<T> composed = t1 * t2;

  // Verify by transforming a point.
  mathkata::Vector<T, 3> point(T(0), T(0), T(0));

  // Through composition:
  mathkata::Vector<T, 3> result_composed = composed.TransformPoint(point);

  // Step by step: first t2, then t1:
  mathkata::Vector<T, 3> after_t2 = t2.TransformPoint(point);
  mathkata::Vector<T, 3> result_stepwise = t1.TransformPoint(after_t2);

  T relaxed = precision * T(10);
  EXPECT_TRUE(NearVector(result_composed, result_stepwise, relaxed))
      << "Composition should match sequential application.";
}
TEST_ALL_F(Composition)

// Test: Composition matches matrix multiplication (uniform scale).
template <class T>
void CompositionMatchesMatrix_Test(T precision) {
  mathkata::Vector<T, 3> pos1(T(2), T(-1), T(3));
  mathkata::Quaternion<T> rot1 = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 4, mathkata::Vector<T, 3>(T(0), T(1), T(0)));
  mathkata::Vector<T, 3> scl1(T(2), T(2), T(2));
  mathkata::Transform<T> t1(pos1, rot1, scl1);

  mathkata::Vector<T, 3> pos2(T(-1), T(0), T(5));
  mathkata::Quaternion<T> rot2 = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 6, mathkata::Vector<T, 3>(T(1), T(0), T(0)));
  mathkata::Vector<T, 3> scl2(T(3), T(3), T(3));
  mathkata::Transform<T> t2(pos2, rot2, scl2);

  // Compose transforms.
  mathkata::Transform<T> composed = t1 * t2;
  mathkata::Matrix<T, 4> composed_matrix = composed.ToMatrix();

  // Multiply matrices directly.
  mathkata::Matrix<T, 4> expected_matrix = t1.ToMatrix() * t2.ToMatrix();

  T relaxed = precision * T(100);
  EXPECT_TRUE(NearMatrix(composed_matrix, expected_matrix, relaxed))
      << "Composed transform matrix should match multiplied matrices.";
}
TEST_ALL_F(CompositionMatchesMatrix)

// Test: Static Identity returns identity transform.
template <class T>
void StaticIdentity_Test(T precision) {
  mathkata::Transform<T> id = mathkata::Transform<T>::Identity();
  EXPECT_TRUE(NearVector(id.position, mathkata::Vector<T, 3>(T(0), T(0), T(0)),
                         precision));
  EXPECT_EQ(id.rotation, mathkata::Quaternion<T>::identity);
  EXPECT_TRUE(NearVector(id.scale, mathkata::Vector<T, 3>(T(1), T(1), T(1)),
                         precision));
}
TEST_ALL_F(StaticIdentity)

// Test: Equality operator.
template <class T>
void Equality_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, 3> pos(T(1), T(2), T(3));
  mathkata::Quaternion<T> rot = mathkata::Quaternion<T>::FromAngleAxis(
      std::numbers::pi_v<T> / 4, mathkata::Vector<T, 3>(T(0), T(1), T(0)));
  mathkata::Vector<T, 3> scl(T(2), T(3), T(4));

  mathkata::Transform<T> t1(pos, rot, scl);
  mathkata::Transform<T> t2(pos, rot, scl);
  mathkata::Transform<T> t3;

  EXPECT_TRUE(t1 == t2) << "Identical transforms should be equal.";
  EXPECT_FALSE(t1 == t3) << "Different transforms should not be equal.";
  EXPECT_FALSE(t1 != t2) << "Identical transforms should not be not-equal.";
  EXPECT_TRUE(t1 != t3) << "Different transforms should be not-equal.";
}
TEST_ALL_F(Equality)

// Test: Identity transform does not change a point.
template <class T>
void IdentityTransformPoint_Test(T precision) {
  mathkata::Transform<T> id = mathkata::Transform<T>::Identity();
  mathkata::Vector<T, 3> point(T(7), T(-3), T(11));
  mathkata::Vector<T, 3> result = id.TransformPoint(point);
  EXPECT_TRUE(NearVector(result, point, precision))
      << "Identity should not change a point.";
}
TEST_ALL_F(IdentityTransformPoint)

// Test: ToMatrix for identity transform produces identity matrix.
template <class T>
void IdentityToMatrix_Test(T precision) {
  mathkata::Transform<T> id = mathkata::Transform<T>::Identity();
  mathkata::Matrix<T, 4> result = id.ToMatrix();
  mathkata::Matrix<T, 4> expected = mathkata::Matrix<T, 4>::Identity();
  EXPECT_TRUE(NearMatrix(result, expected, precision))
      << "Identity transform should produce identity matrix.";
}
TEST_ALL_F(IdentityToMatrix)

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

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
#include "mathkata/capsule.h"

#include <cmath>

#include "gtest/gtest.h"
#include "precision.h"

class CapsuleTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Macro to generate tests for float/double and 2D/3D combinations.
#define TEST_ALL_CAPSULE_F(MY_TEST)              \
  TEST_F(CapsuleTests, MY_TEST##_float_2) {      \
    MY_TEST##_Test<float, 2>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(CapsuleTests, MY_TEST##_double_2) {     \
    MY_TEST##_Test<double, 2>(DOUBLE_PRECISION); \
  }                                              \
  TEST_F(CapsuleTests, MY_TEST##_float_3) {      \
    MY_TEST##_Test<float, 3>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(CapsuleTests, MY_TEST##_double_3) {     \
    MY_TEST##_Test<double, 3>(DOUBLE_PRECISION); \
  }

// Helper to create a 2D or 3D vector with the first two components set.
template <class T, int N>
mathkata::Vector<T, N> MakeVec(T x, T y);

template <>
mathkata::Vector<float, 2> MakeVec<float, 2>(float x, float y) {
  return mathkata::Vector<float, 2>(x, y);
}

template <>
mathkata::Vector<double, 2> MakeVec<double, 2>(double x, double y) {
  return mathkata::Vector<double, 2>(x, y);
}

template <>
mathkata::Vector<float, 3> MakeVec<float, 3>(float x, float y) {
  return mathkata::Vector<float, 3>(x, y, 0.0f);
}

template <>
mathkata::Vector<double, 3> MakeVec<double, 3>(double x, double y) {
  return mathkata::Vector<double, 3>(x, y, 0.0);
}

// Helper to create a vector with a z component (only meaningful for 3D).
template <class T, int N>
mathkata::Vector<T, N> MakeVec3(T x, T y, T z);

template <>
mathkata::Vector<float, 2> MakeVec3<float, 2>(float x, float y, float) {
  return mathkata::Vector<float, 2>(x, y);
}

template <>
mathkata::Vector<double, 2> MakeVec3<double, 2>(double x, double y, double) {
  return mathkata::Vector<double, 2>(x, y);
}

template <>
mathkata::Vector<float, 3> MakeVec3<float, 3>(float x, float y, float z) {
  return mathkata::Vector<float, 3>(x, y, z);
}

template <>
mathkata::Vector<double, 3> MakeVec3<double, 3>(double x, double y, double z) {
  return mathkata::Vector<double, 3>(x, y, z);
}

// Test construction from start, end, and radius.
template <class T, int N>
void Construction_Test(T precision) {
  (void)precision;
  const auto start = MakeVec<T, N>(static_cast<T>(1), static_cast<T>(2));
  const auto end = MakeVec<T, N>(static_cast<T>(4), static_cast<T>(6));
  const T radius = static_cast<T>(0.5);

  mathkata::Capsule<T, N> capsule(start, end, radius);

  EXPECT_EQ(capsule.start, start);
  EXPECT_EQ(capsule.end, end);
  EXPECT_EQ(capsule.radius, radius);
}
TEST_ALL_CAPSULE_F(Construction)

// Test default construction compiles (values are uninitialized).
template <class T, int N>
void DefaultConstruction_Test(T precision) {
  (void)precision;
  mathkata::Capsule<T, N> capsule;
  // Just verify it compiles and doesn't crash. Values are indeterminate.
  (void)capsule;
}
TEST_ALL_CAPSULE_F(DefaultConstruction)

// Test Center() returns the midpoint of start and end.
template <class T, int N>
void Center_Test(T precision) {
  const auto start = MakeVec<T, N>(static_cast<T>(0), static_cast<T>(0));
  const auto end = MakeVec<T, N>(static_cast<T>(4), static_cast<T>(6));
  const T radius = static_cast<T>(1);

  mathkata::Capsule<T, N> capsule(start, end, radius);
  const auto center = capsule.Center();

  const auto expected = MakeVec<T, N>(static_cast<T>(2), static_cast<T>(3));
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(center[i], expected[i], precision);
  }
}
TEST_ALL_CAPSULE_F(Center)

// Test Center() with non-origin start.
template <class T, int N>
void CenterNonOrigin_Test(T precision) {
  const auto start = MakeVec<T, N>(static_cast<T>(2), static_cast<T>(3));
  const auto end = MakeVec<T, N>(static_cast<T>(6), static_cast<T>(7));
  const T radius = static_cast<T>(0.5);

  mathkata::Capsule<T, N> capsule(start, end, radius);
  const auto center = capsule.Center();

  const auto expected = MakeVec<T, N>(static_cast<T>(4), static_cast<T>(5));
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(center[i], expected[i], precision);
  }
}
TEST_ALL_CAPSULE_F(CenterNonOrigin)

// Test Length() returns the distance between start and end.
template <class T, int N>
void Length_Test(T precision) {
  const auto start = MakeVec<T, N>(static_cast<T>(0), static_cast<T>(0));
  const auto end = MakeVec<T, N>(static_cast<T>(3), static_cast<T>(4));
  const T radius = static_cast<T>(1);

  mathkata::Capsule<T, N> capsule(start, end, radius);

  EXPECT_NEAR(capsule.Length(), static_cast<T>(5), precision);
}
TEST_ALL_CAPSULE_F(Length)

// Test Length() for a degenerate capsule (start == end).
template <class T, int N>
void LengthDegenerate_Test(T precision) {
  const auto point = MakeVec<T, N>(static_cast<T>(3), static_cast<T>(4));
  const T radius = static_cast<T>(2);

  mathkata::Capsule<T, N> capsule(point, point, radius);

  EXPECT_NEAR(capsule.Length(), static_cast<T>(0), precision);
}
TEST_ALL_CAPSULE_F(LengthDegenerate)

// Test Contains() for a point on the line segment.
template <class T, int N>
void ContainsOnSegment_Test(T precision) {
  (void)precision;
  const auto start = MakeVec<T, N>(static_cast<T>(0), static_cast<T>(0));
  const auto end = MakeVec<T, N>(static_cast<T>(10), static_cast<T>(0));
  const T radius = static_cast<T>(1);

  mathkata::Capsule<T, N> capsule(start, end, radius);

  // Point exactly on the segment midpoint.
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(0))));

  // Point at the start.
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(0), static_cast<T>(0))));

  // Point at the end.
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(10), static_cast<T>(0))));
}
TEST_ALL_CAPSULE_F(ContainsOnSegment)

// Test Contains() for points at the edge of the radius.
template <class T, int N>
void ContainsAtRadius_Test(T precision) {
  (void)precision;
  const auto start = MakeVec<T, N>(static_cast<T>(0), static_cast<T>(0));
  const auto end = MakeVec<T, N>(static_cast<T>(10), static_cast<T>(0));
  const T radius = static_cast<T>(2);

  mathkata::Capsule<T, N> capsule(start, end, radius);

  // Point exactly at the radius boundary perpendicular to the segment.
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(2))));
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(-2))));

  // Point just inside the radius.
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(1.9))));
}
TEST_ALL_CAPSULE_F(ContainsAtRadius)

// Test Contains() for points at the spherical caps.
template <class T, int N>
void ContainsAtCaps_Test(T precision) {
  (void)precision;
  const auto start = MakeVec<T, N>(static_cast<T>(0), static_cast<T>(0));
  const auto end = MakeVec<T, N>(static_cast<T>(10), static_cast<T>(0));
  const T radius = static_cast<T>(2);

  mathkata::Capsule<T, N> capsule(start, end, radius);

  // Point at the start cap (extending beyond start along the segment axis).
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(-2), static_cast<T>(0))));
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(-1.9), static_cast<T>(0))));

  // Point at the end cap.
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(12), static_cast<T>(0))));
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(11.9), static_cast<T>(0))));

  // Point just outside the start cap.
  EXPECT_FALSE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(-2.1), static_cast<T>(0))));

  // Point just outside the end cap.
  EXPECT_FALSE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(12.1), static_cast<T>(0))));
}
TEST_ALL_CAPSULE_F(ContainsAtCaps)

// Test Contains() for points clearly outside.
template <class T, int N>
void ContainsOutside_Test(T precision) {
  (void)precision;
  const auto start = MakeVec<T, N>(static_cast<T>(0), static_cast<T>(0));
  const auto end = MakeVec<T, N>(static_cast<T>(10), static_cast<T>(0));
  const T radius = static_cast<T>(1);

  mathkata::Capsule<T, N> capsule(start, end, radius);

  // Far away from the capsule.
  EXPECT_FALSE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(10))));
  EXPECT_FALSE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(-10))));
  EXPECT_FALSE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(-5), static_cast<T>(0))));
  EXPECT_FALSE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(15), static_cast<T>(0))));

  // Just outside the radius boundary.
  EXPECT_FALSE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(1.1))));
}
TEST_ALL_CAPSULE_F(ContainsOutside)

// Test Contains() for a degenerate capsule (start == end, effectively a
// sphere).
template <class T, int N>
void ContainsDegenerate_Test(T precision) {
  (void)precision;
  const auto center = MakeVec<T, N>(static_cast<T>(5), static_cast<T>(5));
  const T radius = static_cast<T>(3);

  mathkata::Capsule<T, N> capsule(center, center, radius);

  // Point at the center.
  EXPECT_TRUE(capsule.Contains(center));

  // Point within the sphere.
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(7))));

  // Point at the sphere boundary.
  EXPECT_TRUE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(8))));

  // Point outside the sphere.
  EXPECT_FALSE(
      capsule.Contains(MakeVec<T, N>(static_cast<T>(5), static_cast<T>(8.1))));
}
TEST_ALL_CAPSULE_F(ContainsDegenerate)

// Test Contains() with a 3D point off the XY plane.
TEST_F(CapsuleTests, Contains3D_float) {
  const mathkata::Vector<float, 3> start(0, 0, 0);
  const mathkata::Vector<float, 3> end(10, 0, 0);
  const float radius = 2.0f;

  mathkata::Capsule<float, 3> capsule(start, end, radius);

  // Point above the segment in Z.
  EXPECT_TRUE(capsule.Contains(mathkata::Vector<float, 3>(5, 0, 1.5f)));
  EXPECT_TRUE(capsule.Contains(mathkata::Vector<float, 3>(5, 0, 2.0f)));
  EXPECT_FALSE(capsule.Contains(mathkata::Vector<float, 3>(5, 0, 2.1f)));

  // Point diagonally offset in Y and Z.
  const float diag = radius / std::sqrt(2.0f);
  EXPECT_TRUE(capsule.Contains(mathkata::Vector<float, 3>(5, diag, diag)));
  // Just outside diagonally.
  const float diag_out = (radius * 1.1f) / std::sqrt(2.0f);
  EXPECT_FALSE(
      capsule.Contains(mathkata::Vector<float, 3>(5, diag_out, diag_out)));
}

TEST_F(CapsuleTests, Contains3D_double) {
  const mathkata::Vector<double, 3> start(0, 0, 0);
  const mathkata::Vector<double, 3> end(10, 0, 0);
  const double radius = 2.0;

  mathkata::Capsule<double, 3> capsule(start, end, radius);

  // Point above the segment in Z.
  EXPECT_TRUE(capsule.Contains(mathkata::Vector<double, 3>(5, 0, 1.5)));
  EXPECT_TRUE(capsule.Contains(mathkata::Vector<double, 3>(5, 0, 2.0)));
  EXPECT_FALSE(capsule.Contains(mathkata::Vector<double, 3>(5, 0, 2.1)));

  // Point diagonally offset in Y and Z.
  const double diag = radius / std::sqrt(2.0);
  EXPECT_TRUE(capsule.Contains(mathkata::Vector<double, 3>(5, diag, diag)));
  // Just outside diagonally.
  const double diag_out = (radius * 1.1) / std::sqrt(2.0);
  EXPECT_FALSE(
      capsule.Contains(mathkata::Vector<double, 3>(5, diag_out, diag_out)));
}

// Test equality operator.
template <class T, int N>
void Equality_Test(T precision) {
  (void)precision;
  const auto start = MakeVec<T, N>(static_cast<T>(1), static_cast<T>(2));
  const auto end = MakeVec<T, N>(static_cast<T>(3), static_cast<T>(4));
  const T radius = static_cast<T>(0.5);

  mathkata::Capsule<T, N> c1(start, end, radius);
  mathkata::Capsule<T, N> c2(start, end, radius);

  EXPECT_TRUE(c1 == c2);
  EXPECT_FALSE(c1 != c2);
}
TEST_ALL_CAPSULE_F(Equality)

// Test inequality with different start.
template <class T, int N>
void InequalityStart_Test(T precision) {
  (void)precision;
  const auto start1 = MakeVec<T, N>(static_cast<T>(1), static_cast<T>(2));
  const auto start2 = MakeVec<T, N>(static_cast<T>(0), static_cast<T>(2));
  const auto end = MakeVec<T, N>(static_cast<T>(3), static_cast<T>(4));
  const T radius = static_cast<T>(0.5);

  mathkata::Capsule<T, N> c1(start1, end, radius);
  mathkata::Capsule<T, N> c2(start2, end, radius);

  EXPECT_FALSE(c1 == c2);
  EXPECT_TRUE(c1 != c2);
}
TEST_ALL_CAPSULE_F(InequalityStart)

// Test inequality with different end.
template <class T, int N>
void InequalityEnd_Test(T precision) {
  (void)precision;
  const auto start = MakeVec<T, N>(static_cast<T>(1), static_cast<T>(2));
  const auto end1 = MakeVec<T, N>(static_cast<T>(3), static_cast<T>(4));
  const auto end2 = MakeVec<T, N>(static_cast<T>(3), static_cast<T>(5));
  const T radius = static_cast<T>(0.5);

  mathkata::Capsule<T, N> c1(start, end1, radius);
  mathkata::Capsule<T, N> c2(start, end2, radius);

  EXPECT_FALSE(c1 == c2);
  EXPECT_TRUE(c1 != c2);
}
TEST_ALL_CAPSULE_F(InequalityEnd)

// Test inequality with different radius.
template <class T, int N>
void InequalityRadius_Test(T precision) {
  (void)precision;
  const auto start = MakeVec<T, N>(static_cast<T>(1), static_cast<T>(2));
  const auto end = MakeVec<T, N>(static_cast<T>(3), static_cast<T>(4));

  mathkata::Capsule<T, N> c1(start, end, static_cast<T>(0.5));
  mathkata::Capsule<T, N> c2(start, end, static_cast<T>(1.0));

  EXPECT_FALSE(c1 == c2);
  EXPECT_TRUE(c1 != c2);
}
TEST_ALL_CAPSULE_F(InequalityRadius)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

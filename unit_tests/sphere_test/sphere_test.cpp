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
#include "mathkata/sphere.h"

#include <cmath>
#include <numbers>

#include "gtest/gtest.h"
#include "mathkata/utilities.h"
#include "precision.h"

class SphereTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// This will automatically generate tests for float and double, 2D and 3D.
#define TEST_ALL_F(MY_TEST)                      \
  TEST_F(SphereTests, MY_TEST##_float_2) {       \
    MY_TEST##_Test<float, 2>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(SphereTests, MY_TEST##_double_2) {      \
    MY_TEST##_Test<double, 2>(DOUBLE_PRECISION); \
  }                                              \
  TEST_F(SphereTests, MY_TEST##_float_3) {       \
    MY_TEST##_Test<float, 3>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(SphereTests, MY_TEST##_double_3) {      \
    MY_TEST##_Test<double, 3>(DOUBLE_PRECISION); \
  }

// Generate tests only for 2D (circle-specific).
#define TEST_2D_F(MY_TEST)                    \
  TEST_F(SphereTests, MY_TEST##_float_2) {    \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
  }                                           \
  TEST_F(SphereTests, MY_TEST##_double_2) {   \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

// Generate tests only for 3D (sphere-specific).
#define TEST_3D_F(MY_TEST)                    \
  TEST_F(SphereTests, MY_TEST##_float_3) {    \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
  }                                           \
  TEST_F(SphereTests, MY_TEST##_double_3) {   \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

// Helper to create a zero vector.
template <class T, int N>
mathkata::Vector<T, N> MakeZeroVector();

template <>
mathkata::Vector<float, 2> MakeZeroVector<float, 2>() {
  return mathkata::Vector<float, 2>(0.0f, 0.0f);
}

template <>
mathkata::Vector<double, 2> MakeZeroVector<double, 2>() {
  return mathkata::Vector<double, 2>(0.0, 0.0);
}

template <>
mathkata::Vector<float, 3> MakeZeroVector<float, 3>() {
  return mathkata::Vector<float, 3>(0.0f, 0.0f, 0.0f);
}

template <>
mathkata::Vector<double, 3> MakeZeroVector<double, 3>() {
  return mathkata::Vector<double, 3>(0.0, 0.0, 0.0);
}

// Helper to create a vector with a value along the first axis.
template <class T, int N>
mathkata::Vector<T, N> MakeAxisVector(T value);

template <>
mathkata::Vector<float, 2> MakeAxisVector<float, 2>(float value) {
  return mathkata::Vector<float, 2>(value, 0.0f);
}

template <>
mathkata::Vector<double, 2> MakeAxisVector<double, 2>(double value) {
  return mathkata::Vector<double, 2>(value, 0.0);
}

template <>
mathkata::Vector<float, 3> MakeAxisVector<float, 3>(float value) {
  return mathkata::Vector<float, 3>(value, 0.0f, 0.0f);
}

template <>
mathkata::Vector<double, 3> MakeAxisVector<double, 3>(double value) {
  return mathkata::Vector<double, 3>(value, 0.0, 0.0);
}

// Helper to create a unit vector along the second axis.
template <class T, int N>
mathkata::Vector<T, N> MakeSecondAxisVector(T value);

template <>
mathkata::Vector<float, 2> MakeSecondAxisVector<float, 2>(float value) {
  return mathkata::Vector<float, 2>(0.0f, value);
}

template <>
mathkata::Vector<double, 2> MakeSecondAxisVector<double, 2>(double value) {
  return mathkata::Vector<double, 2>(0.0, value);
}

template <>
mathkata::Vector<float, 3> MakeSecondAxisVector<float, 3>(float value) {
  return mathkata::Vector<float, 3>(0.0f, value, 0.0f);
}

template <>
mathkata::Vector<double, 3> MakeSecondAxisVector<double, 3>(double value) {
  return mathkata::Vector<double, 3>(0.0, value, 0.0);
}

// Test: Construction from center and radius.
template <class T, int N>
void Construction_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  T radius = static_cast<T>(5);
  mathkata::Sphere<T, N> sphere(center, radius);

  for (int i = 0; i < N; ++i) {
    EXPECT_EQ(sphere.center[i], static_cast<T>(0));
  }
  EXPECT_EQ(sphere.radius, static_cast<T>(5));
}
TEST_ALL_F(Construction)

// Test: Construction with non-origin center.
template <class T, int N>
void ConstructionNonOrigin_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeAxisVector<T, N>(static_cast<T>(3));
  T radius = static_cast<T>(7);
  mathkata::Sphere<T, N> sphere(center, radius);

  EXPECT_EQ(sphere.center[0], static_cast<T>(3));
  EXPECT_EQ(sphere.radius, static_cast<T>(7));
}
TEST_ALL_F(ConstructionNonOrigin)

// Test: Equality operators.
template <class T, int N>
void Equality_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeAxisVector<T, N>(static_cast<T>(1));
  T radius = static_cast<T>(5);
  mathkata::Sphere<T, N> s1(center, radius);
  mathkata::Sphere<T, N> s2(center, radius);

  EXPECT_TRUE(s1 == s2);
  EXPECT_FALSE(s1 != s2);
}
TEST_ALL_F(Equality)

// Test: Inequality with different centers.
template <class T, int N>
void InequalityCenter_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> c1 = MakeAxisVector<T, N>(static_cast<T>(1));
  mathkata::Vector<T, N> c2 = MakeAxisVector<T, N>(static_cast<T>(2));
  T radius = static_cast<T>(5);
  mathkata::Sphere<T, N> s1(c1, radius);
  mathkata::Sphere<T, N> s2(c2, radius);

  EXPECT_FALSE(s1 == s2);
  EXPECT_TRUE(s1 != s2);
}
TEST_ALL_F(InequalityCenter)

// Test: Inequality with different radii.
template <class T, int N>
void InequalityRadius_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> s1(center, static_cast<T>(5));
  mathkata::Sphere<T, N> s2(center, static_cast<T>(10));

  EXPECT_FALSE(s1 == s2);
  EXPECT_TRUE(s1 != s2);
}
TEST_ALL_F(InequalityRadius)

// Test: diameter calculation.
template <class T, int N>
void diameter_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(5));
  EXPECT_EQ(sphere.diameter(), static_cast<T>(10));
}
TEST_ALL_F(diameter)

// Test: contains point - point at center.
template <class T, int N>
void ContainsPointCenter_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(5));
  EXPECT_TRUE(sphere.contains(center));
}
TEST_ALL_F(ContainsPointCenter)

// Test: contains point - point inside sphere.
template <class T, int N>
void ContainsPointInside_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(5));
  mathkata::Vector<T, N> point = MakeAxisVector<T, N>(static_cast<T>(3));
  EXPECT_TRUE(sphere.contains(point));
}
TEST_ALL_F(ContainsPointInside)

// Test: contains point - point on boundary.
template <class T, int N>
void ContainsPointBoundary_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(5));
  mathkata::Vector<T, N> point = MakeAxisVector<T, N>(static_cast<T>(5));
  EXPECT_TRUE(sphere.contains(point));
}
TEST_ALL_F(ContainsPointBoundary)

// Test: contains point - point outside sphere.
template <class T, int N>
void ContainsPointOutside_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(5));
  mathkata::Vector<T, N> point = MakeAxisVector<T, N>(static_cast<T>(6));
  EXPECT_FALSE(sphere.contains(point));
}
TEST_ALL_F(ContainsPointOutside)

// Test: contains sphere - smaller sphere fully inside.
template <class T, int N>
void ContainsSphereInside_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> outer(center, static_cast<T>(10));
  mathkata::Sphere<T, N> inner(center, static_cast<T>(5));
  EXPECT_TRUE(outer.contains(inner));
}
TEST_ALL_F(ContainsSphereInside)

// Test: contains sphere - offset inner sphere still fully inside.
template <class T, int N>
void ContainsSphereOffset_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> outer(center, static_cast<T>(10));
  mathkata::Vector<T, N> inner_center = MakeAxisVector<T, N>(static_cast<T>(3));
  mathkata::Sphere<T, N> inner(inner_center, static_cast<T>(5));
  // distance between centers is 3, inner radius is 5, so 3 + 5 = 8 <= 10.
  EXPECT_TRUE(outer.contains(inner));
}
TEST_ALL_F(ContainsSphereOffset)

// Test: contains sphere - sphere partially outside.
template <class T, int N>
void ContainsSpherePartiallyOutside_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> outer(center, static_cast<T>(10));
  mathkata::Vector<T, N> inner_center = MakeAxisVector<T, N>(static_cast<T>(7));
  mathkata::Sphere<T, N> inner(inner_center, static_cast<T>(5));
  // distance between centers is 7, inner radius is 5, so 7 + 5 = 12 > 10.
  EXPECT_FALSE(outer.contains(inner));
}
TEST_ALL_F(ContainsSpherePartiallyOutside)

// Test: contains sphere - same sphere.
template <class T, int N>
void ContainsSphereSame_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(5));
  // A sphere should contain itself (dist=0, 0 + 5 <= 5).
  EXPECT_TRUE(sphere.contains(sphere));
}
TEST_ALL_F(ContainsSphereSame)

// Test: intersects - overlapping spheres.
template <class T, int N>
void IntersectsOverlapping_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> c1 = MakeZeroVector<T, N>();
  mathkata::Vector<T, N> c2 = MakeAxisVector<T, N>(static_cast<T>(5));
  mathkata::Sphere<T, N> s1(c1, static_cast<T>(4));
  mathkata::Sphere<T, N> s2(c2, static_cast<T>(4));
  // distance between centers is 5, sum of radii is 8. 5 <= 8 so they
  // intersect.
  EXPECT_TRUE(s1.intersects(s2));
  EXPECT_TRUE(s2.intersects(s1));
}
TEST_ALL_F(IntersectsOverlapping)

// Test: intersects - touching spheres (boundary contact).
template <class T, int N>
void IntersectsTouching_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> c1 = MakeZeroVector<T, N>();
  mathkata::Vector<T, N> c2 = MakeAxisVector<T, N>(static_cast<T>(10));
  mathkata::Sphere<T, N> s1(c1, static_cast<T>(5));
  mathkata::Sphere<T, N> s2(c2, static_cast<T>(5));
  // distance between centers is 10, sum of radii is 10. 10 <= 10 so they
  // intersect at the boundary.
  EXPECT_TRUE(s1.intersects(s2));
}
TEST_ALL_F(IntersectsTouching)

// Test: intersects - non-overlapping spheres.
template <class T, int N>
void IntersectsNonOverlapping_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> c1 = MakeZeroVector<T, N>();
  mathkata::Vector<T, N> c2 = MakeAxisVector<T, N>(static_cast<T>(20));
  mathkata::Sphere<T, N> s1(c1, static_cast<T>(5));
  mathkata::Sphere<T, N> s2(c2, static_cast<T>(5));
  // distance between centers is 20, sum of radii is 10. 20 > 10 so they don't
  // intersect.
  EXPECT_FALSE(s1.intersects(s2));
  EXPECT_FALSE(s2.intersects(s1));
}
TEST_ALL_F(IntersectsNonOverlapping)

// Test: intersects - concentric spheres.
template <class T, int N>
void IntersectsConcentric_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> s1(center, static_cast<T>(5));
  mathkata::Sphere<T, N> s2(center, static_cast<T>(3));
  EXPECT_TRUE(s1.intersects(s2));
  EXPECT_TRUE(s2.intersects(s1));
}
TEST_ALL_F(IntersectsConcentric)

// Test: area (2D circle only).
template <class T>
void area_Test(T precision) {
  mathkata::Vector<T, 2> center(static_cast<T>(0), static_cast<T>(0));
  T radius = static_cast<T>(5);
  mathkata::Sphere<T, 2> circle(center, radius);
  T expected = std::numbers::pi_v<T> * static_cast<T>(25);
  EXPECT_NEAR(static_cast<double>(circle.area()), static_cast<double>(expected),
              static_cast<double>(precision));
}
TEST_2D_F(area)

// Test: area with unit circle.
template <class T>
void AreaUnit_Test(T precision) {
  mathkata::Vector<T, 2> center(static_cast<T>(0), static_cast<T>(0));
  T radius = static_cast<T>(1);
  mathkata::Sphere<T, 2> circle(center, radius);
  EXPECT_NEAR(static_cast<double>(circle.area()),
              static_cast<double>(std::numbers::pi_v<T>),
              static_cast<double>(precision));
}
TEST_2D_F(AreaUnit)

// Test: volume (3D sphere only).
template <class T>
void volume_Test(T precision) {
  mathkata::Vector<T, 3> center(static_cast<T>(0), static_cast<T>(0),
                                static_cast<T>(0));
  T radius = static_cast<T>(5);
  mathkata::Sphere<T, 3> sphere(center, radius);
  T expected = (static_cast<T>(4) / static_cast<T>(3)) * std::numbers::pi_v<T>
               * radius * radius * radius;
  EXPECT_NEAR(static_cast<double>(sphere.volume()),
              static_cast<double>(expected), static_cast<double>(precision));
}
TEST_3D_F(volume)

// Test: volume with unit sphere.
template <class T>
void VolumeUnit_Test(T precision) {
  mathkata::Vector<T, 3> center(static_cast<T>(0), static_cast<T>(0),
                                static_cast<T>(0));
  T radius = static_cast<T>(1);
  mathkata::Sphere<T, 3> sphere(center, radius);
  T expected = (static_cast<T>(4) / static_cast<T>(3)) * std::numbers::pi_v<T>;
  EXPECT_NEAR(static_cast<double>(sphere.volume()),
              static_cast<double>(expected), static_cast<double>(precision));
}
TEST_3D_F(VolumeUnit)

// Test: contains point with non-origin center.
template <class T, int N>
void ContainsPointNonOrigin_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeAxisVector<T, N>(static_cast<T>(10));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(3));
  // Point at the center.
  EXPECT_TRUE(sphere.contains(center));
  // Point within radius along second axis from center.
  mathkata::Vector<T, N> nearby = center;
  nearby[1] = static_cast<T>(2);
  EXPECT_TRUE(sphere.contains(nearby));
  // Point far away.
  mathkata::Vector<T, N> far_away = MakeZeroVector<T, N>();
  EXPECT_FALSE(sphere.contains(far_away));
}
TEST_ALL_F(ContainsPointNonOrigin)

// Test: diameter with zero radius.
template <class T, int N>
void DiameterZero_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center = MakeZeroVector<T, N>();
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(0));
  EXPECT_EQ(sphere.diameter(), static_cast<T>(0));
}
TEST_ALL_F(DiameterZero)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

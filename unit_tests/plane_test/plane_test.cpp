/*
 * Copyright 2024 Google Inc. All rights reserved.
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
#include "mathfu/plane.h"

#include <cmath>
#include <cstdio>

#include "gtest/gtest.h"
#include "mathfu/utilities.h"
#include "precision.h"

class PlaneTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Helper macro to generate float and double test variants.
#define TEST_PLANE_F(MY_TEST)                 \
  TEST_F(PlaneTests, MY_TEST##_float) {       \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
  }                                           \
  TEST_F(PlaneTests, MY_TEST##_double) {      \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

// Convenience typedef to avoid commas in macro arguments.
template <class T>
using Vec3 = mathfu::Vector<T, 3>;

template <class T>
using Vec4 = mathfu::Vector<T, 4>;

// Test construction from normal and distance.
template <class T>
void ConstructNormalDistance_Test(T /*precision*/) {
  const Vec3<T> normal(0, 1, 0);
  const T distance = static_cast<T>(5);
  const mathfu::Plane<T> plane(normal, distance);
  EXPECT_EQ(plane.normal, normal);
  EXPECT_EQ(plane.distance, distance);
}
TEST_PLANE_F(ConstructNormalDistance)

// Test construction from Vector4.
template <class T>
void ConstructVector4_Test(T /*precision*/) {
  const Vec4<T> v(0, 1, 0, 5);
  const mathfu::Plane<T> plane(v);
  const Vec3<T> expected_normal(0, 1, 0);
  EXPECT_EQ(plane.normal, expected_normal);
  EXPECT_EQ(plane.distance, static_cast<T>(5));
}
TEST_PLANE_F(ConstructVector4)

// Test FromPointNormal factory method.
template <class T>
void FromPointNormal_Test(T precision) {
  // A plane at y=3 with normal pointing up.
  const Vec3<T> point(0, 3, 0);
  const Vec3<T> normal(0, 1, 0);
  const mathfu::Plane<T> plane =
      mathfu::Plane<T>::FromPointNormal(point, normal);
  EXPECT_EQ(plane.normal, normal);
  // distance should be -dot(normal, point) = -3
  EXPECT_NEAR(plane.distance, static_cast<T>(-3), precision);
  // The point itself should lie on the plane.
  EXPECT_NEAR(plane.SignedDistance(point), static_cast<T>(0), precision);
}
TEST_PLANE_F(FromPointNormal)

// Test FromPoints factory method.
template <class T>
void FromPoints_Test(T precision) {
  // Three points on the XZ plane (y=0).
  const Vec3<T> a(0, 0, 0);
  const Vec3<T> b(1, 0, 0);
  const Vec3<T> c(0, 0, 1);
  const mathfu::Plane<T> plane = mathfu::Plane<T>::FromPoints(a, b, c);
  // The normal should point in the +Y or -Y direction.
  // Cross product of (b-a)=(1,0,0) and (c-a)=(0,0,1) is (0,-1,0).
  // Normalized: (0,-1,0).
  EXPECT_NEAR(std::abs(plane.normal[1]), static_cast<T>(1), precision);
  EXPECT_NEAR(plane.normal[0], static_cast<T>(0), precision);
  EXPECT_NEAR(plane.normal[2], static_cast<T>(0), precision);
  // All three points should lie on the plane.
  EXPECT_NEAR(plane.SignedDistance(a), static_cast<T>(0), precision);
  EXPECT_NEAR(plane.SignedDistance(b), static_cast<T>(0), precision);
  EXPECT_NEAR(plane.SignedDistance(c), static_cast<T>(0), precision);
}
TEST_PLANE_F(FromPoints)

// Test SignedDistance with points on positive side, negative side, and on
// plane.
template <class T>
void SignedDistance_Test(T precision) {
  // Plane: y = 2 (normal=(0,1,0), distance=-2)
  const Vec3<T> plane_point(0, 2, 0);
  const Vec3<T> plane_normal(0, 1, 0);
  const mathfu::Plane<T> plane =
      mathfu::Plane<T>::FromPointNormal(plane_point, plane_normal);
  // Point on the plane.
  const Vec3<T> on_plane(5, 2, 3);
  EXPECT_NEAR(plane.SignedDistance(on_plane), static_cast<T>(0), precision);
  // Point on the positive side (above the plane).
  const Vec3<T> above(0, 5, 0);
  EXPECT_NEAR(plane.SignedDistance(above), static_cast<T>(3), precision);
  // Point on the negative side (below the plane).
  const Vec3<T> below(0, -1, 0);
  EXPECT_NEAR(plane.SignedDistance(below), static_cast<T>(-3), precision);
}
TEST_PLANE_F(SignedDistance)

// Test ProjectPoint.
template <class T>
void ProjectPoint_Test(T precision) {
  // Plane: y = 2 (normal=(0,1,0), distance=-2)
  const Vec3<T> plane_point(0, 2, 0);
  const Vec3<T> plane_normal(0, 1, 0);
  const mathfu::Plane<T> plane =
      mathfu::Plane<T>::FromPointNormal(plane_point, plane_normal);
  // Project a point above the plane.
  const Vec3<T> point_above(3, 7, 4);
  const Vec3<T> projected = plane.ProjectPoint(point_above);
  EXPECT_NEAR(projected[0], static_cast<T>(3), precision);
  EXPECT_NEAR(projected[1], static_cast<T>(2), precision);
  EXPECT_NEAR(projected[2], static_cast<T>(4), precision);
  // A point already on the plane should project to itself.
  const Vec3<T> on_plane(3, 2, 4);
  const Vec3<T> projected_on = plane.ProjectPoint(on_plane);
  EXPECT_NEAR(projected_on[0], on_plane[0], precision);
  EXPECT_NEAR(projected_on[1], on_plane[1], precision);
  EXPECT_NEAR(projected_on[2], on_plane[2], precision);
}
TEST_PLANE_F(ProjectPoint)

// Test Flipped.
template <class T>
void Flipped_Test(T /*precision*/) {
  const Vec3<T> normal(0, 1, 0);
  const T distance = static_cast<T>(-3);
  const mathfu::Plane<T> plane(normal, distance);
  const mathfu::Plane<T> flipped = plane.Flipped();
  const Vec3<T> neg_normal = -normal;
  EXPECT_EQ(flipped.normal, neg_normal);
  EXPECT_EQ(flipped.distance, -distance);
}
TEST_PLANE_F(Flipped)

// Test equality and inequality operators.
template <class T>
void Equality_Test(T /*precision*/) {
  const Vec3<T> n1(0, 1, 0);
  const Vec3<T> n2(1, 0, 0);
  const mathfu::Plane<T> p1(n1, static_cast<T>(5));
  const mathfu::Plane<T> p2(n1, static_cast<T>(5));
  const mathfu::Plane<T> p3(n2, static_cast<T>(5));
  const mathfu::Plane<T> p4(n1, static_cast<T>(3));
  EXPECT_TRUE(p1 == p2);
  EXPECT_FALSE(p1 != p2);
  EXPECT_TRUE(p1 != p3);
  EXPECT_TRUE(p1 != p4);
  EXPECT_FALSE(p1 == p3);
  EXPECT_FALSE(p1 == p4);
}
TEST_PLANE_F(Equality)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHFU_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

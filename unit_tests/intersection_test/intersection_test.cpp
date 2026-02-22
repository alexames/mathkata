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
#include <cmath>
#include <cstdio>

#include "gtest/gtest.h"
#include "mathfu/intersections.h"
#include "mathfu/utilities.h"
#include "precision.h"

class IntersectionTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Helper macros for generating tests with float and double.
// For N-dimensional tests (2D and 3D):
#define TEST_ALL_DIMS_F(MY_TEST)                  \
  TEST_F(IntersectionTests, MY_TEST##_float_2) {  \
    MY_TEST##_Test<float, 2>(FLOAT_PRECISION);    \
  }                                               \
  TEST_F(IntersectionTests, MY_TEST##_double_2) { \
    MY_TEST##_Test<double, 2>(DOUBLE_PRECISION);  \
  }                                               \
  TEST_F(IntersectionTests, MY_TEST##_float_3) {  \
    MY_TEST##_Test<float, 3>(FLOAT_PRECISION);    \
  }                                               \
  TEST_F(IntersectionTests, MY_TEST##_double_3) { \
    MY_TEST##_Test<double, 3>(DOUBLE_PRECISION);  \
  }

// For 3D-only tests (Plane is 3D only):
#define TEST_3D_F(MY_TEST)                      \
  TEST_F(IntersectionTests, MY_TEST##_float) {  \
    MY_TEST##_Test<float>(FLOAT_PRECISION);     \
  }                                             \
  TEST_F(IntersectionTests, MY_TEST##_double) { \
    MY_TEST##_Test<double>(DOUBLE_PRECISION);   \
  }

// ============================================================================
// RayIntersectsSphere tests
// ============================================================================

template <class T, int N>
void RayIntersectsSphere_Hit_Test(T precision) {
  // Ray pointing at a sphere centered at the origin.
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(1));

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathfu::RayIntersectsSphere(ray, sphere, &t));
  EXPECT_NEAR(t, static_cast<T>(4), precision);
}
TEST_ALL_DIMS_F(RayIntersectsSphere_Hit)

template <class T, int N>
void RayIntersectsSphere_Miss_Test(T precision) {
  (void)precision;
  // Ray pointing away from the sphere.
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(-1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(1));

  EXPECT_FALSE(mathfu::RayIntersectsSphere(ray, sphere));
}
TEST_ALL_DIMS_F(RayIntersectsSphere_Miss)

template <class T, int N>
void RayIntersectsSphere_FromInside_Test(T precision) {
  // Ray origin inside the sphere.
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(2));

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathfu::RayIntersectsSphere(ray, sphere, &t));
  // The first intersection should be at t=2 (exit point).
  EXPECT_NEAR(t, static_cast<T>(2), precision);
}
TEST_ALL_DIMS_F(RayIntersectsSphere_FromInside)

template <class T, int N>
void RayIntersectsSphere_Tangent_Test(T precision) {
  // Ray tangent to the sphere (just barely touching).
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  if (N >= 2) {
    origin[1] = static_cast<T>(1);  // Offset by exactly the radius.
  }
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(1));

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathfu::RayIntersectsSphere(ray, sphere, &t));
  EXPECT_NEAR(t, static_cast<T>(5), precision);
}
TEST_ALL_DIMS_F(RayIntersectsSphere_Tangent)

template <class T, int N>
void RayIntersectsSphere_MissOffset_Test(T precision) {
  (void)precision;
  // Ray offset past the sphere so it misses.
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  if (N >= 2) {
    origin[1] = static_cast<T>(2);  // Offset past the radius.
  }
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(1));

  EXPECT_FALSE(mathfu::RayIntersectsSphere(ray, sphere));
}
TEST_ALL_DIMS_F(RayIntersectsSphere_MissOffset)

// ============================================================================
// RayIntersectsAABB tests
// ============================================================================

template <class T, int N>
void RayIntersectsAABB_Hit_Test(T precision) {
  // Ray pointing at an AABB.
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> box_min(static_cast<T>(-1));
  mathfu::Vector<T, N> box_max(static_cast<T>(1));
  mathfu::AABB<T, N> aabb(box_min, box_max);

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathfu::RayIntersectsAABB(ray, aabb, &t));
  EXPECT_NEAR(t, static_cast<T>(4), precision);
}
TEST_ALL_DIMS_F(RayIntersectsAABB_Hit)

template <class T, int N>
void RayIntersectsAABB_Miss_Test(T precision) {
  (void)precision;
  // Ray pointing away from the AABB.
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(-1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> box_min(static_cast<T>(-1));
  mathfu::Vector<T, N> box_max(static_cast<T>(1));
  mathfu::AABB<T, N> aabb(box_min, box_max);

  EXPECT_FALSE(mathfu::RayIntersectsAABB(ray, aabb));
}
TEST_ALL_DIMS_F(RayIntersectsAABB_Miss)

template <class T, int N>
void RayIntersectsAABB_FromInside_Test(T precision) {
  // Ray origin inside the AABB.
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> box_min(static_cast<T>(-2));
  mathfu::Vector<T, N> box_max(static_cast<T>(2));
  mathfu::AABB<T, N> aabb(box_min, box_max);

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathfu::RayIntersectsAABB(ray, aabb, &t));
  EXPECT_NEAR(t, static_cast<T>(0), precision);
}
TEST_ALL_DIMS_F(RayIntersectsAABB_FromInside)

template <class T, int N>
void RayIntersectsAABB_AlongEdge_Test(T precision) {
  // Ray along the edge of the AABB (origin at a face).
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  if (N >= 2) {
    origin[1] = static_cast<T>(1);  // On the boundary.
  }
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> box_min(static_cast<T>(-1));
  mathfu::Vector<T, N> box_max(static_cast<T>(1));
  mathfu::AABB<T, N> aabb(box_min, box_max);

  T t = static_cast<T>(0);
  // Ray should still hit the AABB on the boundary.
  EXPECT_TRUE(mathfu::RayIntersectsAABB(ray, aabb, &t));
  EXPECT_NEAR(t, static_cast<T>(4), precision);
}
TEST_ALL_DIMS_F(RayIntersectsAABB_AlongEdge)

template <class T, int N>
void RayIntersectsAABB_Parallel_Test(T precision) {
  (void)precision;
  // Ray parallel to a slab but outside the AABB.
  mathfu::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  if (N >= 2) {
    origin[1] = static_cast<T>(3);  // Outside the AABB.
  }
  mathfu::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathfu::Ray<T, N> ray(origin, direction);

  mathfu::Vector<T, N> box_min(static_cast<T>(-1));
  mathfu::Vector<T, N> box_max(static_cast<T>(1));
  mathfu::AABB<T, N> aabb(box_min, box_max);

  if (N >= 2) {
    EXPECT_FALSE(mathfu::RayIntersectsAABB(ray, aabb));
  }
}
TEST_ALL_DIMS_F(RayIntersectsAABB_Parallel)

// ============================================================================
// RayIntersectsPlane tests (3D only)
// ============================================================================

template <class T>
void RayIntersectsPlane_Hit_Test(T precision) {
  // Ray pointing toward a plane at z=5.
  mathfu::Ray<T, 3> ray(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(0)),
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(1)));
  mathfu::Plane<T> plane(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(1)),
      static_cast<T>(-5));

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathfu::RayIntersectsPlane(ray, plane, &t));
  EXPECT_NEAR(t, static_cast<T>(5), precision);
}
TEST_3D_F(RayIntersectsPlane_Hit)

template <class T>
void RayIntersectsPlane_Parallel_Test(T precision) {
  (void)precision;
  // Ray parallel to the plane.
  mathfu::Ray<T, 3> ray(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(0)),
      mathfu::Vector<T, 3>(static_cast<T>(1), static_cast<T>(0),
                           static_cast<T>(0)));
  mathfu::Plane<T> plane(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(1)),
      static_cast<T>(-5));

  EXPECT_FALSE(mathfu::RayIntersectsPlane(ray, plane));
}
TEST_3D_F(RayIntersectsPlane_Parallel)

template <class T>
void RayIntersectsPlane_FromBehind_Test(T precision) {
  (void)precision;
  // Ray pointing away from the plane.
  mathfu::Ray<T, 3> ray(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(0)),
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(-1)));
  mathfu::Plane<T> plane(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(1)),
      static_cast<T>(-5));

  EXPECT_FALSE(mathfu::RayIntersectsPlane(ray, plane));
}
TEST_3D_F(RayIntersectsPlane_FromBehind)

// ============================================================================
// AABBIntersectsAABB tests
// ============================================================================

template <class T, int N>
void AABBIntersectsAABB_Overlap_Test(T precision) {
  (void)precision;
  mathfu::AABB<T, N> a(mathfu::Vector<T, N>(static_cast<T>(0)),
                       mathfu::Vector<T, N>(static_cast<T>(3)));
  mathfu::AABB<T, N> b(mathfu::Vector<T, N>(static_cast<T>(2)),
                       mathfu::Vector<T, N>(static_cast<T>(5)));
  EXPECT_TRUE(mathfu::AABBIntersectsAABB(a, b));
}
TEST_ALL_DIMS_F(AABBIntersectsAABB_Overlap)

template <class T, int N>
void AABBIntersectsAABB_Touching_Test(T precision) {
  (void)precision;
  mathfu::AABB<T, N> a(mathfu::Vector<T, N>(static_cast<T>(0)),
                       mathfu::Vector<T, N>(static_cast<T>(2)));
  mathfu::AABB<T, N> b(mathfu::Vector<T, N>(static_cast<T>(2)),
                       mathfu::Vector<T, N>(static_cast<T>(4)));
  EXPECT_TRUE(mathfu::AABBIntersectsAABB(a, b));
}
TEST_ALL_DIMS_F(AABBIntersectsAABB_Touching)

template <class T, int N>
void AABBIntersectsAABB_Separate_Test(T precision) {
  (void)precision;
  mathfu::AABB<T, N> a(mathfu::Vector<T, N>(static_cast<T>(0)),
                       mathfu::Vector<T, N>(static_cast<T>(1)));
  mathfu::AABB<T, N> b(mathfu::Vector<T, N>(static_cast<T>(3)),
                       mathfu::Vector<T, N>(static_cast<T>(5)));
  EXPECT_FALSE(mathfu::AABBIntersectsAABB(a, b));
}
TEST_ALL_DIMS_F(AABBIntersectsAABB_Separate)

// ============================================================================
// SphereIntersectsSphere tests
// ============================================================================

template <class T, int N>
void SphereIntersectsSphere_Overlap_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> c1(static_cast<T>(0));
  mathfu::Vector<T, N> c2(static_cast<T>(0));
  c2[0] = static_cast<T>(1);
  mathfu::Sphere<T, N> s1(c1, static_cast<T>(2));
  mathfu::Sphere<T, N> s2(c2, static_cast<T>(2));
  EXPECT_TRUE(mathfu::SphereIntersectsSphere(s1, s2));
}
TEST_ALL_DIMS_F(SphereIntersectsSphere_Overlap)

template <class T, int N>
void SphereIntersectsSphere_Touching_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> c1(static_cast<T>(0));
  mathfu::Vector<T, N> c2(static_cast<T>(0));
  c2[0] = static_cast<T>(4);
  mathfu::Sphere<T, N> s1(c1, static_cast<T>(2));
  mathfu::Sphere<T, N> s2(c2, static_cast<T>(2));
  EXPECT_TRUE(mathfu::SphereIntersectsSphere(s1, s2));
}
TEST_ALL_DIMS_F(SphereIntersectsSphere_Touching)

template <class T, int N>
void SphereIntersectsSphere_Separate_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> c1(static_cast<T>(0));
  mathfu::Vector<T, N> c2(static_cast<T>(0));
  c2[0] = static_cast<T>(10);
  mathfu::Sphere<T, N> s1(c1, static_cast<T>(2));
  mathfu::Sphere<T, N> s2(c2, static_cast<T>(2));
  EXPECT_FALSE(mathfu::SphereIntersectsSphere(s1, s2));
}
TEST_ALL_DIMS_F(SphereIntersectsSphere_Separate)

// ============================================================================
// SphereIntersectsAABB tests
// ============================================================================

template <class T, int N>
void SphereIntersectsAABB_Overlap_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> center(static_cast<T>(0));
  center[0] = static_cast<T>(2);
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(2));

  mathfu::AABB<T, N> aabb(mathfu::Vector<T, N>(static_cast<T>(-1)),
                          mathfu::Vector<T, N>(static_cast<T>(1)));
  EXPECT_TRUE(mathfu::SphereIntersectsAABB(sphere, aabb));
}
TEST_ALL_DIMS_F(SphereIntersectsAABB_Overlap)

template <class T, int N>
void SphereIntersectsAABB_Separate_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> center(static_cast<T>(0));
  center[0] = static_cast<T>(10);
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(1));

  mathfu::AABB<T, N> aabb(mathfu::Vector<T, N>(static_cast<T>(-1)),
                          mathfu::Vector<T, N>(static_cast<T>(1)));
  EXPECT_FALSE(mathfu::SphereIntersectsAABB(sphere, aabb));
}
TEST_ALL_DIMS_F(SphereIntersectsAABB_Separate)

template <class T, int N>
void SphereIntersectsAABB_SphereInsideAABB_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(0.5));

  mathfu::AABB<T, N> aabb(mathfu::Vector<T, N>(static_cast<T>(-5)),
                          mathfu::Vector<T, N>(static_cast<T>(5)));
  EXPECT_TRUE(mathfu::SphereIntersectsAABB(sphere, aabb));
}
TEST_ALL_DIMS_F(SphereIntersectsAABB_SphereInsideAABB)

// ============================================================================
// PointInAABB tests
// ============================================================================

template <class T, int N>
void PointInAABB_Inside_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> point(static_cast<T>(0));
  mathfu::AABB<T, N> aabb(mathfu::Vector<T, N>(static_cast<T>(-1)),
                          mathfu::Vector<T, N>(static_cast<T>(1)));
  EXPECT_TRUE(mathfu::PointInAABB(point, aabb));
}
TEST_ALL_DIMS_F(PointInAABB_Inside)

template <class T, int N>
void PointInAABB_Outside_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> point(static_cast<T>(5));
  mathfu::AABB<T, N> aabb(mathfu::Vector<T, N>(static_cast<T>(-1)),
                          mathfu::Vector<T, N>(static_cast<T>(1)));
  EXPECT_FALSE(mathfu::PointInAABB(point, aabb));
}
TEST_ALL_DIMS_F(PointInAABB_Outside)

template <class T, int N>
void PointInAABB_OnBoundary_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> point(static_cast<T>(1));
  mathfu::AABB<T, N> aabb(mathfu::Vector<T, N>(static_cast<T>(-1)),
                          mathfu::Vector<T, N>(static_cast<T>(1)));
  EXPECT_TRUE(mathfu::PointInAABB(point, aabb));
}
TEST_ALL_DIMS_F(PointInAABB_OnBoundary)

// ============================================================================
// PointInSphere tests
// ============================================================================

template <class T, int N>
void PointInSphere_Inside_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> point(static_cast<T>(0));
  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(5));
  EXPECT_TRUE(mathfu::PointInSphere(point, sphere));
}
TEST_ALL_DIMS_F(PointInSphere_Inside)

template <class T, int N>
void PointInSphere_Outside_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> point(static_cast<T>(10));
  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(1));
  EXPECT_FALSE(mathfu::PointInSphere(point, sphere));
}
TEST_ALL_DIMS_F(PointInSphere_Outside)

template <class T, int N>
void PointInSphere_OnBoundary_Test(T precision) {
  (void)precision;
  mathfu::Vector<T, N> point(static_cast<T>(0));
  point[0] = static_cast<T>(3);
  mathfu::Vector<T, N> center(static_cast<T>(0));
  mathfu::Sphere<T, N> sphere(center, static_cast<T>(3));
  EXPECT_TRUE(mathfu::PointInSphere(point, sphere));
}
TEST_ALL_DIMS_F(PointInSphere_OnBoundary)

// ============================================================================
// PointOnPlane tests (3D only)
// ============================================================================

template <class T>
void PointOnPlane_OnPlane_Test(T precision) {
  mathfu::Plane<T> plane(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(1),
                           static_cast<T>(0)),
      static_cast<T>(-3));
  // Point at y=3 should be on the plane y=3.
  mathfu::Vector<T, 3> point(static_cast<T>(1), static_cast<T>(3),
                             static_cast<T>(2));
  EXPECT_TRUE(mathfu::PointOnPlane(point, plane, precision));
}
TEST_3D_F(PointOnPlane_OnPlane)

template <class T>
void PointOnPlane_OffPlane_Test(T precision) {
  mathfu::Plane<T> plane(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(1),
                           static_cast<T>(0)),
      static_cast<T>(-3));
  // Point at y=5 should NOT be on the plane y=3.
  mathfu::Vector<T, 3> point(static_cast<T>(1), static_cast<T>(5),
                             static_cast<T>(2));
  EXPECT_FALSE(mathfu::PointOnPlane(point, plane, precision));
}
TEST_3D_F(PointOnPlane_OffPlane)

template <class T>
void PointOnPlane_NearPlane_Test(T precision) {
  mathfu::Plane<T> plane(
      mathfu::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                           static_cast<T>(1)),
      static_cast<T>(0));
  // Point very close to the z=0 plane.
  mathfu::Vector<T, 3> point(static_cast<T>(1), static_cast<T>(2),
                             precision * static_cast<T>(0.5));
  EXPECT_TRUE(
      mathfu::PointOnPlane(point, plane, precision * static_cast<T>(2)));
}
TEST_3D_F(PointOnPlane_NearPlane)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHFU_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

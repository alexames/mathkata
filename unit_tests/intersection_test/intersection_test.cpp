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
#include <cmath>
#include <cstdio>

#include "gtest/gtest.h"
#include "mathkata/intersections.h"
#include "mathkata/utilities.h"
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
// rayIntersectsSphere tests
// ============================================================================

template <class T, int N>
void RayIntersectsSphere_Hit_Test(T precision) {
  // Ray pointing at a sphere centered at the origin.
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(1));

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathkata::rayIntersectsSphere(ray, sphere, &t));
  EXPECT_NEAR(t, static_cast<T>(4), precision);
}
TEST_ALL_DIMS_F(RayIntersectsSphere_Hit)

template <class T, int N>
void RayIntersectsSphere_Miss_Test(T precision) {
  (void)precision;
  // Ray pointing away from the sphere.
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(-1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(1));

  EXPECT_FALSE(mathkata::rayIntersectsSphere(ray, sphere));
}
TEST_ALL_DIMS_F(RayIntersectsSphere_Miss)

template <class T, int N>
void RayIntersectsSphere_FromInside_Test(T precision) {
  // Ray origin inside the sphere.
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(2));

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathkata::rayIntersectsSphere(ray, sphere, &t));
  // The first intersection should be at t=2 (exit point).
  EXPECT_NEAR(t, static_cast<T>(2), precision);
}
TEST_ALL_DIMS_F(RayIntersectsSphere_FromInside)

template <class T, int N>
void RayIntersectsSphere_Tangent_Test(T precision) {
  // Ray tangent to the sphere (just barely touching).
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  if (N >= 2) {
    origin[1] = static_cast<T>(1);  // Offset by exactly the radius.
  }
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(1));

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathkata::rayIntersectsSphere(ray, sphere, &t));
  EXPECT_NEAR(t, static_cast<T>(5), precision);
}
TEST_ALL_DIMS_F(RayIntersectsSphere_Tangent)

template <class T, int N>
void RayIntersectsSphere_MissOffset_Test(T precision) {
  (void)precision;
  // Ray offset past the sphere so it misses.
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  if (N >= 2) {
    origin[1] = static_cast<T>(2);  // Offset past the radius.
  }
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(1));

  EXPECT_FALSE(mathkata::rayIntersectsSphere(ray, sphere));
}
TEST_ALL_DIMS_F(RayIntersectsSphere_MissOffset)

// ============================================================================
// rayIntersectsAABB tests
// ============================================================================

template <class T, int N>
void RayIntersectsAABB_Hit_Test(T precision) {
  // Ray pointing at an AABB.
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> box_min(static_cast<T>(-1));
  mathkata::Vector<T, N> box_max(static_cast<T>(1));
  mathkata::AABB<T, N> aabb(box_min, box_max);

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathkata::rayIntersectsAABB(ray, aabb, &t));
  EXPECT_NEAR(t, static_cast<T>(4), precision);
}
TEST_ALL_DIMS_F(RayIntersectsAABB_Hit)

template <class T, int N>
void RayIntersectsAABB_Miss_Test(T precision) {
  (void)precision;
  // Ray pointing away from the AABB.
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(-1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> box_min(static_cast<T>(-1));
  mathkata::Vector<T, N> box_max(static_cast<T>(1));
  mathkata::AABB<T, N> aabb(box_min, box_max);

  EXPECT_FALSE(mathkata::rayIntersectsAABB(ray, aabb));
}
TEST_ALL_DIMS_F(RayIntersectsAABB_Miss)

template <class T, int N>
void RayIntersectsAABB_FromInside_Test(T precision) {
  // Ray origin inside the AABB.
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> box_min(static_cast<T>(-2));
  mathkata::Vector<T, N> box_max(static_cast<T>(2));
  mathkata::AABB<T, N> aabb(box_min, box_max);

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathkata::rayIntersectsAABB(ray, aabb, &t));
  EXPECT_NEAR(t, static_cast<T>(0), precision);
}
TEST_ALL_DIMS_F(RayIntersectsAABB_FromInside)

template <class T, int N>
void RayIntersectsAABB_AlongEdge_Test(T precision) {
  // Ray along the edge of the AABB (origin at a face).
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  if (N >= 2) {
    origin[1] = static_cast<T>(1);  // On the boundary.
  }
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> box_min(static_cast<T>(-1));
  mathkata::Vector<T, N> box_max(static_cast<T>(1));
  mathkata::AABB<T, N> aabb(box_min, box_max);

  T t = static_cast<T>(0);
  // Ray should still hit the AABB on the boundary.
  EXPECT_TRUE(mathkata::rayIntersectsAABB(ray, aabb, &t));
  EXPECT_NEAR(t, static_cast<T>(4), precision);
}
TEST_ALL_DIMS_F(RayIntersectsAABB_AlongEdge)

template <class T, int N>
void RayIntersectsAABB_Parallel_Test(T precision) {
  (void)precision;
  // Ray parallel to a slab but outside the AABB.
  mathkata::Vector<T, N> origin(static_cast<T>(0));
  origin[0] = static_cast<T>(-5);
  if (N >= 2) {
    origin[1] = static_cast<T>(3);  // Outside the AABB.
  }
  mathkata::Vector<T, N> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);
  mathkata::Ray<T, N> ray(origin, direction);

  mathkata::Vector<T, N> box_min(static_cast<T>(-1));
  mathkata::Vector<T, N> box_max(static_cast<T>(1));
  mathkata::AABB<T, N> aabb(box_min, box_max);

  if (N >= 2) {
    EXPECT_FALSE(mathkata::rayIntersectsAABB(ray, aabb));
  }
}
TEST_ALL_DIMS_F(RayIntersectsAABB_Parallel)

// ============================================================================
// rayIntersectsPlane tests (3D only)
// ============================================================================

template <class T>
void RayIntersectsPlane_Hit_Test(T precision) {
  // Ray pointing toward a plane at z=5.
  mathkata::Ray<T, 3> ray(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(0)),
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(1)));
  mathkata::Plane<T> plane(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(1)),
      static_cast<T>(-5));

  T t = static_cast<T>(0);
  EXPECT_TRUE(mathkata::rayIntersectsPlane(ray, plane, &t));
  EXPECT_NEAR(t, static_cast<T>(5), precision);
}
TEST_3D_F(RayIntersectsPlane_Hit)

template <class T>
void RayIntersectsPlane_Parallel_Test(T precision) {
  (void)precision;
  // Ray parallel to the plane.
  mathkata::Ray<T, 3> ray(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(0)),
      mathkata::Vector<T, 3>(static_cast<T>(1), static_cast<T>(0),
                             static_cast<T>(0)));
  mathkata::Plane<T> plane(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(1)),
      static_cast<T>(-5));

  EXPECT_FALSE(mathkata::rayIntersectsPlane(ray, plane));
}
TEST_3D_F(RayIntersectsPlane_Parallel)

template <class T>
void RayIntersectsPlane_FromBehind_Test(T precision) {
  (void)precision;
  // Ray pointing away from the plane.
  mathkata::Ray<T, 3> ray(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(0)),
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(-1)));
  mathkata::Plane<T> plane(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(1)),
      static_cast<T>(-5));

  EXPECT_FALSE(mathkata::rayIntersectsPlane(ray, plane));
}
TEST_3D_F(RayIntersectsPlane_FromBehind)

// ============================================================================
// aabbIntersectsAABB tests
// ============================================================================

template <class T, int N>
void AABBIntersectsAABB_Overlap_Test(T precision) {
  (void)precision;
  mathkata::AABB<T, N> a(mathkata::Vector<T, N>(static_cast<T>(0)),
                         mathkata::Vector<T, N>(static_cast<T>(3)));
  mathkata::AABB<T, N> b(mathkata::Vector<T, N>(static_cast<T>(2)),
                         mathkata::Vector<T, N>(static_cast<T>(5)));
  EXPECT_TRUE(mathkata::aabbIntersectsAABB(a, b));
}
TEST_ALL_DIMS_F(AABBIntersectsAABB_Overlap)

template <class T, int N>
void AABBIntersectsAABB_Touching_Test(T precision) {
  (void)precision;
  mathkata::AABB<T, N> a(mathkata::Vector<T, N>(static_cast<T>(0)),
                         mathkata::Vector<T, N>(static_cast<T>(2)));
  mathkata::AABB<T, N> b(mathkata::Vector<T, N>(static_cast<T>(2)),
                         mathkata::Vector<T, N>(static_cast<T>(4)));
  EXPECT_TRUE(mathkata::aabbIntersectsAABB(a, b));
}
TEST_ALL_DIMS_F(AABBIntersectsAABB_Touching)

template <class T, int N>
void AABBIntersectsAABB_Separate_Test(T precision) {
  (void)precision;
  mathkata::AABB<T, N> a(mathkata::Vector<T, N>(static_cast<T>(0)),
                         mathkata::Vector<T, N>(static_cast<T>(1)));
  mathkata::AABB<T, N> b(mathkata::Vector<T, N>(static_cast<T>(3)),
                         mathkata::Vector<T, N>(static_cast<T>(5)));
  EXPECT_FALSE(mathkata::aabbIntersectsAABB(a, b));
}
TEST_ALL_DIMS_F(AABBIntersectsAABB_Separate)

// ============================================================================
// sphereIntersectsSphere tests
// ============================================================================

template <class T, int N>
void SphereIntersectsSphere_Overlap_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> c1(static_cast<T>(0));
  mathkata::Vector<T, N> c2(static_cast<T>(0));
  c2[0] = static_cast<T>(1);
  mathkata::Sphere<T, N> s1(c1, static_cast<T>(2));
  mathkata::Sphere<T, N> s2(c2, static_cast<T>(2));
  EXPECT_TRUE(mathkata::sphereIntersectsSphere(s1, s2));
}
TEST_ALL_DIMS_F(SphereIntersectsSphere_Overlap)

template <class T, int N>
void SphereIntersectsSphere_Touching_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> c1(static_cast<T>(0));
  mathkata::Vector<T, N> c2(static_cast<T>(0));
  c2[0] = static_cast<T>(4);
  mathkata::Sphere<T, N> s1(c1, static_cast<T>(2));
  mathkata::Sphere<T, N> s2(c2, static_cast<T>(2));
  EXPECT_TRUE(mathkata::sphereIntersectsSphere(s1, s2));
}
TEST_ALL_DIMS_F(SphereIntersectsSphere_Touching)

template <class T, int N>
void SphereIntersectsSphere_Separate_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> c1(static_cast<T>(0));
  mathkata::Vector<T, N> c2(static_cast<T>(0));
  c2[0] = static_cast<T>(10);
  mathkata::Sphere<T, N> s1(c1, static_cast<T>(2));
  mathkata::Sphere<T, N> s2(c2, static_cast<T>(2));
  EXPECT_FALSE(mathkata::sphereIntersectsSphere(s1, s2));
}
TEST_ALL_DIMS_F(SphereIntersectsSphere_Separate)

// ============================================================================
// sphereIntersectsAABB tests
// ============================================================================

template <class T, int N>
void SphereIntersectsAABB_Overlap_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center(static_cast<T>(0));
  center[0] = static_cast<T>(2);
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(2));

  mathkata::AABB<T, N> aabb(mathkata::Vector<T, N>(static_cast<T>(-1)),
                            mathkata::Vector<T, N>(static_cast<T>(1)));
  EXPECT_TRUE(mathkata::sphereIntersectsAABB(sphere, aabb));
}
TEST_ALL_DIMS_F(SphereIntersectsAABB_Overlap)

template <class T, int N>
void SphereIntersectsAABB_Separate_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center(static_cast<T>(0));
  center[0] = static_cast<T>(10);
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(1));

  mathkata::AABB<T, N> aabb(mathkata::Vector<T, N>(static_cast<T>(-1)),
                            mathkata::Vector<T, N>(static_cast<T>(1)));
  EXPECT_FALSE(mathkata::sphereIntersectsAABB(sphere, aabb));
}
TEST_ALL_DIMS_F(SphereIntersectsAABB_Separate)

template <class T, int N>
void SphereIntersectsAABB_SphereInsideAABB_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(0.5));

  mathkata::AABB<T, N> aabb(mathkata::Vector<T, N>(static_cast<T>(-5)),
                            mathkata::Vector<T, N>(static_cast<T>(5)));
  EXPECT_TRUE(mathkata::sphereIntersectsAABB(sphere, aabb));
}
TEST_ALL_DIMS_F(SphereIntersectsAABB_SphereInsideAABB)

// ============================================================================
// pointInAABB tests
// ============================================================================

template <class T, int N>
void PointInAABB_Inside_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> point(static_cast<T>(0));
  mathkata::AABB<T, N> aabb(mathkata::Vector<T, N>(static_cast<T>(-1)),
                            mathkata::Vector<T, N>(static_cast<T>(1)));
  EXPECT_TRUE(mathkata::pointInAABB(point, aabb));
}
TEST_ALL_DIMS_F(PointInAABB_Inside)

template <class T, int N>
void PointInAABB_Outside_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> point(static_cast<T>(5));
  mathkata::AABB<T, N> aabb(mathkata::Vector<T, N>(static_cast<T>(-1)),
                            mathkata::Vector<T, N>(static_cast<T>(1)));
  EXPECT_FALSE(mathkata::pointInAABB(point, aabb));
}
TEST_ALL_DIMS_F(PointInAABB_Outside)

template <class T, int N>
void PointInAABB_OnBoundary_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> point(static_cast<T>(1));
  mathkata::AABB<T, N> aabb(mathkata::Vector<T, N>(static_cast<T>(-1)),
                            mathkata::Vector<T, N>(static_cast<T>(1)));
  EXPECT_TRUE(mathkata::pointInAABB(point, aabb));
}
TEST_ALL_DIMS_F(PointInAABB_OnBoundary)

// ============================================================================
// pointInSphere tests
// ============================================================================

template <class T, int N>
void PointInSphere_Inside_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> point(static_cast<T>(0));
  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(5));
  EXPECT_TRUE(mathkata::pointInSphere(point, sphere));
}
TEST_ALL_DIMS_F(PointInSphere_Inside)

template <class T, int N>
void PointInSphere_Outside_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> point(static_cast<T>(10));
  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(1));
  EXPECT_FALSE(mathkata::pointInSphere(point, sphere));
}
TEST_ALL_DIMS_F(PointInSphere_Outside)

template <class T, int N>
void PointInSphere_OnBoundary_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> point(static_cast<T>(0));
  point[0] = static_cast<T>(3);
  mathkata::Vector<T, N> center(static_cast<T>(0));
  mathkata::Sphere<T, N> sphere(center, static_cast<T>(3));
  EXPECT_TRUE(mathkata::pointInSphere(point, sphere));
}
TEST_ALL_DIMS_F(PointInSphere_OnBoundary)

// ============================================================================
// pointOnPlane tests (3D only)
// ============================================================================

template <class T>
void PointOnPlane_OnPlane_Test(T precision) {
  mathkata::Plane<T> plane(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(1),
                             static_cast<T>(0)),
      static_cast<T>(-3));
  // Point at y=3 should be on the plane y=3.
  mathkata::Vector<T, 3> point(static_cast<T>(1), static_cast<T>(3),
                               static_cast<T>(2));
  EXPECT_TRUE(mathkata::pointOnPlane(point, plane, precision));
}
TEST_3D_F(PointOnPlane_OnPlane)

template <class T>
void PointOnPlane_OffPlane_Test(T precision) {
  mathkata::Plane<T> plane(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(1),
                             static_cast<T>(0)),
      static_cast<T>(-3));
  // Point at y=5 should NOT be on the plane y=3.
  mathkata::Vector<T, 3> point(static_cast<T>(1), static_cast<T>(5),
                               static_cast<T>(2));
  EXPECT_FALSE(mathkata::pointOnPlane(point, plane, precision));
}
TEST_3D_F(PointOnPlane_OffPlane)

template <class T>
void PointOnPlane_NearPlane_Test(T precision) {
  mathkata::Plane<T> plane(
      mathkata::Vector<T, 3>(static_cast<T>(0), static_cast<T>(0),
                             static_cast<T>(1)),
      static_cast<T>(0));
  // Point very close to the z=0 plane.
  mathkata::Vector<T, 3> point(static_cast<T>(1), static_cast<T>(2),
                               precision * static_cast<T>(0.5));
  EXPECT_TRUE(
      mathkata::pointOnPlane(point, plane, precision * static_cast<T>(2)));
}
TEST_3D_F(PointOnPlane_NearPlane)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

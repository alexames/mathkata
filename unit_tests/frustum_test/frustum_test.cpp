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
#include "mathkata/frustum.h"

#include <cmath>
#include <cstdio>
#include <numbers>

#include "gtest/gtest.h"
#include "mathkata/constants.h"
#include "mathkata/utilities.h"
#include "precision.h"

class FrustumTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Helper macro to generate float and double test variants.
#define TEST_FRUSTUM_F(MY_TEST)               \
  TEST_F(FrustumTests, MY_TEST##_float) {     \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
  }                                           \
  TEST_F(FrustumTests, MY_TEST##_double) {    \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

// Convenience typedefs.
template <class T>
using Vec3 = mathkata::Vector<T, 3>;

template <class T>
using Mat4 = mathkata::Matrix<T, 4, 4>;

// Helper to create an orthographic projection matrix for testing.
// This creates a simple box frustum which is easy to reason about.
// Maps x in [-hw, hw], y in [-hh, hh], z in [-near, -far] to NDC.
template <class T>
Mat4<T> MakeOrthoMatrix(T hw, T hh, T near_val, T far_val) {
  return Mat4<T>::Ortho(-hw, hw, -hh, hh, near_val, far_val);
}

// Helper to create a symmetric perspective projection matrix for testing.
template <class T>
Mat4<T> MakePerspectiveMatrix(T fovy, T aspect, T near_val, T far_val) {
  return Mat4<T>::Perspective(fovy, aspect, near_val, far_val);
}

// Test: Construction from 6 planes.
template <class T>
void ConstructFromPlanes_Test(T /*precision*/) {
  const mathkata::Plane<T> near_p(Vec3<T>(0, 0, 1), static_cast<T>(1));
  const mathkata::Plane<T> far_p(Vec3<T>(0, 0, -1), static_cast<T>(10));
  const mathkata::Plane<T> left_p(Vec3<T>(1, 0, 0), static_cast<T>(5));
  const mathkata::Plane<T> right_p(Vec3<T>(-1, 0, 0), static_cast<T>(5));
  const mathkata::Plane<T> top_p(Vec3<T>(0, -1, 0), static_cast<T>(5));
  const mathkata::Plane<T> bottom_p(Vec3<T>(0, 1, 0), static_cast<T>(5));

  const mathkata::Frustum<T> frustum(near_p, far_p, left_p, right_p, top_p,
                                     bottom_p);

  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kNear), near_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kFar), far_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kLeft), left_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kRight), right_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kTop), top_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kBottom), bottom_p);
}
TEST_FRUSTUM_F(ConstructFromPlanes)

// Test: FromViewProjection with an orthographic matrix produces valid planes.
template <class T>
void FromViewProjectionOrtho_Test(T precision) {
  // Orthographic: x in [-10, 10], y in [-5, 5], z in [-1, -100]
  const T hw = static_cast<T>(10);
  const T hh = static_cast<T>(5);
  const T near_val = static_cast<T>(1);
  const T far_val = static_cast<T>(100);

  const Mat4<T> ortho = MakeOrthoMatrix(hw, hh, near_val, far_val);
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(ortho);

  // The origin should be inside the frustum (it's between near and far,
  // and within x/y bounds). Actually, with RH convention, the camera looks
  // down -Z, and the near plane is at z = -near = -1, far at z = -far = -100.
  // The origin (0,0,0) is in front of the near plane (z=0 > z=-1), so
  // it should be outside.
  // A point at (0, 0, -50) should be inside.
  const Vec3<T> inside(0, 0, static_cast<T>(-50));
  EXPECT_TRUE(frustum.ContainsPoint(inside));

  // A point outside to the left.
  const Vec3<T> outside_left(static_cast<T>(-15), 0, static_cast<T>(-50));
  EXPECT_FALSE(frustum.ContainsPoint(outside_left));

  // A point outside to the right.
  const Vec3<T> outside_right(static_cast<T>(15), 0, static_cast<T>(-50));
  EXPECT_FALSE(frustum.ContainsPoint(outside_right));

  // A point behind the far plane.
  const Vec3<T> outside_far(0, 0, static_cast<T>(-200));
  EXPECT_FALSE(frustum.ContainsPoint(outside_far));

  // A point in front of the near plane.
  const Vec3<T> outside_near(0, 0, static_cast<T>(5));
  EXPECT_FALSE(frustum.ContainsPoint(outside_near));

  // Verify that all extracted plane normals are unit length.
  for (int i = 0; i < mathkata::Frustum<T>::kPlaneCount; ++i) {
    const T len =
        frustum
            .GetPlane(
                static_cast<typename mathkata::Frustum<T>::FrustumPlane>(i))
            .normal.Length();
    EXPECT_NEAR(static_cast<double>(len), 1.0, static_cast<double>(precision));
  }
}
TEST_FRUSTUM_F(FromViewProjectionOrtho)

// Test: FromViewProjection with a perspective matrix.
template <class T>
void FromViewProjectionPerspective_Test(T precision) {
  const T fovy = std::numbers::pi_v<T> / static_cast<T>(2);  // 90 degrees
  const T aspect = static_cast<T>(1);                        // Square viewport
  const T near_val = static_cast<T>(1);
  const T far_val = static_cast<T>(100);

  const Mat4<T> persp = MakePerspectiveMatrix(fovy, aspect, near_val, far_val);
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(persp);

  // A point at the center of the frustum should be inside.
  const Vec3<T> inside(0, 0, static_cast<T>(-10));
  EXPECT_TRUE(frustum.ContainsPoint(inside));

  // A point behind the camera should be outside.
  const Vec3<T> behind(0, 0, static_cast<T>(5));
  EXPECT_FALSE(frustum.ContainsPoint(behind));

  // A point beyond the far plane should be outside.
  const Vec3<T> beyond_far(0, 0, static_cast<T>(-200));
  EXPECT_FALSE(frustum.ContainsPoint(beyond_far));

  // With 90-degree FOV and aspect=1, at z=-10 the half-width is 10.
  // A point at (9, 0, -10) should be inside.
  const Vec3<T> inside_edge(static_cast<T>(9), 0, static_cast<T>(-10));
  EXPECT_TRUE(frustum.ContainsPoint(inside_edge));

  // A point at (11, 0, -10) should be outside.
  const Vec3<T> outside_right(static_cast<T>(11), 0, static_cast<T>(-10));
  EXPECT_FALSE(frustum.ContainsPoint(outside_right));

  // Verify unit normals.
  for (int i = 0; i < mathkata::Frustum<T>::kPlaneCount; ++i) {
    const T len =
        frustum
            .GetPlane(
                static_cast<typename mathkata::Frustum<T>::FrustumPlane>(i))
            .normal.Length();
    EXPECT_NEAR(static_cast<double>(len), 1.0, static_cast<double>(precision));
  }
}
TEST_FRUSTUM_F(FromViewProjectionPerspective)

// Test: ContainsPoint with points inside and outside each plane.
template <class T>
void ContainsPointAllPlanes_Test(T /*precision*/) {
  // Use an orthographic frustum for simplicity:
  // x in [-10, 10], y in [-5, 5], z in [-1, -100]
  const Mat4<T> ortho = MakeOrthoMatrix(static_cast<T>(10), static_cast<T>(5),
                                        static_cast<T>(1), static_cast<T>(100));
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(ortho);

  // Center of the frustum.
  const Vec3<T> center(0, 0, static_cast<T>(-50));
  EXPECT_TRUE(frustum.ContainsPoint(center));

  // Outside left (x < -10).
  EXPECT_FALSE(frustum.ContainsPoint(
      Vec3<T>(static_cast<T>(-11), 0, static_cast<T>(-50))));

  // Outside right (x > 10).
  EXPECT_FALSE(frustum.ContainsPoint(
      Vec3<T>(static_cast<T>(11), 0, static_cast<T>(-50))));

  // Outside bottom (y < -5).
  EXPECT_FALSE(frustum.ContainsPoint(
      Vec3<T>(0, static_cast<T>(-6), static_cast<T>(-50))));

  // Outside top (y > 5).
  EXPECT_FALSE(frustum.ContainsPoint(
      Vec3<T>(0, static_cast<T>(6), static_cast<T>(-50))));

  // Outside near (z > -1, i.e., in front of near plane).
  EXPECT_FALSE(frustum.ContainsPoint(Vec3<T>(0, 0, static_cast<T>(0))));

  // Outside far (z < -100, i.e., behind far plane).
  EXPECT_FALSE(frustum.ContainsPoint(Vec3<T>(0, 0, static_cast<T>(-101))));
}
TEST_FRUSTUM_F(ContainsPointAllPlanes)

// Test: IntersectsAABB - box fully inside the frustum.
template <class T>
void IntersectsAABBInside_Test(T /*precision*/) {
  const Mat4<T> ortho = MakeOrthoMatrix(static_cast<T>(10), static_cast<T>(5),
                                        static_cast<T>(1), static_cast<T>(100));
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(ortho);

  // An AABB fully inside the frustum.
  const mathkata::AABB<T, 3> inside(
      Vec3<T>(static_cast<T>(-2), static_cast<T>(-2), static_cast<T>(-60)),
      Vec3<T>(static_cast<T>(2), static_cast<T>(2), static_cast<T>(-40)));
  EXPECT_TRUE(frustum.IntersectsAABB(inside));
}
TEST_FRUSTUM_F(IntersectsAABBInside)

// Test: IntersectsAABB - box fully outside the frustum.
template <class T>
void IntersectsAABBOutside_Test(T /*precision*/) {
  const Mat4<T> ortho = MakeOrthoMatrix(static_cast<T>(10), static_cast<T>(5),
                                        static_cast<T>(1), static_cast<T>(100));
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(ortho);

  // An AABB fully to the right of the frustum.
  const mathkata::AABB<T, 3> outside_right(
      Vec3<T>(static_cast<T>(20), static_cast<T>(-1), static_cast<T>(-60)),
      Vec3<T>(static_cast<T>(30), static_cast<T>(1), static_cast<T>(-40)));
  EXPECT_FALSE(frustum.IntersectsAABB(outside_right));

  // An AABB fully behind the far plane.
  const mathkata::AABB<T, 3> outside_far(
      Vec3<T>(static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(-200)),
      Vec3<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(-150)));
  EXPECT_FALSE(frustum.IntersectsAABB(outside_far));

  // An AABB fully in front of the near plane.
  const mathkata::AABB<T, 3> outside_near(
      Vec3<T>(static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(5)),
      Vec3<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(10)));
  EXPECT_FALSE(frustum.IntersectsAABB(outside_near));
}
TEST_FRUSTUM_F(IntersectsAABBOutside)

// Test: IntersectsAABB - box straddling the frustum boundary.
template <class T>
void IntersectsAABBStraddling_Test(T /*precision*/) {
  const Mat4<T> ortho = MakeOrthoMatrix(static_cast<T>(10), static_cast<T>(5),
                                        static_cast<T>(1), static_cast<T>(100));
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(ortho);

  // An AABB that straddles the right boundary (partially inside).
  const mathkata::AABB<T, 3> straddle_right(
      Vec3<T>(static_cast<T>(8), static_cast<T>(-1), static_cast<T>(-50)),
      Vec3<T>(static_cast<T>(15), static_cast<T>(1), static_cast<T>(-40)));
  EXPECT_TRUE(frustum.IntersectsAABB(straddle_right));

  // An AABB that straddles the near plane.
  const mathkata::AABB<T, 3> straddle_near(
      Vec3<T>(static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(-5)),
      Vec3<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(5)));
  EXPECT_TRUE(frustum.IntersectsAABB(straddle_near));

  // An AABB that straddles the far plane.
  const mathkata::AABB<T, 3> straddle_far(
      Vec3<T>(static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(-105)),
      Vec3<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(-95)));
  EXPECT_TRUE(frustum.IntersectsAABB(straddle_far));
}
TEST_FRUSTUM_F(IntersectsAABBStraddling)

// Test: IntersectsSphere - sphere fully inside the frustum.
template <class T>
void IntersectsSphereInside_Test(T /*precision*/) {
  const Mat4<T> ortho = MakeOrthoMatrix(static_cast<T>(10), static_cast<T>(5),
                                        static_cast<T>(1), static_cast<T>(100));
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(ortho);

  const mathkata::Sphere<T, 3> inside(Vec3<T>(0, 0, static_cast<T>(-50)),
                                      static_cast<T>(2));
  EXPECT_TRUE(frustum.IntersectsSphere(inside));
}
TEST_FRUSTUM_F(IntersectsSphereInside)

// Test: IntersectsSphere - sphere fully outside the frustum.
template <class T>
void IntersectsSphereOutside_Test(T /*precision*/) {
  const Mat4<T> ortho = MakeOrthoMatrix(static_cast<T>(10), static_cast<T>(5),
                                        static_cast<T>(1), static_cast<T>(100));
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(ortho);

  // Sphere far to the right.
  const mathkata::Sphere<T, 3> outside(
      Vec3<T>(static_cast<T>(20), 0, static_cast<T>(-50)), static_cast<T>(2));
  EXPECT_FALSE(frustum.IntersectsSphere(outside));

  // Sphere behind far plane.
  const mathkata::Sphere<T, 3> outside_far(Vec3<T>(0, 0, static_cast<T>(-200)),
                                           static_cast<T>(2));
  EXPECT_FALSE(frustum.IntersectsSphere(outside_far));

  // Sphere in front of near plane.
  const mathkata::Sphere<T, 3> outside_near(Vec3<T>(0, 0, static_cast<T>(10)),
                                            static_cast<T>(2));
  EXPECT_FALSE(frustum.IntersectsSphere(outside_near));
}
TEST_FRUSTUM_F(IntersectsSphereOutside)

// Test: IntersectsSphere - sphere straddling the frustum boundary.
template <class T>
void IntersectsSphereStraddling_Test(T /*precision*/) {
  const Mat4<T> ortho = MakeOrthoMatrix(static_cast<T>(10), static_cast<T>(5),
                                        static_cast<T>(1), static_cast<T>(100));
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(ortho);

  // Sphere centered just outside the right boundary but with radius that
  // reaches inside.
  const mathkata::Sphere<T, 3> straddle(
      Vec3<T>(static_cast<T>(11), 0, static_cast<T>(-50)), static_cast<T>(3));
  EXPECT_TRUE(frustum.IntersectsSphere(straddle));

  // Sphere centered just outside the right boundary and too far away.
  const mathkata::Sphere<T, 3> too_far(
      Vec3<T>(static_cast<T>(15), 0, static_cast<T>(-50)), static_cast<T>(2));
  EXPECT_FALSE(frustum.IntersectsSphere(too_far));
}
TEST_FRUSTUM_F(IntersectsSphereStraddling)

// Test: Equality operators.
template <class T>
void Equality_Test(T /*precision*/) {
  const mathkata::Plane<T> p1(Vec3<T>(1, 0, 0), static_cast<T>(1));
  const mathkata::Plane<T> p2(Vec3<T>(-1, 0, 0), static_cast<T>(1));
  const mathkata::Plane<T> p3(Vec3<T>(0, 1, 0), static_cast<T>(1));
  const mathkata::Plane<T> p4(Vec3<T>(0, -1, 0), static_cast<T>(1));
  const mathkata::Plane<T> p5(Vec3<T>(0, 0, 1), static_cast<T>(1));
  const mathkata::Plane<T> p6(Vec3<T>(0, 0, -1), static_cast<T>(1));

  const mathkata::Frustum<T> f1(p5, p6, p1, p2, p4, p3);
  const mathkata::Frustum<T> f2(p5, p6, p1, p2, p4, p3);

  EXPECT_TRUE(f1 == f2);
  EXPECT_FALSE(f1 != f2);

  // Different frustum.
  const mathkata::Plane<T> p7(Vec3<T>(0, 0, -1), static_cast<T>(5));
  const mathkata::Frustum<T> f3(p5, p7, p1, p2, p4, p3);
  EXPECT_FALSE(f1 == f3);
  EXPECT_TRUE(f1 != f3);
}
TEST_FRUSTUM_F(Equality)

// Test: GetPlane accessor.
template <class T>
void GetPlane_Test(T /*precision*/) {
  const mathkata::Plane<T> near_p(Vec3<T>(0, 0, 1), static_cast<T>(1));
  const mathkata::Plane<T> far_p(Vec3<T>(0, 0, -1), static_cast<T>(100));
  const mathkata::Plane<T> left_p(Vec3<T>(1, 0, 0), static_cast<T>(5));
  const mathkata::Plane<T> right_p(Vec3<T>(-1, 0, 0), static_cast<T>(5));
  const mathkata::Plane<T> top_p(Vec3<T>(0, -1, 0), static_cast<T>(5));
  const mathkata::Plane<T> bottom_p(Vec3<T>(0, 1, 0), static_cast<T>(5));

  const mathkata::Frustum<T> frustum(near_p, far_p, left_p, right_p, top_p,
                                     bottom_p);

  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kNear), near_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kFar), far_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kLeft), left_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kRight), right_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kTop), top_p);
  EXPECT_EQ(frustum.GetPlane(mathkata::Frustum<T>::kBottom), bottom_p);
}
TEST_FRUSTUM_F(GetPlane)

// Test: Perspective frustum with various points to verify correctness.
template <class T>
void PerspectiveCulling_Test(T /*precision*/) {
  // 90-degree FOV, aspect 16:9, near=0.1, far=1000
  const T fovy = std::numbers::pi_v<T> / static_cast<T>(2);
  const T aspect = static_cast<T>(16) / static_cast<T>(9);
  const T near_val = static_cast<T>(0.1);
  const T far_val = static_cast<T>(1000);

  const Mat4<T> persp = MakePerspectiveMatrix(fovy, aspect, near_val, far_val);
  const mathkata::Frustum<T> frustum =
      mathkata::Frustum<T>::FromViewProjection(persp);

  // Point directly in front of camera, well inside frustum.
  EXPECT_TRUE(frustum.ContainsPoint(Vec3<T>(0, 0, static_cast<T>(-10))));

  // Point just inside the near plane.
  EXPECT_TRUE(frustum.ContainsPoint(Vec3<T>(0, 0, static_cast<T>(-0.2))));

  // Point just inside the far plane.
  EXPECT_TRUE(frustum.ContainsPoint(Vec3<T>(0, 0, static_cast<T>(-999))));

  // Point behind camera.
  EXPECT_FALSE(frustum.ContainsPoint(Vec3<T>(0, 0, static_cast<T>(1))));

  // Point beyond far plane.
  EXPECT_FALSE(frustum.ContainsPoint(Vec3<T>(0, 0, static_cast<T>(-1001))));
}
TEST_FRUSTUM_F(PerspectiveCulling)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

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
#include "mathkata/aabb.h"

#include <cstdio>

#include "gtest/gtest.h"
#include "mathkata/utilities.h"
#include "precision.h"

class AABBTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Helper macro to generate tests for float and double with 2D and 3D.
#define TEST_ALL_AABB_F(MY_TEST)                 \
  TEST_F(AABBTests, MY_TEST##_float_2) {         \
    MY_TEST##_Test<float, 2>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(AABBTests, MY_TEST##_double_2) {        \
    MY_TEST##_Test<double, 2>(DOUBLE_PRECISION); \
  }                                              \
  TEST_F(AABBTests, MY_TEST##_float_3) {         \
    MY_TEST##_Test<float, 3>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(AABBTests, MY_TEST##_double_3) {        \
    MY_TEST##_Test<double, 3>(DOUBLE_PRECISION); \
  }

// --- Construction tests ---

template <class T, int N>
void ConstructDefault_Test(T /*precision*/) {
  // Default constructor should compile and not crash.
  mathkata::AABB<T, N> box;
  (void)box;
}
TEST_ALL_AABB_F(ConstructDefault)

template <class T, int N>
void ConstructMinMax_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, N> lo(static_cast<T>(1));
  mathkata::Vector<T, N> hi(static_cast<T>(5));
  mathkata::AABB<T, N> box(lo, hi);
  EXPECT_EQ(box.min, lo);
  EXPECT_EQ(box.max, hi);
}
TEST_ALL_AABB_F(ConstructMinMax)

template <class T, int N>
void ConstructCenterExtents_Test(T precision) {
  mathkata::Vector<T, N> center(static_cast<T>(3));
  mathkata::Vector<T, N> extents(static_cast<T>(2));
  mathkata::AABB<T, N> box =
      mathkata::AABB<T, N>::FromCenterExtents(center, extents);
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(box.min[i], static_cast<T>(1), precision);
    EXPECT_NEAR(box.max[i], static_cast<T>(5), precision);
  }
}
TEST_ALL_AABB_F(ConstructCenterExtents)

// --- Center, Extents, Size tests ---

template <class T, int N>
void Center_Test(T precision) {
  mathkata::Vector<T, N> lo(static_cast<T>(2));
  mathkata::Vector<T, N> hi(static_cast<T>(8));
  mathkata::AABB<T, N> box(lo, hi);
  mathkata::Vector<T, N> c = box.Center();
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(c[i], static_cast<T>(5), precision);
  }
}
TEST_ALL_AABB_F(Center)

template <class T, int N>
void Extents_Test(T precision) {
  mathkata::Vector<T, N> lo(static_cast<T>(2));
  mathkata::Vector<T, N> hi(static_cast<T>(8));
  mathkata::AABB<T, N> box(lo, hi);
  mathkata::Vector<T, N> ext = box.Extents();
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(ext[i], static_cast<T>(3), precision);
  }
}
TEST_ALL_AABB_F(Extents)

template <class T, int N>
void Size_Test(T precision) {
  mathkata::Vector<T, N> lo(static_cast<T>(2));
  mathkata::Vector<T, N> hi(static_cast<T>(8));
  mathkata::AABB<T, N> box(lo, hi);
  mathkata::Vector<T, N> s = box.Size();
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(s[i], static_cast<T>(6), precision);
  }
}
TEST_ALL_AABB_F(Size)

// --- Contains (point) tests ---

template <class T, int N>
void ContainsPoint_Test(T /*precision*/) {
  mathkata::Vector<T, N> lo(static_cast<T>(0));
  mathkata::Vector<T, N> hi(static_cast<T>(10));
  mathkata::AABB<T, N> box(lo, hi);

  // Point inside
  mathkata::Vector<T, N> inside(static_cast<T>(5));
  EXPECT_TRUE(box.Contains(inside));

  // Point on min boundary
  EXPECT_TRUE(box.Contains(lo));

  // Point on max boundary
  EXPECT_TRUE(box.Contains(hi));

  // Point outside (below min)
  mathkata::Vector<T, N> below(static_cast<T>(-1));
  EXPECT_FALSE(box.Contains(below));

  // Point outside (above max)
  mathkata::Vector<T, N> above(static_cast<T>(11));
  EXPECT_FALSE(box.Contains(above));
}
TEST_ALL_AABB_F(ContainsPoint)

// --- Contains (AABB) tests ---

template <class T, int N>
void ContainsAABB_Test(T /*precision*/) {
  mathkata::Vector<T, N> lo(static_cast<T>(0));
  mathkata::Vector<T, N> hi(static_cast<T>(10));
  mathkata::AABB<T, N> outer(lo, hi);

  // Inner box fully contained
  mathkata::AABB<T, N> inner(mathkata::Vector<T, N>(static_cast<T>(2)),
                             mathkata::Vector<T, N>(static_cast<T>(8)));
  EXPECT_TRUE(outer.Contains(inner));

  // Same box contains itself
  EXPECT_TRUE(outer.Contains(outer));

  // Partially overlapping box is not contained
  mathkata::AABB<T, N> partial(mathkata::Vector<T, N>(static_cast<T>(5)),
                               mathkata::Vector<T, N>(static_cast<T>(15)));
  EXPECT_FALSE(outer.Contains(partial));

  // Completely outside box is not contained
  mathkata::AABB<T, N> outside(mathkata::Vector<T, N>(static_cast<T>(11)),
                               mathkata::Vector<T, N>(static_cast<T>(20)));
  EXPECT_FALSE(outer.Contains(outside));
}
TEST_ALL_AABB_F(ContainsAABB)

// --- Intersects tests ---

template <class T, int N>
void Intersects_Test(T /*precision*/) {
  mathkata::Vector<T, N> lo(static_cast<T>(0));
  mathkata::Vector<T, N> hi(static_cast<T>(10));
  mathkata::AABB<T, N> box(lo, hi);

  // Overlapping box
  mathkata::AABB<T, N> overlapping(mathkata::Vector<T, N>(static_cast<T>(5)),
                                   mathkata::Vector<T, N>(static_cast<T>(15)));
  EXPECT_TRUE(box.Intersects(overlapping));

  // Touching at boundary
  mathkata::AABB<T, N> touching(mathkata::Vector<T, N>(static_cast<T>(10)),
                                mathkata::Vector<T, N>(static_cast<T>(20)));
  EXPECT_TRUE(box.Intersects(touching));

  // Fully contained box
  mathkata::AABB<T, N> inner(mathkata::Vector<T, N>(static_cast<T>(2)),
                             mathkata::Vector<T, N>(static_cast<T>(8)));
  EXPECT_TRUE(box.Intersects(inner));

  // Non-overlapping box
  mathkata::AABB<T, N> separate(mathkata::Vector<T, N>(static_cast<T>(11)),
                                mathkata::Vector<T, N>(static_cast<T>(20)));
  EXPECT_FALSE(box.Intersects(separate));

  // Self intersection
  EXPECT_TRUE(box.Intersects(box));
}
TEST_ALL_AABB_F(Intersects)

// --- Intersection tests ---

template <class T, int N>
void Intersection_Test(T precision) {
  mathkata::AABB<T, N> a(mathkata::Vector<T, N>(static_cast<T>(0)),
                         mathkata::Vector<T, N>(static_cast<T>(10)));
  mathkata::AABB<T, N> b(mathkata::Vector<T, N>(static_cast<T>(5)),
                         mathkata::Vector<T, N>(static_cast<T>(15)));
  mathkata::AABB<T, N> result = a.Intersection(b);
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(result.min[i], static_cast<T>(5), precision);
    EXPECT_NEAR(result.max[i], static_cast<T>(10), precision);
  }
}
TEST_ALL_AABB_F(Intersection)

// --- Union tests ---

template <class T, int N>
void Union_Test(T precision) {
  mathkata::AABB<T, N> a(mathkata::Vector<T, N>(static_cast<T>(0)),
                         mathkata::Vector<T, N>(static_cast<T>(5)));
  mathkata::AABB<T, N> b(mathkata::Vector<T, N>(static_cast<T>(3)),
                         mathkata::Vector<T, N>(static_cast<T>(10)));
  mathkata::AABB<T, N> result = a.Union(b);
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(result.min[i], static_cast<T>(0), precision);
    EXPECT_NEAR(result.max[i], static_cast<T>(10), precision);
  }
}
TEST_ALL_AABB_F(Union)

// --- Expand (point) tests ---

template <class T, int N>
void ExpandPoint_Test(T precision) {
  mathkata::AABB<T, N> box(mathkata::Vector<T, N>(static_cast<T>(0)),
                           mathkata::Vector<T, N>(static_cast<T>(5)));
  mathkata::Vector<T, N> point(static_cast<T>(10));
  box.Expand(point);
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(box.min[i], static_cast<T>(0), precision);
    EXPECT_NEAR(box.max[i], static_cast<T>(10), precision);
  }

  // Expand with point below min
  mathkata::Vector<T, N> below(static_cast<T>(-5));
  box.Expand(below);
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(box.min[i], static_cast<T>(-5), precision);
    EXPECT_NEAR(box.max[i], static_cast<T>(10), precision);
  }

  // Expand with point already inside does nothing
  mathkata::Vector<T, N> inside(static_cast<T>(3));
  box.Expand(inside);
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(box.min[i], static_cast<T>(-5), precision);
    EXPECT_NEAR(box.max[i], static_cast<T>(10), precision);
  }
}
TEST_ALL_AABB_F(ExpandPoint)

// --- Expand (AABB) tests ---

template <class T, int N>
void ExpandAABB_Test(T precision) {
  mathkata::AABB<T, N> box(mathkata::Vector<T, N>(static_cast<T>(0)),
                           mathkata::Vector<T, N>(static_cast<T>(5)));
  mathkata::AABB<T, N> other(mathkata::Vector<T, N>(static_cast<T>(3)),
                             mathkata::Vector<T, N>(static_cast<T>(10)));
  box.Expand(other);
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(box.min[i], static_cast<T>(0), precision);
    EXPECT_NEAR(box.max[i], static_cast<T>(10), precision);
  }

  // Expand with fully contained AABB does nothing
  mathkata::AABB<T, N> inner(mathkata::Vector<T, N>(static_cast<T>(2)),
                             mathkata::Vector<T, N>(static_cast<T>(8)));
  box.Expand(inner);
  for (int i = 0; i < N; ++i) {
    EXPECT_NEAR(box.min[i], static_cast<T>(0), precision);
    EXPECT_NEAR(box.max[i], static_cast<T>(10), precision);
  }
}
TEST_ALL_AABB_F(ExpandAABB)

// --- Equality operator tests ---

template <class T, int N>
void Equality_Test(T /*precision*/) {
  mathkata::AABB<T, N> a(mathkata::Vector<T, N>(static_cast<T>(1)),
                         mathkata::Vector<T, N>(static_cast<T>(5)));
  mathkata::AABB<T, N> b(mathkata::Vector<T, N>(static_cast<T>(1)),
                         mathkata::Vector<T, N>(static_cast<T>(5)));
  mathkata::AABB<T, N> c(mathkata::Vector<T, N>(static_cast<T>(0)),
                         mathkata::Vector<T, N>(static_cast<T>(5)));
  mathkata::AABB<T, N> d(mathkata::Vector<T, N>(static_cast<T>(1)),
                         mathkata::Vector<T, N>(static_cast<T>(6)));

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_FALSE(a == c);
  EXPECT_TRUE(a != c);
  EXPECT_FALSE(a == d);
  EXPECT_TRUE(a != d);
}
TEST_ALL_AABB_F(Equality)

// --- Test with specific 2D values (non-uniform) ---

TEST_F(AABBTests, NonUniform2D_float) {
  mathkata::Vector<float, 2> lo(1.0f, 2.0f);
  mathkata::Vector<float, 2> hi(5.0f, 8.0f);
  mathkata::AABB<float, 2> box(lo, hi);

  mathkata::Vector<float, 2> center = box.Center();
  EXPECT_NEAR(center[0], 3.0f, FLOAT_PRECISION);
  EXPECT_NEAR(center[1], 5.0f, FLOAT_PRECISION);

  mathkata::Vector<float, 2> extents = box.Extents();
  EXPECT_NEAR(extents[0], 2.0f, FLOAT_PRECISION);
  EXPECT_NEAR(extents[1], 3.0f, FLOAT_PRECISION);

  mathkata::Vector<float, 2> size = box.Size();
  EXPECT_NEAR(size[0], 4.0f, FLOAT_PRECISION);
  EXPECT_NEAR(size[1], 6.0f, FLOAT_PRECISION);

  // Point inside
  EXPECT_TRUE(box.Contains(mathkata::Vector<float, 2>(3.0f, 5.0f)));
  // Point outside in x
  EXPECT_FALSE(box.Contains(mathkata::Vector<float, 2>(0.0f, 5.0f)));
  // Point outside in y
  EXPECT_FALSE(box.Contains(mathkata::Vector<float, 2>(3.0f, 9.0f)));
}

// --- Test with specific 3D values (non-uniform) ---

TEST_F(AABBTests, NonUniform3D_double) {
  mathkata::Vector<double, 3> lo(1.0, 2.0, 3.0);
  mathkata::Vector<double, 3> hi(7.0, 10.0, 9.0);
  mathkata::AABB<double, 3> box(lo, hi);

  mathkata::Vector<double, 3> center = box.Center();
  EXPECT_NEAR(center[0], 4.0, DOUBLE_PRECISION);
  EXPECT_NEAR(center[1], 6.0, DOUBLE_PRECISION);
  EXPECT_NEAR(center[2], 6.0, DOUBLE_PRECISION);

  mathkata::Vector<double, 3> extents = box.Extents();
  EXPECT_NEAR(extents[0], 3.0, DOUBLE_PRECISION);
  EXPECT_NEAR(extents[1], 4.0, DOUBLE_PRECISION);
  EXPECT_NEAR(extents[2], 3.0, DOUBLE_PRECISION);

  mathkata::Vector<double, 3> size = box.Size();
  EXPECT_NEAR(size[0], 6.0, DOUBLE_PRECISION);
  EXPECT_NEAR(size[1], 8.0, DOUBLE_PRECISION);
  EXPECT_NEAR(size[2], 6.0, DOUBLE_PRECISION);
}

// --- Test default N=3 template parameter ---

TEST_F(AABBTests, DefaultDimension) {
  // AABB<float> should default to N=3.
  mathkata::AABB<float> box(mathkata::Vector<float, 3>(0.0f, 0.0f, 0.0f),
                            mathkata::Vector<float, 3>(1.0f, 1.0f, 1.0f));
  mathkata::Vector<float, 3> center = box.Center();
  EXPECT_NEAR(center[0], 0.5f, FLOAT_PRECISION);
  EXPECT_NEAR(center[1], 0.5f, FLOAT_PRECISION);
  EXPECT_NEAR(center[2], 0.5f, FLOAT_PRECISION);
}

// --- Test FromCenterExtents round-trip ---

TEST_F(AABBTests, CenterExtentsRoundTrip_float_3) {
  mathkata::Vector<float, 3> center(5.0f, 10.0f, 15.0f);
  mathkata::Vector<float, 3> extents(2.0f, 3.0f, 4.0f);
  mathkata::AABB<float, 3> box =
      mathkata::AABB<float, 3>::FromCenterExtents(center, extents);

  mathkata::Vector<float, 3> computed_center = box.Center();
  mathkata::Vector<float, 3> computed_extents = box.Extents();

  for (int i = 0; i < 3; ++i) {
    EXPECT_NEAR(computed_center[i], center[i], FLOAT_PRECISION);
    EXPECT_NEAR(computed_extents[i], extents[i], FLOAT_PRECISION);
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

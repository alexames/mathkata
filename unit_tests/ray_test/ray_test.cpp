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
#include "mathkata/ray.h"

#include <cmath>
#include <type_traits>

#include "gtest/gtest.h"
#include "precision.h"

class RayTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Helper macro to generate tests for float/double and 2D/3D.
#define TEST_ALL_F(MY_TEST)                      \
  TEST_F(RayTests, MY_TEST##_float_2) {          \
    MY_TEST##_Test<float, 2>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(RayTests, MY_TEST##_double_2) {         \
    MY_TEST##_Test<double, 2>(DOUBLE_PRECISION); \
  }                                              \
  TEST_F(RayTests, MY_TEST##_float_3) {          \
    MY_TEST##_Test<float, 3>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(RayTests, MY_TEST##_double_3) {         \
    MY_TEST##_Test<double, 3>(DOUBLE_PRECISION); \
  }

// --- Ray tests ---

template <class T, int Dims>
void RayConstruction_Test(T precision) {
  (void)precision;
  // Inside the template so it runs for every T and Dims the suite
  // instantiates.
  static_assert(!std::is_default_constructible_v<mathkata::Ray<T, Dims>>);
  auto r1 = mathkata::Ray<T, Dims>::uninitialized();
  r1.origin = mathkata::Vector<T, Dims>(static_cast<T>(4));
  EXPECT_EQ(r1.origin[0], static_cast<T>(4));

  // Construction from origin and direction.
  mathkata::Vector<T, Dims> origin(static_cast<T>(0));
  mathkata::Vector<T, Dims> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);

  mathkata::Ray<T, Dims> r2(origin, direction);
  EXPECT_EQ(r2.origin, origin);
  EXPECT_EQ(r2.direction, direction);
}
TEST_ALL_F(RayConstruction)

template <class T, int Dims>
void RayPointAt_Test(T precision) {
  mathkata::Vector<T, Dims> origin(static_cast<T>(1));
  mathkata::Vector<T, Dims> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);

  mathkata::Ray<T, Dims> ray(origin, direction);

  // At t=0, should be at origin.
  mathkata::Vector<T, Dims> p0 = ray.pointAt(static_cast<T>(0));
  for (int i = 0; i < Dims; ++i) {
    EXPECT_NEAR(p0[i], origin[i], precision);
  }

  // At t=3, should be origin + direction * 3.
  mathkata::Vector<T, Dims> p3 = ray.pointAt(static_cast<T>(3));
  EXPECT_NEAR(p3[0], static_cast<T>(4), precision);
  for (int i = 1; i < Dims; ++i) {
    EXPECT_NEAR(p3[i], static_cast<T>(1), precision);
  }

  // At t=-2, should be origin + direction * -2.
  mathkata::Vector<T, Dims> pn = ray.pointAt(static_cast<T>(-2));
  EXPECT_NEAR(pn[0], static_cast<T>(-1), precision);
}
TEST_ALL_F(RayPointAt)

template <class T, int Dims>
void RayEquality_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, Dims> origin(static_cast<T>(1));
  mathkata::Vector<T, Dims> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);

  mathkata::Ray<T, Dims> r1(origin, direction);
  mathkata::Ray<T, Dims> r2(origin, direction);
  EXPECT_TRUE(r1 == r2);
  EXPECT_FALSE(r1 != r2);

  mathkata::Vector<T, Dims> other_origin(static_cast<T>(2));
  mathkata::Ray<T, Dims> r3(other_origin, direction);
  EXPECT_TRUE(r1 != r3);
  EXPECT_FALSE(r1 == r3);
}
TEST_ALL_F(RayEquality)

// --- Line tests ---

template <class T, int Dims>
void LineConstruction_Test(T precision) {
  (void)precision;
  // Inside the template so it runs for every T and Dims the suite
  // instantiates.
  static_assert(!std::is_default_constructible_v<mathkata::Line<T, Dims>>);
  auto l1 = mathkata::Line<T, Dims>::uninitialized();
  l1.point = mathkata::Vector<T, Dims>(static_cast<T>(4));
  EXPECT_EQ(l1.point[0], static_cast<T>(4));

  // Construction from point and direction.
  mathkata::Vector<T, Dims> point(static_cast<T>(0));
  mathkata::Vector<T, Dims> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);

  mathkata::Line<T, Dims> l2(point, direction);
  EXPECT_EQ(l2.point, point);
  EXPECT_EQ(l2.direction, direction);
}
TEST_ALL_F(LineConstruction)

template <class T, int Dims>
void LineFromPoints_Test(T precision) {
  mathkata::Vector<T, Dims> a(static_cast<T>(0));
  mathkata::Vector<T, Dims> b(static_cast<T>(0));
  b[0] = static_cast<T>(5);

  mathkata::Line<T, Dims> line = mathkata::Line<T, Dims>::fromPoints(a, b);
  EXPECT_EQ(line.point, a);

  // direction should be normalized (1, 0, ...).
  EXPECT_NEAR(line.direction[0], static_cast<T>(1), precision);
  for (int i = 1; i < Dims; ++i) {
    EXPECT_NEAR(line.direction[i], static_cast<T>(0), precision);
  }

  // direction should be unit length.
  EXPECT_NEAR(line.direction.length(), static_cast<T>(1), precision);
}
TEST_ALL_F(LineFromPoints)

template <class T, int Dims>
void LinePointAt_Test(T precision) {
  mathkata::Vector<T, Dims> point(static_cast<T>(2));
  mathkata::Vector<T, Dims> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);

  mathkata::Line<T, Dims> line(point, direction);

  // At t=0, should be at point.
  mathkata::Vector<T, Dims> p0 = line.pointAt(static_cast<T>(0));
  for (int i = 0; i < Dims; ++i) {
    EXPECT_NEAR(p0[i], point[i], precision);
  }

  // At t=5, should be point + direction * 5.
  mathkata::Vector<T, Dims> p5 = line.pointAt(static_cast<T>(5));
  EXPECT_NEAR(p5[0], static_cast<T>(7), precision);
  for (int i = 1; i < Dims; ++i) {
    EXPECT_NEAR(p5[i], static_cast<T>(2), precision);
  }
}
TEST_ALL_F(LinePointAt)

template <class T, int Dims>
void LineEquality_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, Dims> point(static_cast<T>(1));
  mathkata::Vector<T, Dims> direction(static_cast<T>(0));
  direction[0] = static_cast<T>(1);

  mathkata::Line<T, Dims> l1(point, direction);
  mathkata::Line<T, Dims> l2(point, direction);
  EXPECT_TRUE(l1 == l2);
  EXPECT_FALSE(l1 != l2);

  mathkata::Vector<T, Dims> other_point(static_cast<T>(5));
  mathkata::Line<T, Dims> l3(other_point, direction);
  EXPECT_TRUE(l1 != l3);
  EXPECT_FALSE(l1 == l3);
}
TEST_ALL_F(LineEquality)

// --- LineSegment tests ---

template <class T, int Dims>
void LineSegmentConstruction_Test(T precision) {
  (void)precision;
  // Inside the template so it runs for every T and Dims the suite
  // instantiates.
  static_assert(
      !std::is_default_constructible_v<mathkata::LineSegment<T, Dims>>);
  auto s1 = mathkata::LineSegment<T, Dims>::uninitialized();
  s1.start = mathkata::Vector<T, Dims>(static_cast<T>(4));
  EXPECT_EQ(s1.start[0], static_cast<T>(4));

  // Construction from two points.
  mathkata::Vector<T, Dims> start(static_cast<T>(1));
  mathkata::Vector<T, Dims> end(static_cast<T>(3));

  mathkata::LineSegment<T, Dims> s2(start, end);
  EXPECT_EQ(s2.start, start);
  EXPECT_EQ(s2.end, end);
}
TEST_ALL_F(LineSegmentConstruction)

template <class T, int Dims>
void LineSegmentCenter_Test(T precision) {
  mathkata::Vector<T, Dims> start(static_cast<T>(0));
  mathkata::Vector<T, Dims> end(static_cast<T>(4));

  mathkata::LineSegment<T, Dims> segment(start, end);
  mathkata::Vector<T, Dims> center = segment.center();

  for (int i = 0; i < Dims; ++i) {
    EXPECT_NEAR(center[i], static_cast<T>(2), precision);
  }
}
TEST_ALL_F(LineSegmentCenter)

template <class T, int Dims>
void LineSegmentLength_Test(T precision) {
  mathkata::Vector<T, Dims> start(static_cast<T>(0));
  mathkata::Vector<T, Dims> end(static_cast<T>(0));
  end[0] = static_cast<T>(5);

  mathkata::LineSegment<T, Dims> segment(start, end);
  EXPECT_NEAR(segment.length(), static_cast<T>(5), precision);
}
TEST_ALL_F(LineSegmentLength)

template <class T, int Dims>
void LineSegmentLengthSquared_Test(T precision) {
  mathkata::Vector<T, Dims> start(static_cast<T>(0));
  mathkata::Vector<T, Dims> end(static_cast<T>(0));
  end[0] = static_cast<T>(3);
  end[1] = static_cast<T>(4);

  mathkata::LineSegment<T, Dims> segment(start, end);
  EXPECT_NEAR(segment.lengthSquared(), static_cast<T>(25), precision);
}
TEST_ALL_F(LineSegmentLengthSquared)

template <class T, int Dims>
void LineSegmentDirection_Test(T precision) {
  mathkata::Vector<T, Dims> start(static_cast<T>(0));
  mathkata::Vector<T, Dims> end(static_cast<T>(0));
  end[0] = static_cast<T>(10);

  mathkata::LineSegment<T, Dims> segment(start, end);
  mathkata::Vector<T, Dims> dir = segment.direction();

  EXPECT_NEAR(dir[0], static_cast<T>(1), precision);
  for (int i = 1; i < Dims; ++i) {
    EXPECT_NEAR(dir[i], static_cast<T>(0), precision);
  }

  // direction should be unit length.
  EXPECT_NEAR(dir.length(), static_cast<T>(1), precision);
}
TEST_ALL_F(LineSegmentDirection)

template <class T, int Dims>
void LineSegmentClosestPoint_Test(T precision) {
  // Segment along the x-axis from (0,...,0) to (10,0,...,0).
  mathkata::Vector<T, Dims> start(static_cast<T>(0));
  mathkata::Vector<T, Dims> end(static_cast<T>(0));
  end[0] = static_cast<T>(10);

  mathkata::LineSegment<T, Dims> segment(start, end);

  // Point projected onto the middle of the segment.
  mathkata::Vector<T, Dims> mid_point(static_cast<T>(0));
  mid_point[0] = static_cast<T>(5);
  if (Dims > 1) {
    mid_point[1] = static_cast<T>(3);
  }
  mathkata::Vector<T, Dims> closest = segment.closestPoint(mid_point);
  EXPECT_NEAR(closest[0], static_cast<T>(5), precision);
  for (int i = 1; i < Dims; ++i) {
    EXPECT_NEAR(closest[i], static_cast<T>(0), precision);
  }

  // Point before the start - should clamp to start.
  mathkata::Vector<T, Dims> before_start(static_cast<T>(0));
  before_start[0] = static_cast<T>(-5);
  closest = segment.closestPoint(before_start);
  for (int i = 0; i < Dims; ++i) {
    EXPECT_NEAR(closest[i], start[i], precision);
  }

  // Point beyond the end - should clamp to end.
  mathkata::Vector<T, Dims> past_end(static_cast<T>(0));
  past_end[0] = static_cast<T>(15);
  closest = segment.closestPoint(past_end);
  for (int i = 0; i < Dims; ++i) {
    EXPECT_NEAR(closest[i], end[i], precision);
  }

  // Zero-length segment - should return start.
  mathkata::LineSegment<T, Dims> zero_segment(start, start);
  mathkata::Vector<T, Dims> any_point(static_cast<T>(5));
  closest = zero_segment.closestPoint(any_point);
  for (int i = 0; i < Dims; ++i) {
    EXPECT_NEAR(closest[i], start[i], precision);
  }
}
TEST_ALL_F(LineSegmentClosestPoint)

template <class T, int Dims>
void LineSegmentEquality_Test(T precision) {
  (void)precision;
  mathkata::Vector<T, Dims> start(static_cast<T>(1));
  mathkata::Vector<T, Dims> end(static_cast<T>(5));

  mathkata::LineSegment<T, Dims> s1(start, end);
  mathkata::LineSegment<T, Dims> s2(start, end);
  EXPECT_TRUE(s1 == s2);
  EXPECT_FALSE(s1 != s2);

  mathkata::Vector<T, Dims> other_end(static_cast<T>(10));
  mathkata::LineSegment<T, Dims> s3(start, other_end);
  EXPECT_TRUE(s1 != s3);
  EXPECT_FALSE(s1 == s3);
}
TEST_ALL_F(LineSegmentEquality)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

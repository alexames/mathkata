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
#include "mathkata/rect.h"

#include "gtest/gtest.h"
#include "precision.h"

class RectTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// This will automatically generate tests for float and double.
#define TEST_ALL_F(MY_TEST)                   \
  TEST_F(RectTests, MY_TEST##_float) {        \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
  }                                           \
  TEST_F(RectTests, MY_TEST##_double) {       \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

// Tests for integer types that don't need a precision parameter.
#define TEST_ALL_INT(MY_TEST) \
  TEST_F(RectTests, MY_TEST##_int) { MY_TEST##_Test<int>(); }

// --- Construction and Equality ---

template <class T>
void Construction_Test(T precision) {
  (void)precision;
  // Default construction should be zero.
  mathkata::Rect<T> r;
  EXPECT_EQ(r.pos.x, static_cast<T>(0));
  EXPECT_EQ(r.pos.y, static_cast<T>(0));
  EXPECT_EQ(r.size.x, static_cast<T>(0));
  EXPECT_EQ(r.size.y, static_cast<T>(0));

  // Construction from values.
  mathkata::Rect<T> r2(static_cast<T>(1), static_cast<T>(2), static_cast<T>(3),
                       static_cast<T>(4));
  EXPECT_EQ(r2.pos.x, static_cast<T>(1));
  EXPECT_EQ(r2.pos.y, static_cast<T>(2));
  EXPECT_EQ(r2.size.x, static_cast<T>(3));
  EXPECT_EQ(r2.size.y, static_cast<T>(4));

  // Construction from vectors.
  mathkata::Vector<T, 2> p(static_cast<T>(5), static_cast<T>(6));
  mathkata::Vector<T, 2> s(static_cast<T>(7), static_cast<T>(8));
  mathkata::Rect<T> r3(p, s);
  EXPECT_EQ(r3.pos, p);
  EXPECT_EQ(r3.size, s);

  // Equality.
  EXPECT_EQ(r3, mathkata::Rect<T>(p, s));
  EXPECT_NE(r2, r3);
}
TEST_ALL_F(Construction)

// --- center ---

template <class T>
void center_Test(T precision) {
  mathkata::Rect<T> r(static_cast<T>(2), static_cast<T>(4), static_cast<T>(10),
                      static_cast<T>(6));
  mathkata::Vector<T, 2> center = r.center();
  EXPECT_NEAR(center.x, static_cast<T>(7), precision);
  EXPECT_NEAR(center.y, static_cast<T>(7), precision);

  // Rect at origin.
  mathkata::Rect<T> r2(static_cast<T>(0), static_cast<T>(0), static_cast<T>(4),
                       static_cast<T>(8));
  mathkata::Vector<T, 2> center2 = r2.center();
  EXPECT_NEAR(center2.x, static_cast<T>(2), precision);
  EXPECT_NEAR(center2.y, static_cast<T>(4), precision);
}
TEST_ALL_F(center)

// --- min and max ---

template <class T>
void MinMax_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> r(static_cast<T>(3), static_cast<T>(5), static_cast<T>(10),
                      static_cast<T>(20));
  mathkata::Vector<T, 2> min_corner = r.min();
  mathkata::Vector<T, 2> max_corner = r.max();

  EXPECT_EQ(min_corner.x, static_cast<T>(3));
  EXPECT_EQ(min_corner.y, static_cast<T>(5));
  EXPECT_EQ(max_corner.x, static_cast<T>(13));
  EXPECT_EQ(max_corner.y, static_cast<T>(25));
}
TEST_ALL_F(MinMax)

// --- area ---

template <class T>
void area_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> r(static_cast<T>(0), static_cast<T>(0), static_cast<T>(5),
                      static_cast<T>(3));
  EXPECT_EQ(r.area(), static_cast<T>(15));

  // Zero-size rect.
  mathkata::Rect<T> r2;
  EXPECT_EQ(r2.area(), static_cast<T>(0));

  // Single-dimension zero.
  mathkata::Rect<T> r3(static_cast<T>(0), static_cast<T>(0), static_cast<T>(5),
                       static_cast<T>(0));
  EXPECT_EQ(r3.area(), static_cast<T>(0));
}
TEST_ALL_F(area)

// --- contains (point) ---

template <class T>
void containsPoint_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> r(static_cast<T>(0), static_cast<T>(0), static_cast<T>(10),
                      static_cast<T>(10));

  // Inside.
  EXPECT_TRUE(
      r.contains(mathkata::Vector<T, 2>(static_cast<T>(5), static_cast<T>(5))));

  // On boundary (corners).
  EXPECT_TRUE(
      r.contains(mathkata::Vector<T, 2>(static_cast<T>(0), static_cast<T>(0))));
  EXPECT_TRUE(r.contains(
      mathkata::Vector<T, 2>(static_cast<T>(10), static_cast<T>(10))));

  // On boundary (edges).
  EXPECT_TRUE(
      r.contains(mathkata::Vector<T, 2>(static_cast<T>(5), static_cast<T>(0))));
  EXPECT_TRUE(
      r.contains(mathkata::Vector<T, 2>(static_cast<T>(0), static_cast<T>(5))));

  // Outside.
  EXPECT_FALSE(r.contains(
      mathkata::Vector<T, 2>(static_cast<T>(11), static_cast<T>(5))));
  EXPECT_FALSE(r.contains(
      mathkata::Vector<T, 2>(static_cast<T>(5), static_cast<T>(11))));
  EXPECT_FALSE(r.contains(
      mathkata::Vector<T, 2>(static_cast<T>(-1), static_cast<T>(5))));
  EXPECT_FALSE(r.contains(
      mathkata::Vector<T, 2>(static_cast<T>(5), static_cast<T>(-1))));
}
TEST_ALL_F(containsPoint)

// --- contains (rect) ---

template <class T>
void ContainsRect_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> outer(static_cast<T>(0), static_cast<T>(0),
                          static_cast<T>(10), static_cast<T>(10));

  // Inner rect fully contained.
  mathkata::Rect<T> inner(static_cast<T>(2), static_cast<T>(2),
                          static_cast<T>(3), static_cast<T>(3));
  EXPECT_TRUE(outer.contains(inner));

  // Same rect.
  EXPECT_TRUE(outer.contains(outer));

  // Partially overlapping.
  mathkata::Rect<T> partial(static_cast<T>(5), static_cast<T>(5),
                            static_cast<T>(10), static_cast<T>(10));
  EXPECT_FALSE(outer.contains(partial));

  // Completely outside.
  mathkata::Rect<T> outside(static_cast<T>(20), static_cast<T>(20),
                            static_cast<T>(5), static_cast<T>(5));
  EXPECT_FALSE(outer.contains(outside));
}
TEST_ALL_F(ContainsRect)

// --- intersects ---

template <class T>
void intersects_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> r(static_cast<T>(0), static_cast<T>(0), static_cast<T>(10),
                      static_cast<T>(10));

  // Overlapping.
  mathkata::Rect<T> overlap(static_cast<T>(5), static_cast<T>(5),
                            static_cast<T>(10), static_cast<T>(10));
  EXPECT_TRUE(r.intersects(overlap));
  EXPECT_TRUE(overlap.intersects(r));

  // Contained.
  mathkata::Rect<T> inner(static_cast<T>(2), static_cast<T>(2),
                          static_cast<T>(3), static_cast<T>(3));
  EXPECT_TRUE(r.intersects(inner));
  EXPECT_TRUE(inner.intersects(r));

  // Not overlapping (separated horizontally).
  mathkata::Rect<T> right(static_cast<T>(20), static_cast<T>(0),
                          static_cast<T>(5), static_cast<T>(5));
  EXPECT_FALSE(r.intersects(right));
  EXPECT_FALSE(right.intersects(r));

  // Touching edges (not overlapping with strict inequality).
  mathkata::Rect<T> touching(static_cast<T>(10), static_cast<T>(0),
                             static_cast<T>(5), static_cast<T>(5));
  EXPECT_FALSE(r.intersects(touching));
  EXPECT_FALSE(touching.intersects(r));

  // Not overlapping (separated vertically).
  mathkata::Rect<T> below(static_cast<T>(0), static_cast<T>(20),
                          static_cast<T>(5), static_cast<T>(5));
  EXPECT_FALSE(r.intersects(below));
  EXPECT_FALSE(below.intersects(r));
}
TEST_ALL_F(intersects)

// --- intersection ---

template <class T>
void intersection_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> r1(static_cast<T>(0), static_cast<T>(0), static_cast<T>(10),
                       static_cast<T>(10));
  mathkata::Rect<T> r2(static_cast<T>(5), static_cast<T>(5), static_cast<T>(10),
                       static_cast<T>(10));

  mathkata::Rect<T> result = r1.intersection(r2);
  EXPECT_EQ(result.pos.x, static_cast<T>(5));
  EXPECT_EQ(result.pos.y, static_cast<T>(5));
  EXPECT_EQ(result.size.x, static_cast<T>(5));
  EXPECT_EQ(result.size.y, static_cast<T>(5));

  // No overlap returns zero rect.
  mathkata::Rect<T> r3(static_cast<T>(20), static_cast<T>(20),
                       static_cast<T>(5), static_cast<T>(5));
  mathkata::Rect<T> empty_result = r1.intersection(r3);
  EXPECT_EQ(empty_result, mathkata::Rect<T>());

  // Contained rect intersection returns inner rect.
  mathkata::Rect<T> inner(static_cast<T>(2), static_cast<T>(3),
                          static_cast<T>(4), static_cast<T>(5));
  mathkata::Rect<T> inner_result = r1.intersection(inner);
  EXPECT_EQ(inner_result, inner);

  // Self intersection returns self.
  mathkata::Rect<T> self_result = r1.intersection(r1);
  EXPECT_EQ(self_result, r1);
}
TEST_ALL_F(intersection)

// --- merge ---

template <class T>
void merge_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> r1(static_cast<T>(0), static_cast<T>(0), static_cast<T>(5),
                       static_cast<T>(5));
  mathkata::Rect<T> r2(static_cast<T>(10), static_cast<T>(10),
                       static_cast<T>(5), static_cast<T>(5));

  mathkata::Rect<T> result = r1.merge(r2);
  EXPECT_EQ(result.pos.x, static_cast<T>(0));
  EXPECT_EQ(result.pos.y, static_cast<T>(0));
  EXPECT_EQ(result.size.x, static_cast<T>(15));
  EXPECT_EQ(result.size.y, static_cast<T>(15));

  // merge with overlapping rect.
  mathkata::Rect<T> r3(static_cast<T>(3), static_cast<T>(3), static_cast<T>(5),
                       static_cast<T>(5));
  mathkata::Rect<T> overlap_result = r1.merge(r3);
  EXPECT_EQ(overlap_result.pos.x, static_cast<T>(0));
  EXPECT_EQ(overlap_result.pos.y, static_cast<T>(0));
  EXPECT_EQ(overlap_result.size.x, static_cast<T>(8));
  EXPECT_EQ(overlap_result.size.y, static_cast<T>(8));

  // merge with self returns self.
  mathkata::Rect<T> self_result = r1.merge(r1);
  EXPECT_EQ(self_result, r1);

  // merge with contained rect returns outer rect.
  mathkata::Rect<T> outer(static_cast<T>(0), static_cast<T>(0),
                          static_cast<T>(20), static_cast<T>(20));
  mathkata::Rect<T> contained_result = outer.merge(r1);
  EXPECT_EQ(contained_result, outer);
}
TEST_ALL_F(merge)

// --- expand (point) ---

template <class T>
void ExpandPoint_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> r(static_cast<T>(2), static_cast<T>(2), static_cast<T>(6),
                      static_cast<T>(6));

  // Point already inside - no change.
  mathkata::Rect<T> same =
      r.expand(mathkata::Vector<T, 2>(static_cast<T>(5), static_cast<T>(5)));
  EXPECT_EQ(same, r);

  // Point outside to the right and below.
  mathkata::Rect<T> expanded =
      r.expand(mathkata::Vector<T, 2>(static_cast<T>(20), static_cast<T>(20)));
  EXPECT_EQ(expanded.pos.x, static_cast<T>(2));
  EXPECT_EQ(expanded.pos.y, static_cast<T>(2));
  EXPECT_EQ(expanded.size.x, static_cast<T>(18));
  EXPECT_EQ(expanded.size.y, static_cast<T>(18));

  // Point outside to the left and above.
  mathkata::Rect<T> expanded2 =
      r.expand(mathkata::Vector<T, 2>(static_cast<T>(-5), static_cast<T>(-3)));
  EXPECT_EQ(expanded2.pos.x, static_cast<T>(-5));
  EXPECT_EQ(expanded2.pos.y, static_cast<T>(-3));
  EXPECT_EQ(expanded2.size.x, static_cast<T>(13));
  EXPECT_EQ(expanded2.size.y, static_cast<T>(11));

  // Point on boundary - no change.
  mathkata::Rect<T> boundary =
      r.expand(mathkata::Vector<T, 2>(static_cast<T>(8), static_cast<T>(8)));
  EXPECT_EQ(boundary, r);
}
TEST_ALL_F(ExpandPoint)

// --- expand (amount) ---

template <class T>
void ExpandAmount_Test(T precision) {
  (void)precision;
  mathkata::Rect<T> r(static_cast<T>(5), static_cast<T>(5), static_cast<T>(10),
                      static_cast<T>(10));

  mathkata::Rect<T> expanded = r.expand(static_cast<T>(2));
  EXPECT_EQ(expanded.pos.x, static_cast<T>(3));
  EXPECT_EQ(expanded.pos.y, static_cast<T>(3));
  EXPECT_EQ(expanded.size.x, static_cast<T>(14));
  EXPECT_EQ(expanded.size.y, static_cast<T>(14));

  // expand by zero - no change.
  mathkata::Rect<T> same = r.expand(static_cast<T>(0));
  EXPECT_EQ(same, r);

  // Shrink (negative amount).
  mathkata::Rect<T> shrunk = r.expand(static_cast<T>(-1));
  EXPECT_EQ(shrunk.pos.x, static_cast<T>(6));
  EXPECT_EQ(shrunk.pos.y, static_cast<T>(6));
  EXPECT_EQ(shrunk.size.x, static_cast<T>(8));
  EXPECT_EQ(shrunk.size.y, static_cast<T>(8));
}
TEST_ALL_F(ExpandAmount)

// --- Integer-specific tests ---

template <class T>
void IntegerArea_Test() {
  mathkata::Rect<T> r(0, 0, 7, 3);
  EXPECT_EQ(r.area(), 21);
}
TEST_ALL_INT(IntegerArea)

template <class T>
void IntegerCenter_Test() {
  // Integer division truncates: (0 + 10/2, 0 + 10/2) = (5, 5).
  mathkata::Rect<T> r(0, 0, 10, 10);
  mathkata::Vector<T, 2> center = r.center();
  EXPECT_EQ(center.x, 5);
  EXPECT_EQ(center.y, 5);
}
TEST_ALL_INT(IntegerCenter)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

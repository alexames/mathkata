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
#include "mathkata/affine_transform_2d.h"

#include <cmath>
#include <numbers>

#include "gtest/gtest.h"
#include "precision.h"

namespace {

class AffineTransform2DTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

#define TEST_ALL_F(MY_TEST)                   \
  TEST_F(AffineTransform2DTests, MY_TEST) {   \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

// Helper: check if two 2D vectors are approximately equal.
template <class T>
bool NearVector2(const mathkata::Vector<T, 2>& v1,
                 const mathkata::Vector<T, 2>& v2, T abs_error) {
  return std::fabs(static_cast<double>(v1.x - v2.x))
             <= static_cast<double>(abs_error)
         && std::fabs(static_cast<double>(v1.y - v2.y))
                <= static_cast<double>(abs_error);
}

// Helper: check if two transforms are approximately equal.
template <class T>
bool NearTransform(const mathkata::AffineTransform2D<T>& t1,
                   const mathkata::AffineTransform2D<T>& t2, T abs_error) {
  const T* a = t1.GetMatrix();
  const T* b = t2.GetMatrix();
  for (int i = 0; i < 16; ++i) {
    if (std::fabs(static_cast<double>(a[i] - b[i]))
        > static_cast<double>(abs_error)) {
      return false;
    }
  }
  return true;
}

// --- Default Construction ---

template <class T>
void DefaultConstruction_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  const T* m = t.GetMatrix();

  // Should be a 4x4 identity matrix.
  for (int col = 0; col < 4; ++col) {
    for (int row = 0; row < 4; ++row) {
      T expected = (row == col) ? T(1) : T(0);
      EXPECT_NEAR(static_cast<double>(m[col * 4 + row]),
                  static_cast<double>(expected), static_cast<double>(precision))
          << "Element [" << row << "][" << col << "]";
    }
  }
}
TEST_ALL_F(DefaultConstruction)

// --- 9-Element Construction ---

template <class T>
void NineElementConstruction_Test(T precision) {
  mathkata::AffineTransform2D<T> t(T(1), T(2), T(3), T(4), T(5), T(6), T(0),
                                   T(0), T(1));
  const T* m = t.GetMatrix();

  // Column 0: [a00, a10, 0, a20]
  EXPECT_NEAR(m[0], T(1), precision);  // a00
  EXPECT_NEAR(m[1], T(4), precision);  // a10
  EXPECT_NEAR(m[2], T(0), precision);
  EXPECT_NEAR(m[3], T(0), precision);  // a20

  // Column 1: [a01, a11, 0, a21]
  EXPECT_NEAR(m[4], T(2), precision);  // a01
  EXPECT_NEAR(m[5], T(5), precision);  // a11
  EXPECT_NEAR(m[6], T(0), precision);
  EXPECT_NEAR(m[7], T(0), precision);  // a21

  // Column 2: [0, 0, 1, 0]
  EXPECT_NEAR(m[8], T(0), precision);
  EXPECT_NEAR(m[9], T(0), precision);
  EXPECT_NEAR(m[10], T(1), precision);
  EXPECT_NEAR(m[11], T(0), precision);

  // Column 3: [a02, a12, 0, a22]
  EXPECT_NEAR(m[12], T(3), precision);  // a02
  EXPECT_NEAR(m[13], T(6), precision);  // a12
  EXPECT_NEAR(m[14], T(0), precision);
  EXPECT_NEAR(m[15], T(1), precision);  // a22
}
TEST_ALL_F(NineElementConstruction)

// --- Identity Transform Point ---

template <class T>
void IdentityTransformPoint_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  mathkata::Vector<T, 2> point(T(7), T(-3));
  mathkata::Vector<T, 2> result = t.TransformPoint(point);
  EXPECT_TRUE(NearVector2(result, point, precision))
      << "Identity should not change a point.";
}
TEST_ALL_F(IdentityTransformPoint)

// --- Translation ---

template <class T>
void TranslatePoint_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  t.Translate(T(10), T(20));

  mathkata::Vector<T, 2> point(T(5), T(3));
  mathkata::Vector<T, 2> result = t.TransformPoint(point);
  mathkata::Vector<T, 2> expected(T(15), T(23));
  EXPECT_TRUE(NearVector2(result, expected, precision))
      << "Translation should offset the point.";
}
TEST_ALL_F(TranslatePoint)

// --- Rotation ---

template <class T>
void Rotate90_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  t.Rotate(T(90));

  // Rotating (1, 0) by 90 degrees CCW should give (0, 1).
  mathkata::Vector<T, 2> point(T(1), T(0));
  mathkata::Vector<T, 2> result = t.TransformPoint(point);
  T relaxed = precision * T(10);
  EXPECT_TRUE(NearVector2(result, mathkata::Vector<T, 2>(T(0), T(1)), relaxed))
      << "90-degree rotation of (1,0) should yield (0,1).";
}
TEST_ALL_F(Rotate90)

template <class T>
void Rotate180_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  t.Rotate(T(180));

  mathkata::Vector<T, 2> point(T(1), T(0));
  mathkata::Vector<T, 2> result = t.TransformPoint(point);
  T relaxed = precision * T(10);
  EXPECT_TRUE(NearVector2(result, mathkata::Vector<T, 2>(T(-1), T(0)), relaxed))
      << "180-degree rotation of (1,0) should yield (-1,0).";
}
TEST_ALL_F(Rotate180)

// --- Rotation Around Center ---

template <class T>
void RotateAroundCenter_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  mathkata::Vector<T, 2> center(T(5), T(5));
  t.Rotate(T(90), center);

  // Rotating (6, 5) by 90 degrees CCW around (5, 5):
  // Relative to center: (1, 0) -> rotate -> (0, 1) -> add center -> (5, 6).
  mathkata::Vector<T, 2> point(T(6), T(5));
  mathkata::Vector<T, 2> result = t.TransformPoint(point);
  T relaxed = precision * T(100);
  EXPECT_TRUE(NearVector2(result, mathkata::Vector<T, 2>(T(5), T(6)), relaxed))
      << "Rotation around center (5,5) of (6,5) by 90 should yield (5,6).";
}
TEST_ALL_F(RotateAroundCenter)

// --- Scale ---

template <class T>
void ScalePoint_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  t.Scale(T(2), T(3));

  mathkata::Vector<T, 2> point(T(4), T(5));
  mathkata::Vector<T, 2> result = t.TransformPoint(point);
  EXPECT_TRUE(
      NearVector2(result, mathkata::Vector<T, 2>(T(8), T(15)), precision))
      << "Scale(2,3) should double x and triple y.";
}
TEST_ALL_F(ScalePoint)

// --- Scale Around Center ---

template <class T>
void ScaleAroundCenter_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  mathkata::Vector<T, 2> center(T(10), T(10));
  t.Scale(T(2), T(2), center);

  // Scaling (15, 10) by 2x around center (10, 10):
  // Relative to center: (5, 0) -> scale -> (10, 0) -> add center -> (20, 10).
  mathkata::Vector<T, 2> point(T(15), T(10));
  mathkata::Vector<T, 2> result = t.TransformPoint(point);
  T relaxed = precision * T(10);
  EXPECT_TRUE(
      NearVector2(result, mathkata::Vector<T, 2>(T(20), T(10)), relaxed))
      << "Scale(2,2) around (10,10) of (15,10) should yield (20,10).";
}
TEST_ALL_F(ScaleAroundCenter)

// --- Chaining ---

template <class T>
void Chaining_Test(T precision) {
  // Translate then scale: scale is applied to the translated coordinate system.
  mathkata::AffineTransform2D<T> t;
  t.Translate(T(10), T(20)).Scale(T(2), T(2));

  // Point (1, 1):
  // After translate: coordinate system shifted by (10, 20).
  // After scale: coordinate system scaled by 2.
  // Transform (1,1): scale first (2,2), then translate (10,20) = (12, 22).
  // Wait — the combine order is: this = this * other.
  // So Translate then Scale means: matrix = Translate * Scale.
  // Applying to point: Translate * Scale * point = Translate * (2, 2) = (12,
  // 22).
  mathkata::Vector<T, 2> point(T(1), T(1));
  mathkata::Vector<T, 2> result = t.TransformPoint(point);
  EXPECT_TRUE(
      NearVector2(result, mathkata::Vector<T, 2>(T(12), T(22)), precision))
      << "Chained translate(10,20).scale(2,2) on (1,1) should yield (12,22).";
}
TEST_ALL_F(Chaining)

// --- TransformRect ---

template <class T>
void TransformRectIdentity_Test(T precision) {
  (void)precision;
  mathkata::AffineTransform2D<T> t;
  mathkata::Rect<T> rect(T(1), T(2), T(3), T(4));
  mathkata::Rect<T> result = t.TransformRect(rect);
  EXPECT_EQ(result.pos.x, T(1));
  EXPECT_EQ(result.pos.y, T(2));
  EXPECT_EQ(result.size.x, T(3));
  EXPECT_EQ(result.size.y, T(4));
}
TEST_ALL_F(TransformRectIdentity)

template <class T>
void TransformRectTranslation_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  t.Translate(T(10), T(20));
  mathkata::Rect<T> rect(T(0), T(0), T(5), T(5));
  mathkata::Rect<T> result = t.TransformRect(rect);
  T relaxed = precision * T(10);
  EXPECT_NEAR(result.pos.x, T(10), relaxed);
  EXPECT_NEAR(result.pos.y, T(20), relaxed);
  EXPECT_NEAR(result.size.x, T(5), relaxed);
  EXPECT_NEAR(result.size.y, T(5), relaxed);
}
TEST_ALL_F(TransformRectTranslation)

template <class T>
void TransformRectRotation_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  t.Rotate(T(90));

  // A unit square at origin rotated 90 degrees CCW.
  // Corners (0,0), (1,0), (0,1), (1,1) become (0,0), (0,1), (-1,0), (-1,1).
  // AABB: pos=(-1,0), size=(1,1).
  mathkata::Rect<T> rect(T(0), T(0), T(1), T(1));
  mathkata::Rect<T> result = t.TransformRect(rect);
  T relaxed = precision * T(100);
  EXPECT_NEAR(result.pos.x, T(-1), relaxed);
  EXPECT_NEAR(result.pos.y, T(0), relaxed);
  EXPECT_NEAR(result.size.x, T(1), relaxed);
  EXPECT_NEAR(result.size.y, T(1), relaxed);
}
TEST_ALL_F(TransformRectRotation)

// --- Inverse ---

template <class T>
void InverseIdentity_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  mathkata::AffineTransform2D<T> inv = t.GetInverse();
  EXPECT_TRUE(NearTransform(inv, t, precision))
      << "Inverse of identity should be identity.";
}
TEST_ALL_F(InverseIdentity)

template <class T>
void InversePointRoundTrip_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  t.Translate(T(5), T(-3));
  t.Rotate(T(45));
  t.Scale(T(2), T(3));

  mathkata::AffineTransform2D<T> inv = t.GetInverse();

  mathkata::Vector<T, 2> original(T(7), T(-2));
  mathkata::Vector<T, 2> transformed = t.TransformPoint(original);
  mathkata::Vector<T, 2> recovered = inv.TransformPoint(transformed);

  T relaxed = precision * T(1000);
  EXPECT_TRUE(NearVector2(recovered, original, relaxed))
      << "Inverse should recover the original point.";
}
TEST_ALL_F(InversePointRoundTrip)

template <class T>
void InverseComposition_Test(T precision) {
  mathkata::AffineTransform2D<T> t;
  t.Translate(T(10), T(20));
  t.Rotate(T(30));

  mathkata::AffineTransform2D<T> inv = t.GetInverse();
  mathkata::AffineTransform2D<T> composed = t * inv;

  T relaxed = precision * T(1000);
  EXPECT_TRUE(NearTransform(
      composed, mathkata::AffineTransform2D<T>::Identity(), relaxed))
      << "t * t.GetInverse() should be identity.";
}
TEST_ALL_F(InverseComposition)

// --- Combine matches operator* ---

template <class T>
void CombineMatchesOperator_Test(T precision) {
  mathkata::AffineTransform2D<T> t1;
  t1.Translate(T(5), T(10));
  t1.Rotate(T(30));

  mathkata::AffineTransform2D<T> t2;
  t2.Scale(T(2), T(3));

  // operator*
  mathkata::AffineTransform2D<T> product = t1 * t2;

  // Combine
  mathkata::AffineTransform2D<T> combined = t1;
  combined.Combine(t2);

  EXPECT_TRUE(NearTransform(product, combined, precision))
      << "operator* and Combine should produce the same result.";
}
TEST_ALL_F(CombineMatchesOperator)

// --- Equality ---

template <class T>
void Equality_Test(T precision) {
  (void)precision;
  mathkata::AffineTransform2D<T> t1;
  t1.Translate(T(5), T(10));

  mathkata::AffineTransform2D<T> t2;
  t2.Translate(T(5), T(10));

  mathkata::AffineTransform2D<T> t3;

  EXPECT_TRUE(t1 == t2) << "Identical transforms should be equal.";
  EXPECT_FALSE(t1 == t3) << "Different transforms should not be equal.";
  EXPECT_FALSE(t1 != t2) << "Identical transforms should not be not-equal.";
  EXPECT_TRUE(t1 != t3) << "Different transforms should be not-equal.";
}
TEST_ALL_F(Equality)

// --- Static Identity ---

template <class T>
void StaticIdentity_Test(T precision) {
  mathkata::AffineTransform2D<T> id =
      mathkata::AffineTransform2D<T>::Identity();
  mathkata::AffineTransform2D<T> default_constructed;
  EXPECT_TRUE(NearTransform(id, default_constructed, precision))
      << "Identity() should match default construction.";
}
TEST_ALL_F(StaticIdentity)

}  // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

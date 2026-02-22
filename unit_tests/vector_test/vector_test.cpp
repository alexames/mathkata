/*
 * Copyright 2014 Google Inc. All rights reserved.
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
#include "mathfu/vector.h"

#include <climits>
#include <cstdint>
#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "mathfu/constants.h"
#include "mathfu/io.h"
#include "mathfu/utilities.h"
#include "precision.h"

class VectorTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// This will automatically generate tests for each template parameter.
#define TEST_ALL_F(MY_TEST)                      \
  TEST_F(VectorTests, MY_TEST##_float_2) {       \
    MY_TEST##_Test<float, 2>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(VectorTests, MY_TEST##_double_2) {      \
    MY_TEST##_Test<double, 2>(DOUBLE_PRECISION); \
  }                                              \
  TEST_F(VectorTests, MY_TEST##_float_3) {       \
    MY_TEST##_Test<float, 3>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(VectorTests, MY_TEST##_double_3) {      \
    MY_TEST##_Test<double, 3>(DOUBLE_PRECISION); \
  }                                              \
  TEST_F(VectorTests, MY_TEST##_float_4) {       \
    MY_TEST##_Test<float, 4>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(VectorTests, MY_TEST##_double_4) {      \
    MY_TEST##_Test<double, 4>(DOUBLE_PRECISION); \
  }                                              \
  TEST_F(VectorTests, MY_TEST##_float_5) {       \
    MY_TEST##_Test<float, 5>(FLOAT_PRECISION);   \
  }                                              \
  TEST_F(VectorTests, MY_TEST##_double_5) {      \
    MY_TEST##_Test<double, 5>(DOUBLE_PRECISION); \
  }

#define TEST_ALL_INTS_F(MY_INT_TEST)                                      \
  TEST_F(VectorTests, MY_INT_TEST##_2) { MY_INT_TEST##_Test<int, 2>(0); } \
  TEST_F(VectorTests, MY_INT_TEST##_3) { MY_INT_TEST##_Test<int, 3>(0); } \
  TEST_F(VectorTests, MY_INT_TEST##_4) { MY_INT_TEST##_Test<int, 4>(0); } \
  TEST_F(VectorTests, MY_INT_TEST##_5) { MY_INT_TEST##_Test<int, 5>(0); }

// This will automatically generate tests for each scalar template parameter.
#define TEST_SCALAR_F(MY_TEST)                \
  TEST_F(VectorTests, MY_TEST##_float) {      \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
  }                                           \
  TEST_F(VectorTests, MY_TEST##_double) {     \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }

// This will automatically generate tests for each scalar template parameter.
#define TEST_SCALAR_AND_INT_F(MY_TEST)        \
  TEST_F(VectorTests, MY_TEST##_float) {      \
    MY_TEST##_Test<float>(FLOAT_PRECISION);   \
  }                                           \
  TEST_F(VectorTests, MY_TEST##_double) {     \
    MY_TEST##_Test<double>(DOUBLE_PRECISION); \
  }                                           \
  TEST_F(VectorTests, MY_TEST##_int) { MY_TEST##_Test<int>(0); }

// Tests float, double, and integer constants in one line.
#define VECTOR_TEST_CONSTANT_EQ(kConst, index, value)                       \
  EXPECT_FLOAT_EQ(mathfu::kConst##f[(index)], static_cast<float>(value));   \
  EXPECT_DOUBLE_EQ(mathfu::kConst##d[(index)], static_cast<double>(value)); \
  EXPECT_EQ(mathfu::kConst##i[(index)], static_cast<int>(value))

// A predicate-formatter for asserting that compares 2 vectors are equal.
template <class T, int d>
::testing::AssertionResult AssertVectorEqual(const char* m_expr,
                                             const char* n_expr,
                                             const mathfu::Vector<T, d>& v1,
                                             const mathfu::Vector<T, d>& v2) {
  for (int32_t i = 0; i < d; ++i) {
    if (v1[i] != v2[i]) {
      return ::testing::AssertionFailure() << m_expr << v1 << " and " << n_expr
                                           << v2 << " are not same value.";
    }
  }

  return ::testing::AssertionSuccess();
}

// A predicate-formatter for asserting that compares 2 vectors are nealy equal
// with an error of abs_error.
template <class T, int d>
::testing::AssertionResult AssertVectorNear(const char* expr1,
                                            const char* expr2,
                                            const char* abs_error_expr,
                                            const mathfu::Vector<T, d>& v1,
                                            const mathfu::Vector<T, d>& v2,
                                            const T abs_error) {
  T diff;
  for (int32_t i = 0; i < d; ++i) {
    diff = fabs(v1[i] - v2[i]);
    if (diff > abs_error) {
      return ::testing::AssertionFailure()
             << "The difference between " << FormatVector(expr1, v1) << " and "
             << FormatVector(expr2, v2) << " is " << diff << ", which exceeds "
             << abs_error_expr;
    }
  }

  return ::testing::AssertionSuccess();
}

// This will test initialization by passing in values. The template parameter d
// corresponds to the size of the vector.
template <class T, int d>
void Initialization_Test(const T& precision) {
  // This will test initialization of the vector using a random single value.
  // The expected result is that all entries equal the given value.
  mathfu::Vector<T, d> vector_splat(static_cast<T>(3.1));
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(3.1, vector_splat[i], precision);
  }
  T x[d];
  for (int i = 0; i < d; ++i) {
    x[i] = rand() / static_cast<T>(RAND_MAX) * 100.f;
  }
  // This will test initialization of the vector using a c style array of
  // values.
  mathfu::Vector<T, d> vector_arr(x);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x[i], vector_arr[i], precision);
  }
  // This will test copy constructor making sure that the new matrix equals
  // the old one.
  mathfu::Vector<T, d> vector_copy(vector_arr);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x[i], vector_copy[i], precision);
  }
  // This will make sure the copy was deep and changing the values of the
  // copied matrix does not effect the original.
  vector_copy -= mathfu::Vector<T, d>(1);
  EXPECT_NE(vector_copy[0], vector_arr[0]);

  // Construct a vector from an integer vector.
  mathfu::Vector<int, d> integer_vector;
  for (int i = 0; i < d; ++i) {
    integer_vector[i] = i;
  }
  mathfu::Vector<T, d> other_vector(integer_vector);
  for (int i = 0; i < d; ++i) {
    EXPECT_EQ(static_cast<int>(other_vector[i]), integer_vector[i]);
  }
}
TEST_ALL_F(Initialization)

// This will test initialization by specifying all values explicitly.
template <class T>
void InitializationPerDimension_Test(const T& precision) {
  mathfu::Vector<T, 2> f2_vector(static_cast<T>(5.3), static_cast<T>(7.1));
  EXPECT_NEAR(5.3, f2_vector[0], precision);
  EXPECT_NEAR(7.1, f2_vector[1], precision);
  mathfu::Vector<T, 3> f3_vector(static_cast<T>(4.3), static_cast<T>(1.1),
                                 static_cast<T>(3.2));
  EXPECT_NEAR(4.3, f3_vector[0], precision);
  EXPECT_NEAR(1.1, f3_vector[1], precision);
  EXPECT_NEAR(3.2, f3_vector[2], precision);
  mathfu::Vector<T, 4> f4_vector(static_cast<T>(2.3), static_cast<T>(4.6),
                                 static_cast<T>(9.2), static_cast<T>(15.5));
  EXPECT_NEAR(2.3, f4_vector[0], precision);
  EXPECT_NEAR(4.6, f4_vector[1], precision);
  EXPECT_NEAR(9.2, f4_vector[2], precision);
  EXPECT_NEAR(15.5, f4_vector[3], precision);
}
TEST_SCALAR_F(InitializationPerDimension)

// Test initialization from a packed vector.
template <class T, int d>
void InitializationPacked_Test(const T& precision) {
  (void)precision;
  mathfu::VectorPacked<T, d> packed;
  for (int i = 0; i < d; ++i) {
    packed.data_[i] = static_cast<T>(i);
  }
  mathfu::Vector<T, d> unpacked(packed);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(packed.data_[i], unpacked[i], static_cast<T>(0))
        << "Element " << i;
  }
}
TEST_ALL_F(InitializationPacked)

// Test vector packing.
template <class T, int d>
void PackedSerialization_Test(const T& precision) {
  (void)precision;
  mathfu::Vector<T, d> unpacked;
  for (int i = 0; i < d; ++i) {
    unpacked[i] = static_cast<T>(i);
  }

  mathfu::VectorPacked<T, d> packed_construction(unpacked);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(unpacked[i], packed_construction.data_[i], static_cast<T>(0))
        << "Element " << i;
  }

  mathfu::VectorPacked<T, d> packed_assignment;
  packed_assignment = unpacked;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(unpacked[i], packed_assignment.data_[i], static_cast<T>(0))
        << "Element " << i;
  }
}
TEST_ALL_F(PackedSerialization)

template <class T, int d>
void Negate_Test(const T& precision) {
  T x[d];
  for (int i = 0; i < d; ++i) {
    x[i] = rand() / static_cast<T>(RAND_MAX) * 100.f;
  }

  mathfu::Vector<T, d> vector(x);

  // Test negation; make sure each element is negated.
  mathfu::Vector<T, d> neg_vector(-vector);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(-x[i], neg_vector[i], precision);
  }
}
TEST_ALL_F(Negate)

template <class T, int d>
void Add_Test(const T& precision) {
  T x1[d], x2[d];
  T scalar = rand() / static_cast<T>(RAND_MAX) * 100.f;
  for (int i = 0; i < d; ++i) {
    x1[i] = rand() / static_cast<T>(RAND_MAX) * 100.f;
  }
  for (int i = 0; i < d; ++i) {
    x2[i] = rand() / static_cast<T>(RAND_MAX) * 100.f;
  }

  mathfu::Vector<T, d> vector1(x1), vector2(x2);

  mathfu::Vector<T, d> sum_vector(vector1 + vector2);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] + x2[i], sum_vector[i], precision);
  }
  mathfu::Vector<T, d> sum_vector_scalar(vector1 + scalar);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] + scalar, sum_vector_scalar[i], precision);
  }
  mathfu::Vector<T, d> sum_scalar_vector(scalar + vector1);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(scalar + x1[i], sum_scalar_vector[i], precision);
  }
  mathfu::Vector<T, d> sum_assign_vector_vector(vector1);
  sum_assign_vector_vector += vector2;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] + x2[i], sum_assign_vector_vector[i], precision);
  }
  mathfu::Vector<T, d> sum_assign_vector_scalar(vector1);
  sum_assign_vector_scalar += scalar;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] + scalar, sum_assign_vector_scalar[i], precision);
  }
}
TEST_ALL_F(Add)

template <class T, int d>
void Sub_Test(const T& precision) {
  T x1[d], x2[d];
  T scalar = rand() / static_cast<T>(RAND_MAX) * 100.f;
  for (int i = 0; i < d; ++i) {
    x1[i] = rand() / static_cast<T>(RAND_MAX) * 100.f;
  }
  for (int i = 0; i < d; ++i) {
    x2[i] = rand() / static_cast<T>(RAND_MAX) * 100.f;
  }

  mathfu::Vector<T, d> vector1(x1), vector2(x2);

  mathfu::Vector<T, d> diff_vector(vector1 - vector2);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] - x2[i], diff_vector[i], precision);
  }
  mathfu::Vector<T, d> diff_vector_scalar(vector1 - scalar);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] - scalar, diff_vector_scalar[i], precision);
  }
  mathfu::Vector<T, d> diff_scalar_vector(scalar - vector1);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(scalar - x1[i], diff_scalar_vector[i], precision);
  }
  mathfu::Vector<T, d> diff_assign_vector_vector(vector1);
  diff_assign_vector_vector -= vector2;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] - x2[i], diff_assign_vector_vector[i], precision);
  }
  mathfu::Vector<T, d> diff_assign_vector_scalar(vector1);
  diff_assign_vector_scalar -= scalar;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] - scalar, diff_assign_vector_scalar[i], precision);
  }
}
TEST_ALL_F(Sub)

template <class T, int d>
void Mul_Test(const T& precision) {
  T x1[d], x2[d];
  T scalar(static_cast<T>(1.4));
  for (int i = 0; i < d; ++i) {
    x1[i] = rand() / static_cast<T>(RAND_MAX);
  }
  for (int i = 0; i < d; ++i) {
    x2[i] = rand() / static_cast<T>(RAND_MAX);
  }

  mathfu::Vector<T, d> vector1(x1), vector2(x2);

  mathfu::Vector<T, d> mul_vector(vector1 * vector2);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] * x2[i], mul_vector[i], precision);
  }
  mathfu::Vector<T, d> mul_vector_scalar(vector1 * scalar);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] * scalar, mul_vector_scalar[i], precision);
  }
  mathfu::Vector<T, d> mul_scalar_vector(scalar * vector2);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x2[i] * scalar, mul_scalar_vector[i], precision);
  }
  mathfu::Vector<T, d> mul_assign_vector_vector(vector1);
  mul_assign_vector_vector *= vector2;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] * x2[i], mul_assign_vector_vector[i], precision);
  }
  mathfu::Vector<T, d> mul_assign_vector_scalar(vector1);
  mul_assign_vector_scalar *= scalar;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] * scalar, mul_assign_vector_scalar[i], precision);
  }
}
TEST_ALL_F(Mul)

template <class T, int d>
void Div_Test(const T& precision) {
  T x1[d], x2[d];
  T scalar = (rand() / static_cast<T>(RAND_MAX)) + 1;
  for (int i = 0; i < d; ++i) {
    x1[i] = (rand() / static_cast<T>(RAND_MAX)) + 1;
  }
  for (int i = 0; i < d; ++i) {
    x2[i] = (rand() / static_cast<T>(RAND_MAX)) + 1;
  }

  mathfu::Vector<T, d> vector1(x1), vector2(x2);

  mathfu::Vector<T, d> div_vector_vector(vector1 / vector2);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] / x2[i], div_vector_vector[i], precision);
  }
  mathfu::Vector<T, d> div_vector_scalar(vector1 / scalar);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] / scalar, div_vector_scalar[i], precision);
  }
  mathfu::Vector<T, d> div_scalar_vector(scalar / vector1);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(scalar / x1[i], div_scalar_vector[i], precision);
  }
  mathfu::Vector<T, d> div_assign_vector_vector(vector1);
  div_assign_vector_vector /= vector2;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] / x2[i], div_assign_vector_vector[i], precision);
  }
  mathfu::Vector<T, d> div_assign_vector_scalar(vector1);
  div_assign_vector_scalar /= scalar;
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x1[i] / scalar, div_assign_vector_scalar[i], precision);
  }
}
TEST_ALL_F(Div)

// This will test normalizing a vector. The template parameter d corresponds to
// the size of the vector.
template <class T, int d>
void Norm_Test(const T& precision) {
  T x[d];
  for (int i = 0; i < d; ++i) {
    x[i] = rand() / static_cast<T>(RAND_MAX);
  }

  mathfu::Vector<T, d> vector(x);
  vector.Normalize();
  // This will verify that the dot product is 1.
  T dot = mathfu::Vector<T, d>::DotProduct(vector, vector);
  EXPECT_NEAR(dot, 1, precision);
}
TEST_ALL_F(Norm)

// This will test the multiplication of vectors by vectors and scalars. The
// template parameter d corresponds to the size of the vector.
template <class T, int d>
void Dot_Test(const T& precision) {
  T x1[d], x2[d];
  for (int i = 0; i < d; ++i) {
    x1[i] = rand() / static_cast<T>(RAND_MAX);
  }
  for (int i = 0; i < d; ++i) {
    x2[i] = rand() / static_cast<T>(RAND_MAX);
  }

  mathfu::Vector<T, d> vector1(x1), vector2(x2);

  // This will test the dot product of two vectors and verify the result
  // is mathematically correct.
  T my_dot = 0;
  for (int i = 0; i < d; ++i) {
    my_dot += x1[i] * x2[i];
  }

  T vec_dot = mathfu::Vector<T, d>::DotProduct(vector1, vector2);
  EXPECT_NEAR(my_dot, vec_dot, precision);
}
TEST_ALL_F(Dot)

// This will test the cross product of two vectors.
template <class T>
void Cross_Test(const T& precision) {
  mathfu::Vector<T, 3> f1_vector(static_cast<T>(1.1), static_cast<T>(4.5),
                                 static_cast<T>(9.8));
  mathfu::Vector<T, 3> f2_vector(-static_cast<T>(1.4), static_cast<T>(9.5),
                                 static_cast<T>(3.2));
  f1_vector.Normalize();
  f2_vector.Normalize();
  mathfu::Vector<T, 3> fcross_vector(
      mathfu::Vector<T, 3>::CrossProduct(f1_vector, f2_vector));
  // This will verify that v1*(v1xv2) and v2*(v1xv2) are 0.
  T f1_dot = mathfu::Vector<T, 3>::DotProduct(fcross_vector, f1_vector);
  T f2_dot = mathfu::Vector<T, 3>::DotProduct(fcross_vector, f2_vector);
  EXPECT_NEAR(f1_dot, 0, precision * 10);
  EXPECT_NEAR(f2_dot, 0, precision * 10);
}
TEST_SCALAR_F(Cross)

// This will test that the 2D cross product of perpendicular vectors gives the
// area of the rectangle they span.
template <class T>
void CrossProduct2D_Perpendicular_Test(const T& precision) {
  mathfu::Vector<T, 2> v1(static_cast<T>(3), static_cast<T>(0));
  mathfu::Vector<T, 2> v2(static_cast<T>(0), static_cast<T>(5));
  T result = mathfu::Vector<T, 2>::CrossProduct(v1, v2);
  EXPECT_NEAR(static_cast<T>(15), result, precision);
}
TEST_SCALAR_F(CrossProduct2D_Perpendicular)

// This will test that the 2D cross product of parallel vectors is 0.
template <class T>
void CrossProduct2D_Parallel_Test(const T& precision) {
  mathfu::Vector<T, 2> v1(static_cast<T>(2), static_cast<T>(3));
  mathfu::Vector<T, 2> v2(static_cast<T>(4), static_cast<T>(6));
  T result = mathfu::Vector<T, 2>::CrossProduct(v1, v2);
  EXPECT_NEAR(static_cast<T>(0), result, precision);
}
TEST_SCALAR_F(CrossProduct2D_Parallel)

// This will test that the 2D cross product of anti-parallel vectors is 0.
template <class T>
void CrossProduct2D_AntiParallel_Test(const T& precision) {
  mathfu::Vector<T, 2> v1(static_cast<T>(2), static_cast<T>(3));
  mathfu::Vector<T, 2> v2(static_cast<T>(-4), static_cast<T>(-6));
  T result = mathfu::Vector<T, 2>::CrossProduct(v1, v2);
  EXPECT_NEAR(static_cast<T>(0), result, precision);
}
TEST_SCALAR_F(CrossProduct2D_AntiParallel)

// This will test that counter-clockwise winding gives a positive result.
template <class T>
void CrossProduct2D_CounterClockwise_Test(const T& precision) {
  // v1 along +x, v2 along +y is counter-clockwise.
  mathfu::Vector<T, 2> v1(static_cast<T>(1), static_cast<T>(0));
  mathfu::Vector<T, 2> v2(static_cast<T>(0), static_cast<T>(1));
  T result = mathfu::Vector<T, 2>::CrossProduct(v1, v2);
  EXPECT_GT(result, static_cast<T>(0));
  (void)precision;
}
TEST_SCALAR_F(CrossProduct2D_CounterClockwise)

// This will test that clockwise winding gives a negative result.
template <class T>
void CrossProduct2D_Clockwise_Test(const T& precision) {
  // v1 along +y, v2 along +x is clockwise.
  mathfu::Vector<T, 2> v1(static_cast<T>(0), static_cast<T>(1));
  mathfu::Vector<T, 2> v2(static_cast<T>(1), static_cast<T>(0));
  T result = mathfu::Vector<T, 2>::CrossProduct(v1, v2);
  EXPECT_LT(result, static_cast<T>(0));
  (void)precision;
}
TEST_SCALAR_F(CrossProduct2D_Clockwise)

// This will test that the cross product of a vector with itself is 0.
template <class T>
void CrossProduct2D_Self_Test(const T& precision) {
  mathfu::Vector<T, 2> v(static_cast<T>(7), static_cast<T>(11));
  T result = mathfu::Vector<T, 2>::CrossProduct(v, v);
  EXPECT_NEAR(static_cast<T>(0), result, precision);
}
TEST_SCALAR_F(CrossProduct2D_Self)

// Create a vector with random values between 0~1.
template <class T, int d>
mathfu::Vector<T, d> RandomVector() {
  T x[d];
  for (int i = 0; i < d; ++i) {
    x[i] = rand() / static_cast<T>(RAND_MAX);
  }
  return mathfu::Vector<T, d>(x);
}

// This will test an equal lerp of two vectors gives their average.
template <class T, int d>
void LerpHalf_Test(const T& precision) {
  mathfu::Vector<T, d> vector1(RandomVector<T, d>());
  mathfu::Vector<T, d> vector2(RandomVector<T, d>());
  mathfu::Vector<T, d> flerp_vector(
      mathfu::Vector<T, d>::Lerp(vector1, vector2, static_cast<T>(0.5)));
  // This will verify f1_vector.x + f2_vector.x == 2 * flerp_vector
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(vector1[i] + vector2[i], static_cast<T>(2.0) * flerp_vector[i],
                precision * 10);
  }
}
TEST_ALL_F(LerpHalf)

// This will test that lerp with weight 0 returns the first vector.
template <class T, int d>
void Lerp0_Test(const T& precision) {
  mathfu::Vector<T, d> vector1(RandomVector<T, d>());
  mathfu::Vector<T, d> vector2(RandomVector<T, d>());
  mathfu::Vector<T, d> flerp_vector(
      mathfu::Vector<T, d>::Lerp(vector1, vector2, static_cast<T>(0.0)));
  // This will verify f1_vector.x + f2_vector.x == 2 * flerp_vector
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(vector1[i], flerp_vector[i], precision * 10);
  }
}
TEST_ALL_F(Lerp0)

// This will test that lerp with weight 1 returns the second vector.
template <class T, int d>
void Lerp1_Test(const T& precision) {
  mathfu::Vector<T, d> vector1(RandomVector<T, d>());
  mathfu::Vector<T, d> vector2(RandomVector<T, d>());
  mathfu::Vector<T, d> flerp_vector(
      mathfu::Vector<T, d>::Lerp(vector1, vector2, static_cast<T>(1.0)));
  // This will verify f1_vector.x + f2_vector.x == 2 * flerp_vector
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(vector2[i], flerp_vector[i], precision * 10);
  }
}
TEST_ALL_F(Lerp1)

// This will test initialization by specifying all values explicitly.
template <class T>
void Clamp_Test() {
  const T min = static_cast<T>(-1);
  const T max = static_cast<T>(8);
  const T inside = static_cast<T>(7);
  const T above = static_cast<T>(9);
  const T below = static_cast<T>(-11);

  EXPECT_EQ(mathfu::Clamp<T>(inside, min, max), inside);
  EXPECT_EQ(mathfu::Clamp<T>(above, min, max), max);
  EXPECT_EQ(mathfu::Clamp<T>(below, min, max), min);
  EXPECT_EQ(mathfu::Clamp<T>(max, min, max), max);
  EXPECT_EQ(mathfu::Clamp<T>(min, min, max), min);
}
TEST_F(VectorTests, Clamp) {
  Clamp_Test<float>();
  Clamp_Test<double>();
  Clamp_Test<int>();
}

// Tests for int/float/double based lerp.  (i. e. not part of a vector)
template <class T>
void Numeric_Lerp_Test(const T& precision) {
  const T zero = static_cast<T>(0);
  const T one = static_cast<T>(1);

  const T a = static_cast<T>(10);
  const T b = static_cast<T>(20);
  const T midpoint = static_cast<T>(0.5);
  const T two_fifths = static_cast<T>(0.4);
  const T seven_tenths = static_cast<T>(0.7);
  const T midpoint_result = static_cast<T>(15);
  const T two_fifths_result = static_cast<T>(14);
  const T seven_tenths_result = static_cast<T>(17);

  EXPECT_EQ(mathfu::Lerp<T>(a, b, zero), a);
  EXPECT_EQ(mathfu::Lerp<T>(a, b, one), b);
  EXPECT_EQ(mathfu::Lerp<T>(-a, b, zero), -a);
  EXPECT_EQ(mathfu::Lerp<T>(-a, b, one), b);
  EXPECT_EQ(mathfu::Lerp<T>(a, -b, zero), a);
  EXPECT_EQ(mathfu::Lerp<T>(a, -b, one), -b);
  EXPECT_EQ(mathfu::Lerp<T>(-a, -b, zero), -a);
  EXPECT_EQ(mathfu::Lerp<T>(-a, -b, one), -b);

  EXPECT_NE(mathfu::Lerp<T>(a, b, midpoint), a);

  EXPECT_NEAR(mathfu::Lerp<T>(a, b, midpoint), midpoint_result, precision);
  EXPECT_NEAR(mathfu::Lerp<T>(a, b, two_fifths), two_fifths_result, precision);
  EXPECT_NEAR(mathfu::Lerp<T>(a, b, seven_tenths), seven_tenths_result,
              precision);
}
TEST_SCALAR_F(Numeric_Lerp)

// Tests the random-in-range function for vectors.
// Given a pair of vectors, it should return a third vector whose elements
// are bounded by the corresponding elements in the argument vectors.
template <class T, int d>
void Vector_RandomInRange_Test(const T& precision) {
  (void)precision;
  mathfu::Vector<T, d> min, max, result1, result2;

  for (int count = 0; count < 100; count++) {
    for (int i = 0; i < d; i++) {
      min[i] = -i - 10;
      max[i] = i * 2 + 2;
    }
    result1 = mathfu::Vector<T, d>::RandomInRange(min, max);
    result2 = mathfu::Vector<T, d>::RandomInRange(max, min);
    for (int i = 0; i < d; i++) {
      EXPECT_GE(result1[i], min[i]);
      EXPECT_LE(result1[i], max[i]);

      EXPECT_GE(result2[i], min[i]);
      EXPECT_LE(result2[i], max[i]);
    }
  }
}
TEST_ALL_INTS_F(Vector_RandomInRange)

// Tests the generic Random in Range function in Mathfu.
template <class T>
void RandomInRange_Test(const T& precision) {
  (void)precision;
  for (int count = 0; count < 100; count++) {
    T result = mathfu::RandomInRange(static_cast<T>(0), static_cast<T>(100));
    EXPECT_GE(result, 0);
    EXPECT_LT(result, 100);
  }
  for (int count = 0; count < 100; count++) {
    T result = mathfu::RandomInRange(static_cast<T>(-100), static_cast<T>(0));
    EXPECT_GE(result, -100);
    EXPECT_LE(result, 0);
  }
  EXPECT_EQ(0, mathfu::RandomInRange(0, 0));
  EXPECT_EQ(-5, mathfu::RandomInRange(-5, -5));
  EXPECT_EQ(23, mathfu::RandomInRange(23, 23));
}
TEST_SCALAR_AND_INT_F(RandomInRange)

// This will test initialization by passing in values. The template parameter d
// corresponds to the size of the vector.
template <class T, int d>
void Accessor_Test(const T& precision) {
  (void)precision;
  T x[d];
  for (int i = 0; i < d; ++i) {
    x[i] = rand() / static_cast<T>(RAND_MAX) * 100.f;
  }

  mathfu::Vector<T, d> vector(x);
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x[i], vector[i], static_cast<T>(0));
  }
  for (int i = 0; i < d; ++i) {
    EXPECT_NEAR(x[i], vector(i), static_cast<T>(0));
  }
}
TEST_ALL_F(Accessor)

// This will test initialization by passing in values. The template parameter d
// corresponds to the size of the vector.
template <class T, int d>
void Max_Test(const T& precision) {
  (void)precision;
  T value1[] = {0, 0, 0, 0, 0};
  T value2[] = {1, 2, 3, 4, 5};
  mathfu::Vector<T, d> v1(value1);
  mathfu::Vector<T, d> v2(value2);

  // vs. zero vector.
  mathfu::Vector<T, d> v3 = mathfu::Vector<T, d>::Max(v1, v2);

  // Comparing to {1, 2, 3, 4, 5}
  mathfu::Vector<T, d> r1(value2);
  EXPECT_PRED_FORMAT2(AssertVectorEqual, v3, r1);

  // inverse vs. zero vector.
  mathfu::Vector<T, d> v4 = mathfu::Vector<T, d>::Max(v2, v1);

  // Comparing to {1, 2, 3, 4, 5}
  EXPECT_PRED_FORMAT2(AssertVectorEqual, v4, r1);

  // vs. negative vector.
  T negative_value[] = {-1, -2, -3, -4, -5};
  v2 = mathfu::Vector<T, d>(negative_value);
  mathfu::Vector<T, d> v5 = mathfu::Vector<T, d>::Max(v1, v2);

  // Comparing to {0, 0, 0, 0, 0}
  mathfu::Vector<T, d> r2(value1);
  EXPECT_PRED_FORMAT2(AssertVectorEqual, v5, r2);

  // vs. interleaving 2 vectors.
  T value3[] = {0, 2, 0, 4, 0};
  T value4[] = {1, 0, 3, 0, 5};
  v1 = mathfu::Vector<T, d>(value3);
  v2 = mathfu::Vector<T, d>(value4);
  mathfu::Vector<T, d> v6 = mathfu::Vector<T, d>::Max(v1, v2);

  // Comparing to {1, 2, 3, 4, 5}
  mathfu::Vector<T, d> r3(value2);
  EXPECT_PRED_FORMAT2(AssertVectorEqual, v6, r3);
}
TEST_ALL_F(Max)

// This will test initialization by passing in values. The template parameter d
// corresponds to the size of the vector.
template <class T, int d>
void Min_Test(const T& precision) {
  (void)precision;
  T value1[] = {0, 0, 0, 0, 0};
  T value2[] = {1, 2, 3, 4, 5};
  mathfu::Vector<T, d> v1(value1);
  mathfu::Vector<T, d> v2(value2);

  // vs. zero vector.
  mathfu::Vector<T, d> v3 = mathfu::Vector<T, d>::Min(v1, v2);

  // Comparing to {0, 0, 0, 0, 0}
  mathfu::Vector<T, d> r1(value1);
  EXPECT_PRED_FORMAT2(AssertVectorEqual, v3, r1);

  // inverse vs. zero vector.
  mathfu::Vector<T, d> v4 = mathfu::Vector<T, d>::Min(v2, v1);

  // Comparing to {0, 0, 0, 0, 0}
  EXPECT_PRED_FORMAT2(AssertVectorEqual, v4, r1);

  // vs. negative vector.
  T negative_value[] = {-1, -2, -3, -4, -5};
  v2 = mathfu::Vector<T, d>(negative_value);
  mathfu::Vector<T, d> v5 = mathfu::Vector<T, d>::Min(v1, v2);

  // Comparing to {-1, -2, -3, -4, -5}
  mathfu::Vector<T, d> r2(negative_value);
  EXPECT_PRED_FORMAT2(AssertVectorEqual, v5, r2);

  // vs. interleaving 2 vectors.
  T value3[] = {0, 2, 0, 4, 0};
  T value4[] = {1, 0, 3, 0, 5};
  v1 = mathfu::Vector<T, d>(value3);
  v2 = mathfu::Vector<T, d>(value4);
  mathfu::Vector<T, d> v6 = mathfu::Vector<T, d>::Min(v1, v2);

  // Comparing to {0, 0, 0, 0, 0}
  EXPECT_PRED_FORMAT2(AssertVectorEqual, v6, r1);
}
TEST_ALL_F(Min)

// Test distance function for vector2.
TEST_F(VectorTests, Distance_Vector2) {
  using namespace mathfu;
  const Vector<float, 2> a(0, 10);
  const Vector<float, 2> b(15, 12);
  const float distance = Vector<float, 2>::Distance(a, b);
  EXPECT_NEAR(distance, sqrtf(15 * 15 + 2 * 2), FLOAT_PRECISION);
}

// Test distance function for vector3.
TEST_F(VectorTests, Distance_Vector3) {
  using namespace mathfu;
  const Vector<float, 3> a(0, 10, 3);
  const Vector<float, 3> b(15, 12, -4);
  const float distance = Vector<float, 3>::Distance(a, b);
  EXPECT_NEAR(distance, sqrtf(15 * 15 + 2 * 2 + 7 * 7), FLOAT_PRECISION);
}

// Test distance function for vector4.
TEST_F(VectorTests, Distance_Vector4) {
  using namespace mathfu;
  const Vector<float, 4> a(9, 10, 3, 5);
  const Vector<float, 4> b(15, 12, -4, 1);
  const float distance = Vector<float, 4>::Distance(a, b);
  EXPECT_NEAR(distance, sqrtf(6 * 6 + 2 * 2 + 7 * 7 + 4 * 4), FLOAT_PRECISION);
}

TEST_F(VectorTests, Angle_Vector2) {
  using Vec2 = mathfu::Vector<float, 2>;
  Vec2 a(0, 1);
  Vec2 b(1, 0);
  EXPECT_NEAR(Vec2::Angle(a, b), mathfu::kPi / 2.0f, FLOAT_PRECISION);

  a = Vec2(1, 1);
  b = Vec2(0, -1);
  EXPECT_NEAR(Vec2::Angle(a, b), 3.0f * mathfu::kPi / 4.0f, FLOAT_PRECISION);
}

TEST_F(VectorTests, Angle_Vector3) {
  using Vec3 = mathfu::Vector<float, 3>;
  Vec3 a(0, 0, 1);
  Vec3 b(0, 1, 0);
  EXPECT_NEAR(Vec3::Angle(a, b), mathfu::kPi / 2.0f, FLOAT_PRECISION);

  a = Vec3(1, 2, 3);
  b = Vec3(-10, 3, -1);
  EXPECT_NEAR(Vec3::Angle(a, b), 1.75013259f, FLOAT_PRECISION);

  a = Vec3(1, 2, 3);
  b = Vec3(-1, -2, -3);
  EXPECT_NEAR(Vec3::Angle(a, b), mathfu::kPi, FLOAT_PRECISION * 1000.f);
}

// Test that AngleHelper does not return NaN for anti-parallel vectors where
// floating point rounding pushes cos_val slightly below -1.
TEST_F(VectorTests, Angle_AntiParallelDoesNotReturnNaN) {
  using Vec3 = mathfu::Vector<float, 3>;
  Vec3 a(1, 0, 0);
  Vec3 b(-1, 0, 0);
  float angle = Vec3::Angle(a, b);
  EXPECT_FALSE(std::isnan(angle));
  EXPECT_NEAR(angle, mathfu::kPi, FLOAT_PRECISION);

  // Also test with parallel vectors (cos_val near +1).
  b = Vec3(1, 0, 0);
  angle = Vec3::Angle(a, b);
  EXPECT_FALSE(std::isnan(angle));
  EXPECT_NEAR(angle, 0.0f, FLOAT_PRECISION);
}

// Tests scalar RoundUpToPowerOf2 for int32_t.
TEST_F(VectorTests, RoundUpToPowerOf2_Int32) {
  // Zero returns zero.
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(0)), 0);

  // One returns one (already a power of 2).
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(1)), 1);

  // Powers of 2 remain unchanged.
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(2)), 2);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(4)), 4);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(64)), 64);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(1024)), 1024);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(1 << 30)),
            (1 << 30));

  // Non-powers of 2 round up.
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(3)), 4);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(5)), 8);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(6)), 8);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(7)), 8);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(9)), 16);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(100)), 128);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(1000)), 1024);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int32_t>(1025)), 2048);
}

// Tests scalar RoundUpToPowerOf2 for uint32_t.
TEST_F(VectorTests, RoundUpToPowerOf2_Uint32) {
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint32_t>(0)),
            static_cast<uint32_t>(0));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint32_t>(1)),
            static_cast<uint32_t>(1));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint32_t>(2)),
            static_cast<uint32_t>(2));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint32_t>(3)),
            static_cast<uint32_t>(4));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint32_t>(5)),
            static_cast<uint32_t>(8));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint32_t>(255)),
            static_cast<uint32_t>(256));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint32_t>(1u << 31)),
            static_cast<uint32_t>(1u << 31));
}

// Tests scalar RoundUpToPowerOf2 for int64_t.
TEST_F(VectorTests, RoundUpToPowerOf2_Int64) {
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int64_t>(0)),
            static_cast<int64_t>(0));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int64_t>(1)),
            static_cast<int64_t>(1));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int64_t>(2)),
            static_cast<int64_t>(2));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int64_t>(3)),
            static_cast<int64_t>(4));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int64_t>(5)),
            static_cast<int64_t>(8));

  // Test values beyond 32-bit range.
  int64_t large = static_cast<int64_t>(1) << 32;
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(large), large);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(large + 1), large * 2);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(large - 1), large);

  int64_t very_large = static_cast<int64_t>(1) << 62;
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(very_large), very_large);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(very_large - 1), very_large);
}

// Tests scalar RoundUpToPowerOf2 for uint64_t.
TEST_F(VectorTests, RoundUpToPowerOf2_Uint64) {
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint64_t>(0)),
            static_cast<uint64_t>(0));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint64_t>(1)),
            static_cast<uint64_t>(1));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint64_t>(3)),
            static_cast<uint64_t>(4));

  // Test values beyond 32-bit range.
  uint64_t large = static_cast<uint64_t>(1) << 32;
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(large), large);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(large + 1), large * 2);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(large - 1), large);

  uint64_t very_large = static_cast<uint64_t>(1) << 63;
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(very_large), very_large);
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(very_large - 1), very_large);
}

// Tests scalar RoundUpToPowerOf2 for int16_t.
TEST_F(VectorTests, RoundUpToPowerOf2_Int16) {
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int16_t>(0)),
            static_cast<int16_t>(0));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int16_t>(1)),
            static_cast<int16_t>(1));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int16_t>(3)),
            static_cast<int16_t>(4));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int16_t>(255)),
            static_cast<int16_t>(256));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<int16_t>(1 << 14)),
            static_cast<int16_t>(1 << 14));
}

// Tests scalar RoundUpToPowerOf2 for uint8_t.
TEST_F(VectorTests, RoundUpToPowerOf2_Uint8) {
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint8_t>(0)),
            static_cast<uint8_t>(0));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint8_t>(1)),
            static_cast<uint8_t>(1));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint8_t>(2)),
            static_cast<uint8_t>(2));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint8_t>(3)),
            static_cast<uint8_t>(4));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint8_t>(127)),
            static_cast<uint8_t>(128));
  EXPECT_EQ(mathfu::RoundUpToPowerOf2(static_cast<uint8_t>(128)),
            static_cast<uint8_t>(128));
}

// Tests scalar RoundUpToPowerOf2 for all int values from 0 to 1024.
TEST_F(VectorTests, RoundUpToPowerOf2_Exhaustive_Int32) {
  int32_t expected = 0;
  for (int32_t i = 0; i <= 1024; ++i) {
    // Compute expected: smallest power of 2 >= i (with 0 mapping to 0).
    if (i == 0) {
      expected = 0;
    } else if (i == 1) {
      expected = 1;
    } else if ((i & (i - 1)) == 0) {
      // i is already a power of 2.
      expected = i;
    } else {
      // Find next power of 2.
      expected = 1;
      while (expected < i) expected <<= 1;
    }
    EXPECT_EQ(mathfu::RoundUpToPowerOf2(i), expected)
        << "Failed for input " << i;
  }
}

// Tests the RoundUpToPowerOf2 function for vectors.
// Given a vector, it should return a vector whose elements are rounded up to
// the nearest power of 2.
template <class T, int d>
void Vector_RoundUpToPowerOf2_Test(const T& precision) {
  (void)precision;
  mathfu::Vector<T, d> powof2, result;

  for (int count = 0; count < 1024; count++) {
    for (int i = 0; i < d; i++) {
      powof2[i] = static_cast<T>(count);
    }
    result = mathfu::RoundUpToPowerOf2(powof2);
    T expected = static_cast<T>(mathfu::RoundUpToPowerOf2(count));
    for (int i = 0; i < d; i++) {
      EXPECT_EQ(result[i], expected);
    }
  }
}
TEST_ALL_INTS_F(Vector_RoundUpToPowerOf2)
TEST_ALL_F(Vector_RoundUpToPowerOf2)

// Test the compilation of basic vector opertations given in the sample file.
// This will test creation of two vectors and computing their cross product.
TEST_F(VectorTests, SampleTest) {
  using namespace mathfu;
  /// @doxysnippetstart Chapter02_Vectors.md Vector_Sample
  Vector<float, 3> point1(0.5f, 0.4f, 0.1f);
  Vector<float, 3> point2(0.4f, 0.9f, 0.1f);
  Vector<float, 3> point3(0.1f, 0.8f, 0.6f);

  Vector<float, 3> vector1 = point2 - point1;
  Vector<float, 3> vector2 = point3 - point1;

  Vector<float, 3> normal = Vector<float, 3>::CrossProduct(vector2, vector1);
  /// @doxysnippetend
  const float precision = 1e-2f;
  EXPECT_NEAR(-0.25f, normal[0], precision);
  EXPECT_NEAR(-0.05f, normal[1], precision);
  EXPECT_NEAR(-0.16f, normal[2], precision);
}

// This will test that the constants have the correct values.
TEST_F(VectorTests, ConstantTest) {
  // Check values of various kinds of Vector2s.
  for (int i = 0; i < 2; ++i) {
    VECTOR_TEST_CONSTANT_EQ(kZeros2, i, 0);
    VECTOR_TEST_CONSTANT_EQ(kOnes2, i, 1);
    VECTOR_TEST_CONSTANT_EQ(kAxisX2, i, i == 0 ? 1 : 0);
    VECTOR_TEST_CONSTANT_EQ(kAxisY2, i, i == 1 ? 1 : 0);
  }

  // Check values of various kinds of Vector3s.
  for (int i = 0; i < 3; ++i) {
    VECTOR_TEST_CONSTANT_EQ(kZeros3, i, 0);
    VECTOR_TEST_CONSTANT_EQ(kOnes3, i, 1);
    VECTOR_TEST_CONSTANT_EQ(kAxisX3, i, i == 0 ? 1 : 0);
    VECTOR_TEST_CONSTANT_EQ(kAxisY3, i, i == 1 ? 1 : 0);
    VECTOR_TEST_CONSTANT_EQ(kAxisZ3, i, i == 2 ? 1 : 0);
  }

  // Check values of various kinds of Vector4s.
  for (int i = 0; i < 4; ++i) {
    VECTOR_TEST_CONSTANT_EQ(kZeros4, i, 0);
    VECTOR_TEST_CONSTANT_EQ(kOnes4, i, 1);
    VECTOR_TEST_CONSTANT_EQ(kAxisX4, i, i == 0 ? 1 : 0);
    VECTOR_TEST_CONSTANT_EQ(kAxisY4, i, i == 1 ? 1 : 0);
    VECTOR_TEST_CONSTANT_EQ(kAxisZ4, i, i == 2 ? 1 : 0);
    VECTOR_TEST_CONSTANT_EQ(kAxisW4, i, i == 3 ? 1 : 0);
  }
}

// This will test the == vectors operator.
template <class T, int d>
void Equal_Test(const T& precision) {
  mathfu::Vector<T, d> expected;
  for (int i = 0; i < d; ++i) {
    expected[i] = static_cast<T>(i * precision);
  }
  mathfu::Vector<T, d> copy(expected);
  EXPECT_TRUE(expected == copy);

  mathfu::Vector<T, d> close(expected - static_cast<T>(1));
  EXPECT_FALSE(expected == close);
}
TEST_ALL_F(Equal)
TEST_ALL_INTS_F(Equal)

// This will test the != vectors operator.
template <class T, int d>
void NotEqual_Test(const T& precision) {
  mathfu::Vector<T, d> expected;
  for (int i = 0; i < d; ++i) {
    expected[i] = static_cast<T>(i * precision);
  }
  mathfu::Vector<T, d> copy(expected);
  EXPECT_FALSE(expected != copy);

  mathfu::Vector<T, d> close(expected - static_cast<T>(1));
  EXPECT_TRUE(expected != close);
}
TEST_ALL_F(NotEqual)
TEST_ALL_INTS_F(NotEqual)

// This will test the == operator for VectorPacked.
template <class T, int d>
void PackedEqual_Test(const T& precision) {
  mathfu::VectorPacked<T, d> a;
  mathfu::VectorPacked<T, d> b;
  for (int i = 0; i < d; ++i) {
    a.data_[i] = static_cast<T>(i * precision);
    b.data_[i] = static_cast<T>(i * precision);
  }
  EXPECT_TRUE(a == b);

  // Changing one element should make them unequal.
  b.data_[0] = static_cast<T>(b.data_[0] + 1);
  EXPECT_FALSE(a == b);
}
TEST_ALL_F(PackedEqual)
TEST_ALL_INTS_F(PackedEqual)

// This will test the != operator for VectorPacked.
template <class T, int d>
void PackedNotEqual_Test(const T& precision) {
  mathfu::VectorPacked<T, d> a;
  mathfu::VectorPacked<T, d> b;
  for (int i = 0; i < d; ++i) {
    a.data_[i] = static_cast<T>(i * precision);
    b.data_[i] = static_cast<T>(i * precision);
  }
  EXPECT_FALSE(a != b);

  // Changing one element should make them unequal.
  b.data_[0] = static_cast<T>(b.data_[0] + 1);
  EXPECT_TRUE(a != b);
}
TEST_ALL_F(PackedNotEqual)
TEST_ALL_INTS_F(PackedNotEqual)

// Simple class that represents a possible compatible type for a vector.
// That is, it's just an array of T of length d, so can be loaded and
// stored from mathfu::Vector<T,d> using ToType() and FromType().
template <class T, int d>
struct SimpleVector {
  T values[d];
};

// This will test the FromType() conversion functions.
template <class T, int d>
void FromType_Test(const T& precision) {
  SimpleVector<T, d> compatible;
  for (int i = 0; i < d; ++i) {
    compatible.values[i] = static_cast<T>(i * precision);
  }

  const mathfu::Vector<T, d> vector =
      mathfu::Vector<T, d>::FromType(compatible);

  for (int i = 0; i < d; ++i) {
    EXPECT_EQ(compatible.values[i], vector[i]);
  }
}
TEST_ALL_F(FromType)
TEST_ALL_INTS_F(FromType)

// This will test the ToType() conversion functions.
template <class T, int d>
void ToType_Test(const T& precision) {
  typedef SimpleVector<T, d> CompatibleT;
  typedef mathfu::Vector<T, d> VectorT;

  VectorT vector;
  for (int i = 0; i < d; ++i) {
    vector[i] = static_cast<T>(i * precision);
  }

  const CompatibleT compatible = VectorT::template ToType<CompatibleT>(vector);

  for (int i = 0; i < d; ++i) {
    EXPECT_EQ(compatible.values[i], vector[i]);
  }
}
TEST_ALL_F(ToType)
TEST_ALL_INTS_F(ToType)

// This will test a roundtrip through FromType() and ToType().
// Converts SimpleVector -> Vector -> SimpleVector and verifies the values
// are preserved. This exercises the memcpy-based type-punning path.
template <class T, int d>
void FromTypeToTypeRoundtrip_Test(const T& precision) {
  typedef SimpleVector<T, d> CompatibleT;
  typedef mathfu::Vector<T, d> VectorT;

  CompatibleT original;
  for (int i = 0; i < d; ++i) {
    original.values[i] = static_cast<T>(i * precision + static_cast<T>(1));
  }

  // SimpleVector -> Vector -> SimpleVector
  const VectorT vector = VectorT::FromType(original);
  const CompatibleT roundtripped =
      VectorT::template ToType<CompatibleT>(vector);

  for (int i = 0; i < d; ++i) {
    EXPECT_EQ(original.values[i], roundtripped.values[i]);
  }
}
TEST_ALL_F(FromTypeToTypeRoundtrip)
TEST_ALL_INTS_F(FromTypeToTypeRoundtrip)

// Test output stream operator.
template <class T, int d>
void OutputStream_Test(const T&) {
  mathfu::Vector<T, d> vector;
  for (int i = 0; i < d; ++i) {
    vector[i] = static_cast<T>(i);
  }

  std::stringstream ss;
  ss << vector;

  switch (d) {
    case 1:
      EXPECT_EQ("(0)", ss.str());
      break;
    case 2:
      EXPECT_EQ("(0, 1)", ss.str());
      break;
    case 3:
      EXPECT_EQ("(0, 1, 2)", ss.str());
      break;
    case 4:
      EXPECT_EQ("(0, 1, 2, 3)", ss.str());
      break;
    case 5:
      EXPECT_EQ("(0, 1, 2, 3, 4)", ss.str());
      break;
  }
}
TEST_ALL_F(OutputStream)
TEST_ALL_INTS_F(OutputStream)
TEST_F(VectorTests, OutputStream_Test_float_1) {
  OutputStream_Test<float, 1>(0.0f);
}

// Test that the kDims static member is present and correct for each
// specialization.
TEST_F(VectorTests, kDims) {
  EXPECT_EQ((mathfu::Vector<float, 2>::kDims), 2);
  EXPECT_EQ((mathfu::Vector<float, 3>::kDims), 3);
  EXPECT_EQ((mathfu::Vector<float, 4>::kDims), 4);
}

// Test that the SIMD padding lane (w / data_[3]) of Vector<float,3> is
// consistently zero after construction and arithmetic operations.
#if defined(MATHFU_COMPILE_WITH_PADDING)
TEST_F(VectorTests, PaddingLaneZeroed_Constructors) {
  // Constructor from three floats.
  mathfu::Vector<float, 3> v3(1.0f, 2.0f, 3.0f);
  EXPECT_EQ(0.0f, v3.data_[3]);

  // Splat constructor.
  mathfu::Vector<float, 3> vs(5.0f);
  EXPECT_EQ(0.0f, vs.data_[3]);

  // Constructor from float array.
  float arr[] = {4.0f, 5.0f, 6.0f};
  mathfu::Vector<float, 3> va(arr);
  EXPECT_EQ(0.0f, va.data_[3]);

  // Constructor from Vector<float,2> + float.
  mathfu::Vector<float, 2> v2(1.0f, 2.0f);
  mathfu::Vector<float, 3> v2f(v2, 3.0f);
  EXPECT_EQ(0.0f, v2f.data_[3]);

  // Copy constructor.
  mathfu::Vector<float, 3> vc(v3);
  EXPECT_EQ(0.0f, vc.data_[3]);

  // Constructor from VectorPacked.
  mathfu::VectorPacked<float, 3> packed;
  packed.data_[0] = 7.0f;
  packed.data_[1] = 8.0f;
  packed.data_[2] = 9.0f;
  mathfu::Vector<float, 3> vp(packed);
  EXPECT_EQ(0.0f, vp.data_[3]);

  // Constructor from integer vector.
  mathfu::Vector<int, 3> vi(1, 2, 3);
  mathfu::Vector<float, 3> vfi(vi);
  EXPECT_EQ(0.0f, vfi.data_[3]);
}

TEST_F(VectorTests, PaddingLaneZeroed_Arithmetic) {
  mathfu::Vector<float, 3> a(1.0f, 2.0f, 3.0f);
  mathfu::Vector<float, 3> b(4.0f, 5.0f, 6.0f);

  // Negation.
  mathfu::Vector<float, 3> neg = -a;
  EXPECT_EQ(0.0f, neg.data_[3]);

  // Vector + Vector.
  mathfu::Vector<float, 3> sum = a + b;
  EXPECT_EQ(0.0f, sum.data_[3]);

  // Vector - Vector.
  mathfu::Vector<float, 3> diff = a - b;
  EXPECT_EQ(0.0f, diff.data_[3]);

  // Vector * Vector (Hadamard).
  mathfu::Vector<float, 3> prod = a * b;
  EXPECT_EQ(0.0f, prod.data_[3]);

  // Vector / Vector.
  mathfu::Vector<float, 3> quot = a / b;
  EXPECT_EQ(0.0f, quot.data_[3]);

  // Vector + scalar.
  mathfu::Vector<float, 3> add_s = a + 10.0f;
  EXPECT_EQ(0.0f, add_s.data_[3]);

  // scalar + Vector.
  mathfu::Vector<float, 3> s_add = 10.0f + a;
  EXPECT_EQ(0.0f, s_add.data_[3]);

  // Vector - scalar.
  mathfu::Vector<float, 3> sub_s = a - 10.0f;
  EXPECT_EQ(0.0f, sub_s.data_[3]);

  // scalar - Vector.
  mathfu::Vector<float, 3> s_sub = 10.0f - a;
  EXPECT_EQ(0.0f, s_sub.data_[3]);

  // Vector * scalar.
  mathfu::Vector<float, 3> mul_s = a * 2.0f;
  EXPECT_EQ(0.0f, mul_s.data_[3]);

  // scalar * Vector.
  mathfu::Vector<float, 3> s_mul = 2.0f * a;
  EXPECT_EQ(0.0f, s_mul.data_[3]);

  // Vector / scalar.
  mathfu::Vector<float, 3> div_s = a / 2.0f;
  EXPECT_EQ(0.0f, div_s.data_[3]);
}

TEST_F(VectorTests, PaddingLaneZeroed_CompoundAssignment) {
  mathfu::Vector<float, 3> b(4.0f, 5.0f, 6.0f);

  // +=Vector
  mathfu::Vector<float, 3> v1(1.0f, 2.0f, 3.0f);
  v1 += b;
  EXPECT_EQ(0.0f, v1.data_[3]);

  // -=Vector
  mathfu::Vector<float, 3> v2(1.0f, 2.0f, 3.0f);
  v2 -= b;
  EXPECT_EQ(0.0f, v2.data_[3]);

  // *=Vector
  mathfu::Vector<float, 3> v3(1.0f, 2.0f, 3.0f);
  v3 *= b;
  EXPECT_EQ(0.0f, v3.data_[3]);

  // /=Vector
  mathfu::Vector<float, 3> v4(1.0f, 2.0f, 3.0f);
  v4 /= b;
  EXPECT_EQ(0.0f, v4.data_[3]);

  // +=scalar
  mathfu::Vector<float, 3> v5(1.0f, 2.0f, 3.0f);
  v5 += 10.0f;
  EXPECT_EQ(0.0f, v5.data_[3]);

  // -=scalar
  mathfu::Vector<float, 3> v6(1.0f, 2.0f, 3.0f);
  v6 -= 10.0f;
  EXPECT_EQ(0.0f, v6.data_[3]);

  // *=scalar
  mathfu::Vector<float, 3> v7(1.0f, 2.0f, 3.0f);
  v7 *= 2.0f;
  EXPECT_EQ(0.0f, v7.data_[3]);

  // /=scalar
  mathfu::Vector<float, 3> v8(1.0f, 2.0f, 3.0f);
  v8 /= 2.0f;
  EXPECT_EQ(0.0f, v8.data_[3]);
}

TEST_F(VectorTests, PaddingLaneZeroed_VectorOps) {
  mathfu::Vector<float, 3> a(1.0f, 2.0f, 3.0f);
  mathfu::Vector<float, 3> b(4.0f, 5.0f, 6.0f);

  // CrossProduct.
  mathfu::Vector<float, 3> cross = mathfu::Vector<float, 3>::CrossProduct(a, b);
  EXPECT_EQ(0.0f, cross.data_[3]);

  // Normalized.
  mathfu::Vector<float, 3> normd = a.Normalized();
  EXPECT_EQ(0.0f, normd.data_[3]);

  // Normalize (in-place).
  mathfu::Vector<float, 3> norm_in_place(a);
  norm_in_place.Normalize();
  EXPECT_EQ(0.0f, norm_in_place.data_[3]);

  // HadamardProduct.
  mathfu::Vector<float, 3> hadamard =
      mathfu::Vector<float, 3>::HadamardProduct(a, b);
  EXPECT_EQ(0.0f, hadamard.data_[3]);

  // Lerp.
  mathfu::Vector<float, 3> lerp = mathfu::Vector<float, 3>::Lerp(a, b, 0.5f);
  EXPECT_EQ(0.0f, lerp.data_[3]);

  // Max.
  mathfu::Vector<float, 3> max_v = mathfu::Vector<float, 3>::Max(a, b);
  EXPECT_EQ(0.0f, max_v.data_[3]);

  // Min.
  mathfu::Vector<float, 3> min_v = mathfu::Vector<float, 3>::Min(a, b);
  EXPECT_EQ(0.0f, min_v.data_[3]);
}
#endif  // defined(MATHFU_COMPILE_WITH_PADDING)

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHFU_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

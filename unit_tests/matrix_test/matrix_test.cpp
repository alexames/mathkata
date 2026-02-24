/*
 * Copyright 2014 Google Inc. All rights reserved.
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
#include "mathkata/matrix.h"

#include <cmath>
#include <numbers>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>

#include "gtest/gtest.h"
#include "mathkata/io.h"
#include "mathkata/quaternion.h"
#include "mathkata/utilities.h"
#include "mathkata/vector.h"
#include "precision.h"

// Thread-local random engine seeded deterministically for reproducible tests.
static std::mt19937& TestRng() {
  static std::mt19937 rng(42);
  return rng;
}

// Generate a random value in [0, 1) for floating point types.
template <class T>
T TestRandom01() {
  std::uniform_real_distribution<T> dist(static_cast<T>(0), static_cast<T>(1));
  return dist(TestRng());
}

// Generate a random value in [-range, +range] for floating point types.
template <class T>
T TestRandomRange(T range) {
  return (TestRandom01<T>() * range * static_cast<T>(2)) - range;
}
static const float kUnProjectFloatPrecision = 0.0012f;
static const double kLookAtDoublePrecision = 1e-8;
class MatrixTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

template <class T, int rows, int columns>
struct MatrixExpectation {
  const char* description;
  mathkata::Matrix<T, rows, columns> calculated;
  mathkata::Matrix<T, rows, columns> expected;
};

// This will automatically generate tests for each template parameter.
#define TEST_ALL_F(MY_TEST, FLOAT_PRECISION_VALUE, DOUBLE_PRECISION_VALUE) \
  TEST_F(MatrixTests, MY_TEST##_float_2) {                                 \
    MY_TEST##_Test<float, 2>(FLOAT_PRECISION_VALUE);                       \
  }                                                                        \
  TEST_F(MatrixTests, MY_TEST##_double_2) {                                \
    MY_TEST##_Test<double, 2>(DOUBLE_PRECISION_VALUE);                     \
  }                                                                        \
  TEST_F(MatrixTests, MY_TEST##_float_3) {                                 \
    MY_TEST##_Test<float, 3>(FLOAT_PRECISION_VALUE);                       \
  }                                                                        \
  TEST_F(MatrixTests, MY_TEST##_double_3) {                                \
    MY_TEST##_Test<double, 3>(DOUBLE_PRECISION_VALUE);                     \
  }                                                                        \
  TEST_F(MatrixTests, MY_TEST##_float_4) {                                 \
    MY_TEST##_Test<float, 4>(FLOAT_PRECISION_VALUE);                       \
  }                                                                        \
  TEST_F(MatrixTests, MY_TEST##_double_4) {                                \
    MY_TEST##_Test<double, 4>(DOUBLE_PRECISION_VALUE);                     \
  }

// This will automatically generate tests for each scalar template parameter.
#define TEST_SCALAR_F(MY_TEST, FLOAT_PRECISION_VALUE, DOUBLE_PRECISION_VALUE) \
  TEST_F(MatrixTests, MY_TEST##_float) {                                      \
    MY_TEST##_Test<float>(FLOAT_PRECISION_VALUE);                             \
  }                                                                           \
  TEST_F(MatrixTests, MY_TEST##_double) {                                     \
    MY_TEST##_Test<double>(DOUBLE_PRECISION_VALUE);                           \
  }

// This will test initialization by passing in values. The template paramter d
// corresponds to the number of rows and columns.
template <class T, int d>
void Initialize_Test(const T& precision) {
  // This will test initialization of the matrix using a random single value.
  // The expected result is that all entries equal the given value.
  mathkata::Matrix<T, d> matrix_splat(static_cast<T>(3.1));
  for (int i = 0; i < d * d; ++i) {
    EXPECT_NEAR(3.1, matrix_splat[i], precision);
  }
  // This will verify that the value is correct when using the (i, j) form
  // of indexing.
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(3.1, matrix_splat(i, j), precision);
    }
  }
  // This will test initialization of the matrix using a c style array of
  // values.
  T x[d * d];
  for (int i = 0; i < d * d; ++i) {
    x[i] = TestRandom01<T>() * static_cast<T>(100);
  }
  mathkata::Matrix<T, d> matrix_arr(x);
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(x[i + d * j], matrix_arr(i, j), precision);
    }
  }
  // This will test the copy constructor making sure that the new matrix
  // equals the old one.
  mathkata::Matrix<T, d> matrix_copy(matrix_arr);
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(x[i + d * j], matrix_copy(i, j), precision);
    }
  }
  // This will verify that the copy was deep and changing the values of the
  // copied matrix does not effect the original.
  matrix_copy = matrix_copy - mathkata::Matrix<T, d>(1);
  EXPECT_NE(matrix_copy(0, 0), matrix_arr(0, 0));
  // This will test creation of the identity matrix.
  mathkata::Matrix<T, d> identity(mathkata::Matrix<T, d>::identity());
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(i == j ? 1 : 0, identity(i, j), precision);
    }
  }
}
TEST_ALL_F(Initialize, FLOAT_PRECISION, DOUBLE_PRECISION)

// This will test initialization by specifying all values explicitly.
template <class T>
void InitializePerDimension_Test(const T& precision) {
  mathkata::Matrix<T, 2> matrix_f2x2(static_cast<T>(4.5), static_cast<T>(3.4),
                                     static_cast<T>(2.6), static_cast<T>(9.8));
  EXPECT_NEAR(4.5, matrix_f2x2(0, 0), precision);
  EXPECT_NEAR(3.4, matrix_f2x2(1, 0), precision);
  EXPECT_NEAR(2.6, matrix_f2x2(0, 1), precision);
  EXPECT_NEAR(9.8, matrix_f2x2(1, 1), precision);
  mathkata::Matrix<T, 3> matrix_f3x3(
      static_cast<T>(3.7), static_cast<T>(2.4), static_cast<T>(6.4),
      static_cast<T>(1.1), static_cast<T>(5.2), static_cast<T>(6.4),
      static_cast<T>(2.7), static_cast<T>(7.4), static_cast<T>(0.1));
  EXPECT_NEAR(3.7, matrix_f3x3(0, 0), precision);
  EXPECT_NEAR(2.4, matrix_f3x3(1, 0), precision);
  EXPECT_NEAR(6.4, matrix_f3x3(2, 0), precision);
  EXPECT_NEAR(1.1, matrix_f3x3(0, 1), precision);
  EXPECT_NEAR(5.2, matrix_f3x3(1, 1), precision);
  EXPECT_NEAR(6.4, matrix_f3x3(2, 1), precision);
  EXPECT_NEAR(2.7, matrix_f3x3(0, 2), precision);
  EXPECT_NEAR(7.4, matrix_f3x3(1, 2), precision);
  EXPECT_NEAR(0.1, matrix_f3x3(2, 2), precision);
  mathkata::Matrix<T, 4> matrix_f4x4(
      static_cast<T>(4.1), static_cast<T>(8.4), static_cast<T>(7.2),
      static_cast<T>(4.8), static_cast<T>(0.9), static_cast<T>(7.8),
      static_cast<T>(5.6), static_cast<T>(8.7), static_cast<T>(2.3),
      static_cast<T>(4.2), static_cast<T>(6.1), static_cast<T>(2.7),
      static_cast<T>(0.1), static_cast<T>(1.4), static_cast<T>(9.4),
      static_cast<T>(3.6));
  EXPECT_NEAR(4.1, matrix_f4x4(0, 0), precision);
  EXPECT_NEAR(8.4, matrix_f4x4(1, 0), precision);
  EXPECT_NEAR(7.2, matrix_f4x4(2, 0), precision);
  EXPECT_NEAR(4.8, matrix_f4x4(3, 0), precision);
  EXPECT_NEAR(0.9, matrix_f4x4(0, 1), precision);
  EXPECT_NEAR(7.8, matrix_f4x4(1, 1), precision);
  EXPECT_NEAR(5.6, matrix_f4x4(2, 1), precision);
  EXPECT_NEAR(8.7, matrix_f4x4(3, 1), precision);
  EXPECT_NEAR(2.3, matrix_f4x4(0, 2), precision);
  EXPECT_NEAR(4.2, matrix_f4x4(1, 2), precision);
  EXPECT_NEAR(6.1, matrix_f4x4(2, 2), precision);
  EXPECT_NEAR(2.7, matrix_f4x4(3, 2), precision);
  EXPECT_NEAR(0.1, matrix_f4x4(0, 3), precision);
  EXPECT_NEAR(1.4, matrix_f4x4(1, 3), precision);
  EXPECT_NEAR(9.4, matrix_f4x4(2, 3), precision);
  EXPECT_NEAR(3.6, matrix_f4x4(3, 3), precision);
}
TEST_SCALAR_F(InitializePerDimension, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test initialization of a matrix from an array of packed vectors.
template <class T, int d>
void InitializePacked_Test(const T& precision) {
  (void)precision;
  mathkata::VectorPacked<T, d> packed[d];
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      packed[i].data_[j] = static_cast<T>((i * d) + j);
    }
  }
  mathkata::Matrix<T, d> matrix(packed);
  for (int i = 0; i < d * d; ++i) {
    EXPECT_NEAR(packed[i / d].data_[i % d], matrix[i], static_cast<T>(0))
        << "Element " << i;
  }
}
TEST_ALL_F(InitializePacked, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test serialization to a packed array of vectors.
template <class T, int d>
void PackedSerialization_Test(const T& precision) {
  (void)precision;
  mathkata::Matrix<T, d> matrix;
  for (int i = 0; i < d * d; ++i) {
    matrix[i] = static_cast<T>(i);
  }
  mathkata::VectorPacked<T, d> packed[d];
  matrix.pack(packed);
  for (int i = 0; i < d * d; ++i) {
    EXPECT_NEAR(matrix[i], packed[i / d].data_[i % d], static_cast<T>(0))
        << "Element " << i;
  }
}
TEST_ALL_F(PackedSerialization, FLOAT_PRECISION, DOUBLE_PRECISION)

// This will test the Addition and Subtraction of matrices. The template
// parameter d corresponds to the number of rows and columns.
template <class T, int d>
void AddSub_Test(const T& precision) {
  T x1[d * d], x2[d * d];
  for (int i = 0; i < d * d; ++i) {
    x1[i] = TestRandom01<T>() * static_cast<T>(100);
  }
  for (int i = 0; i < d * d; ++i) {
    x2[i] = TestRandom01<T>() * static_cast<T>(100);
  }
  mathkata::Matrix<T, d> matrix1(x1), matrix2(x2);
  // This will test the negation of a matrix and verify that each element
  // is negated.
  mathkata::Matrix<T, d> neg_mat1(-matrix1);
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(-x1[i + d * j], neg_mat1(i, j), precision);
    }
  }
  // This will test the addition of two matrices and verify that each element
  // equal to the sum of the input values.
  mathkata::Matrix<T, d> matrix_add(matrix1 + matrix2);
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(x1[i + d * j] + x2[i + d * j], matrix_add(i, j), precision);
    }
  }
  // This will test the subtraction of two matrices and verify that each
  // element equal to the difference of the input values.
  mathkata::Matrix<T, d> matrix_sub(matrix1 - matrix2);
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(x1[i + d * j] - x2[i + d * j], matrix_sub(i, j), precision);
    }
  }
}
TEST_ALL_F(AddSub, FLOAT_PRECISION, DOUBLE_PRECISION)

// This will test the multiplication of matrices by matrices, vectors,
// and scalars. The template parameter d corresponds to the number of rows and
// columns.
template <class T, int d>
void Mult_Test(const T& precision) {
  T x1[d * d], x2[d * d];
  for (int i = 0; i < d * d; ++i) x1[i] = TestRandom01<T>();
  for (int i = 0; i < d * d; ++i) x2[i] = TestRandom01<T>();
  mathkata::Matrix<T, d> matrix1(x1), matrix2(x2);
  // This will test scalar matrix multiplication and verify that each element
  // is equal to multiplication by the scalar.
  mathkata::Matrix<T, d> matrix_mults(matrix1 * static_cast<T>(1.1));
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(x1[i + d * j] * 1.1, matrix_mults(i, j), precision);
    }
  }
  T v[d];
  for (int i = 0; i < d; ++i) v[i] = TestRandom01<T>();
  mathkata::Vector<T, d> vector(v);
  // This will test matrix vector multiplication and verify that the resulting
  // vector is mathematically correct.
  mathkata::Vector<T, d> vector_multv(matrix1 * vector);
  for (int i = 0; i < d; ++i) {
    T v1[d];
    for (int k = 0; k < d; ++k) v1[k] = matrix1(i, k);
    mathkata::Vector<T, d> vec1(v1);
    T dot = mathkata::Vector<T, d>::dotProduct(vec1, vector);
    EXPECT_NEAR(dot, vector_multv[i], precision);
  }
  // This will test matrix multiplication and verify that the resulting
  // matrix is mathematically correct.
  mathkata::Matrix<T, d> matrix_multm(matrix1 * matrix2);
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      T v1[d], v2[d];
      for (int k = 0; k < d; ++k) v1[k] = matrix1(i, k);
      for (int k = 0; k < d; ++k) v2[k] = matrix2(k, j);
      mathkata::Vector<T, d> vec1(v1), vec2(v2);
      T dot = mathkata::Vector<T, d>::dotProduct(vec1, vec2);
      EXPECT_NEAR(dot, matrix_multm(i, j), precision);
    }
  }
}
TEST_ALL_F(Mult, FLOAT_PRECISION, DOUBLE_PRECISION)
TEST_F(MatrixTests, Mult_float_5) { Mult_Test<float, 5>(FLOAT_PRECISION); }
TEST_F(MatrixTests, Mult_double_5) { Mult_Test<double, 5>(DOUBLE_PRECISION); }

// This will test the outer product of two vectors. The template parameter d
// corresponds to the number of rows and columns.
template <class T, int d>
void outerProduct_Test(const T& precision) {
  T x1[d], x2[d];
  for (int i = 0; i < d; ++i) x1[i] = TestRandom01<T>();
  for (int i = 0; i < d; ++i) x2[i] = TestRandom01<T>();
  mathkata::Vector<T, d> vector1(x1), vector2(x2);
  mathkata::Matrix<T, d> matrix(
      mathkata::Matrix<T, d>::outerProduct(vector1, vector2));
  // This will verify that each element is mathematically correct.
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(vector1[i] * vector2[j], matrix(i, j), precision);
    }
  }
}
TEST_ALL_F(outerProduct, FLOAT_PRECISION, DOUBLE_PRECISION)

// Print the specified matrix to output_string in the form.
template <class T, int rows, int columns>
std::string MatrixToString(const mathkata::Matrix<T, rows, columns>& matrix) {
  std::stringstream ss;
  ss.flags(std::ios::fixed);
  ss.precision(4);
  ss << matrix;
  return ss.str();
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127)  // conditional expression is constant
#endif                           // _MSC_VER

// Test the inverse of a set of noninvertible matrices.
template <class T, int d>
void InverseNonInvertible_Test(const T& precision) {
  (void)precision;
  T m[d * d];
  const size_t matrix_size = sizeof(m) / sizeof(m[0]);
  static const T kDeterminantThreshold =
      mathkata::Constants<T>::getDeterminantThreshold();
  static const T kDeterminantThresholdSmall = kDeterminantThreshold / 100;
  static const T kDeterminantThresholdLarge = kDeterminantThreshold * 100;
  static const T kDeterminantThresholdInverse = 1 / kDeterminantThreshold;
  static const T kDeterminantThresholdInverseSmall =
      kDeterminantThresholdInverse / 100;
  static const T kDeterminantThresholdInverseLarge =
      kDeterminantThresholdInverse * 100;
  // Create a matrix with all zeros.
  for (size_t i = 0; i < matrix_size; ++i) m[i] = 0;
  // Verify that it's not possible to invert the matrix.
  {
    mathkata::Matrix<T, d> matrix(m);
    mathkata::Matrix<T, d> inverse_matrix;
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(&inverse_matrix));
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(
        &inverse_matrix, kDeterminantThresholdSmall));
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(
        &inverse_matrix, kDeterminantThresholdLarge));
  }
  // Check a matrix with all elements at the determinant threshold.
  for (size_t i = 0; i < matrix_size; ++i) m[i] = kDeterminantThreshold;
  {
    mathkata::Matrix<T, d> matrix(m);
    mathkata::Matrix<T, d> inverse_matrix;
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(&inverse_matrix));
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(
        &inverse_matrix, kDeterminantThresholdSmall));
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(
        &inverse_matrix, kDeterminantThresholdLarge));
  }
  // Check a matrix with all very large elements.
  for (size_t i = 0; i < matrix_size - 1; ++i) {
    m[i] = kDeterminantThresholdInverse;
  }
  m[matrix_size - 1] = kDeterminantThresholdInverseLarge;
  // NOTE: Due to precision, this case will pass the determinant check with a
  // 2x2 matrix since the determinant of the matrix will be calculated as 1.
  if (d != 2) {
    // Create a matrix with all elements at the determinant threshold and one
    // large value in the matrix.
    for (size_t i = 0; i < matrix_size - 1; ++i) {
      m[i] = kDeterminantThresholdInverseSmall;
    }
    m[matrix_size - 1] = kDeterminantThresholdInverseLarge;
    {
      mathkata::Matrix<T, d> matrix(m);
      mathkata::Matrix<T, d> inverse_matrix;
      EXPECT_FALSE(matrix.inverseWithDeterminantCheck(&inverse_matrix));
      EXPECT_FALSE(matrix.inverseWithDeterminantCheck(
          &inverse_matrix, kDeterminantThresholdSmall));
      EXPECT_FALSE(matrix.inverseWithDeterminantCheck(
          &inverse_matrix, kDeterminantThresholdLarge));
    }
  }
}
TEST_ALL_F(InverseNonInvertible, FLOAT_PRECISION, DOUBLE_PRECISION)

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

// Test that matrices with small scale pass or fail the determinant check based
// on the threshold.
template <class T, int d>
void InverseSmallScale_Test(const T& precision) {
  (void)precision;
  static const T kDeterminantThreshold =
      mathkata::Constants<T>::getDeterminantThreshold();
  static const T kDeterminantThresholdSmall = kDeterminantThreshold / 100;
  static const T kDeterminantThresholdLarge = kDeterminantThreshold * 100;

  // The scale of the determinant grows with the square for 2x2 matrices, and
  // with the cube for both 3x3 and 4x4 matrices.
  static const T kDeterminantPower = static_cast<T>(1) / (d == 2 ? 2 : 3);
  static const T kScaleMin = pow(kDeterminantThreshold, kDeterminantPower);

  mathkata::Matrix<T, d> matrix = mathkata::Matrix<T, d>::identity();
  mathkata::Matrix<T, d> inverse_matrix;

  // scale too small - non-invertible.
  {
    for (int i = 0; i != d; ++i) matrix(i, i) = kScaleMin / 2;
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(&inverse_matrix));
    EXPECT_TRUE(matrix.inverseWithDeterminantCheck(&inverse_matrix,
                                                   kDeterminantThresholdSmall));
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(
        &inverse_matrix, kDeterminantThresholdLarge));
  }

  // scale large enough - invertible.
  {
    for (int i = 0; i != d; ++i) matrix(i, i) = kScaleMin * 2;
    EXPECT_TRUE(matrix.inverseWithDeterminantCheck(&inverse_matrix));
    EXPECT_TRUE(matrix.inverseWithDeterminantCheck(&inverse_matrix,
                                                   kDeterminantThresholdSmall));
    EXPECT_FALSE(matrix.inverseWithDeterminantCheck(
        &inverse_matrix, kDeterminantThresholdLarge));
  }
}
TEST_ALL_F(InverseSmallScale, FLOAT_PRECISION, DOUBLE_PRECISION)

// This will test calculating the inverse of a matrix. The template parameter d
// corresponds to the number of rows and columns.
template <class T, int d>
void inverse_Test(const T& precision) {
  T x[d * d];
  for (int iterations = 0; iterations < 1000; ++iterations) {
    // NOTE: This assumes that matrices generated here are invertible since
    // there is a tiny probability that a randomly generated matrix will be
    // noninvertible.  This does mean that this test can be flakey by
    // occasionally generating noninvertible matrices.
    for (int i = 0; i < mathkata::Matrix<T, d>::kElements; ++i) {
      x[i] = TestRandomRange<T>(1);
    }
    mathkata::Matrix<T, d> matrix(x);
    std::string error_string = MatrixToString(matrix);
    error_string += "\n";
    mathkata::Matrix<T, d> inverse_matrix(matrix.inverse());
    mathkata::Matrix<T, d> identity_matrix(matrix * inverse_matrix);

    error_string += MatrixToString(inverse_matrix);
    error_string += "\n";
    error_string += MatrixToString(identity_matrix);

    // This will verify that M * Minv is equal to the identity.
    for (int i = 0; i < d; ++i) {
      for (int j = 0; j < d; ++j) {
        EXPECT_NEAR(i == j ? 1 : 0, identity_matrix(i, j), 100 * precision)
            << error_string << " row=" << i << " column=" << j;
      }
    }
  }
}
// Due to the number of operations involved and the random numbers used to
// generate the test matrices, the precision the inverse matrix is calculated
// to is relatively low.
TEST_ALL_F(inverse, 1e-4f, 1e-8)

// This will test converting from a translation into a matrix and back again.
template <class T>
void translationVector3D_Test(const T& precision) {
  (void)precision;
  const mathkata::Vector<T, 3> trans(
      static_cast<T>(-100.0), static_cast<T>(0.0), static_cast<T>(0.00003));
  const mathkata::Matrix<T, 4> trans_matrix =
      mathkata::Matrix<T, 4>::fromTranslationVector(trans);
  const mathkata::Vector<T, 3> trans_back = trans_matrix.translationVector3D();

  // This will verify that the translation vector has not changed.
  for (int i = 0; i < 3; ++i) {
    EXPECT_EQ(trans[i], trans_back[i]);
  }
}
TEST_SCALAR_F(translationVector3D, FLOAT_PRECISION, DOUBLE_PRECISION)

// This will test converting from a translation into a matrix and back again.
template <class T>
void translationVector2D_Test(const T& precision) {
  (void)precision;
  const mathkata::Vector<T, 2> trans(static_cast<T>(-100.0),
                                     static_cast<T>(0.00003));
  const mathkata::Matrix<T, 3> trans_matrix =
      mathkata::Matrix<T, 3>::fromTranslationVector(trans);
  const mathkata::Vector<T, 2> trans_back = trans_matrix.translationVector2D();

  // This will verify that the translation vector has not changed.
  for (int i = 0; i < 2; ++i) {
    EXPECT_EQ(trans[i], trans_back[i]);
  }
}
TEST_SCALAR_F(translationVector2D, FLOAT_PRECISION, DOUBLE_PRECISION)

// This will test converting from a scale into a matrix, then multiply by
// a vector of 1's, which should produce the original scale again.
template <class T, int d>
void fromScaleVector_Test(const T& precision) {
  mathkata::Vector<T, d> ones(static_cast<T>(1));
  mathkata::Vector<T, d - 1> v;

  // Tests that the scale vector is placed in the correct order in the matrix.
  for (int i = 0; i < d - 1; ++i) {
    v[i] = static_cast<T>(i + 10);
  }
  mathkata::Matrix<T, d> m = mathkata::Matrix<T, d>::fromScaleVector(v);

  // Ensure that the v is on the diagonal.
  for (int i = 0; i < d - 1; ++i) {
    EXPECT_NEAR(v[i], m(i, i), precision);
  }

  // Ensure that the last diagonal element is one.
  EXPECT_NEAR(m(d - 1, d - 1), 1, precision);

  // Ensure that all non-diagonal elements are zero.
  for (int i = 0; i < d - 1; ++i) {
    for (int j = 0; j < d - 1; ++j) {
      if (i == j) continue;
      EXPECT_NEAR(m(i, j), 0, precision);
    }
  }
}
// Precision is zero. Results must be perfect for this test.
TEST_ALL_F(fromScaleVector, 0.0f, 0.0)

// Compare a set of Matrix<T, rows, columns> with expected values.
template <class T, int rows, int columns>
void VerifyMatrixExpectations(
    const MatrixExpectation<T, rows, columns>* test_cases,
    const size_t number_of_test_cases, const T& precision) {
  for (size_t i = 0; i < number_of_test_cases; ++i) {
    const MatrixExpectation<T, rows, columns>& test = test_cases[i];
    for (int j = 0; j < mathkata::Matrix<T, rows, columns>::kElements; ++j) {
      const mathkata::Matrix<T, rows, columns>& calculated = test.calculated;
      const mathkata::Matrix<T, rows, columns>& expected = test.expected;
      EXPECT_NEAR(calculated[j], expected[j], precision)
          << "element " << j << " (" << (j / columns) << ", " << (j % columns)
          << "), case " << test.description << "\n"
          << MatrixToString(calculated) << "vs expected\n"
          << MatrixToString(expected);
    }
  }
}

// Test perspective matrix calculation.
template <class T>
void perspective_Test(const T& precision) {
  using RH = std::integral_constant<mathkata::Handedness,
                                    mathkata::Handedness::kRightHanded>;
  using LH = std::integral_constant<mathkata::Handedness,
                                    mathkata::Handedness::kLeftHanded>;
  // clang-format off
  const MatrixExpectation<T, 4, 4> kTestCasesRH[] = {
    {
      "normalized right-handed",
      mathkata::Matrix<T, 4>::template perspective<RH::value>(
          atan(static_cast<T>(1)) * 2, 1, 0, 1),
      mathkata::Matrix<T, 4>(1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, -1, -1,
                           0, 0, 0, 0),
    },
    {
      "widefov",
      mathkata::Matrix<T, 4>::perspective(
          atan(static_cast<T>(2)) * 2, 1, 0, 1),
      mathkata::Matrix<T, 4>(0.5, 0, 0, 0,
                           0, 0.5, 0, 0,
                           0, 0, -1, -1,
                           0, 0, 0, 0),
    },
    {
      "narrowfov",
      mathkata::Matrix<T, 4>::perspective(
          atan(static_cast<T>(0.1)) * 2, 1, 0, 1),
      mathkata::Matrix<T, 4>(10, 0, 0, 0,
                           0, 10, 0, 0,
                           0, 0, -1, -1,
                           0, 0, 0, 0),
    },
    {
      "2:1 aspect ratio",
      mathkata::Matrix<T, 4>::perspective(
          atan(static_cast<T>(1)) * 2, 0.5, 0, 1),
      mathkata::Matrix<T, 4>(2, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, -1, -1,
                           0, 0, 0, 0),
    },
    {
      "deeper view frustrum",
      mathkata::Matrix<T, 4>::perspective(
          atan(static_cast<T>(1)) * 2, 1, -2, 2),
      mathkata::Matrix<T, 4>(1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 0, -1,
                           0, 0, 2, 0),
    },
  };
  const MatrixExpectation<T, 4, 4> kTestCasesLH[] = {
    {
      "normalized left-handed",
      mathkata::Matrix<T, 4>::template perspective<LH::value>(
          atan(static_cast<T>(1)) * 2, 1, 0, 1),
      mathkata::Matrix<T, 4>(1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 1, 1,
                           0, 0, 0, 0),
    },
  };
  // clang-format on
  VerifyMatrixExpectations(
      kTestCasesRH, sizeof(kTestCasesRH) / sizeof(kTestCasesRH[0]), precision);
  VerifyMatrixExpectations(
      kTestCasesLH, sizeof(kTestCasesLH) / sizeof(kTestCasesLH[0]), precision);
}
TEST_SCALAR_F(perspective, FLOAT_PRECISION, DOUBLE_PRECISION * 10)

// Test that the OpenGL perspective matrix maps the near plane to z = -1
// and the far plane to z = 1.
template <class T>
void PerspectiveOpenGLDepth_Test(const T& precision) {
  const T fovy = std::numbers::pi_v<T> / 4;  // 45 degrees
  const T aspect = static_cast<T>(1.5);
  const T znear = static_cast<T>(0.1);
  const T zfar = static_cast<T>(100.0);

  mathkata::Matrix<T, 4> proj = mathkata::Matrix<T, 4>::perspective(
      fovy, aspect, znear, zfar, mathkata::DepthRange::kOpenGL);

  // transform a point on the near plane (z = -znear in view space for RH).
  mathkata::Vector<T, 4> near_point(0, 0, -znear, 1);
  mathkata::Vector<T, 4> near_clip = proj * near_point;
  T near_ndc_z = near_clip.z / near_clip.w;
  EXPECT_NEAR(near_ndc_z, static_cast<T>(-1), precision);

  // transform a point on the far plane (z = -zfar in view space for RH).
  mathkata::Vector<T, 4> far_point(0, 0, -zfar, 1);
  mathkata::Vector<T, 4> far_clip = proj * far_point;
  T far_ndc_z = far_clip.z / far_clip.w;
  EXPECT_NEAR(far_ndc_z, static_cast<T>(1), precision);
}
TEST_SCALAR_F(PerspectiveOpenGLDepth, 1e-5f, 1e-10)

// Test that the DirectX perspective matrix maps the near plane to z = 0
// and the far plane to z = 1.
template <class T>
void PerspectiveDirectXDepth_Test(const T& precision) {
  const T fovy = std::numbers::pi_v<T> / 4;  // 45 degrees
  const T aspect = static_cast<T>(1.5);
  const T znear = static_cast<T>(0.1);
  const T zfar = static_cast<T>(100.0);

  mathkata::Matrix<T, 4> proj = mathkata::Matrix<T, 4>::perspective(
      fovy, aspect, znear, zfar, mathkata::DepthRange::kDirectX);

  // transform a point on the near plane (z = -znear in view space for RH).
  mathkata::Vector<T, 4> near_point(0, 0, -znear, 1);
  mathkata::Vector<T, 4> near_clip = proj * near_point;
  T near_ndc_z = near_clip.z / near_clip.w;
  EXPECT_NEAR(near_ndc_z, static_cast<T>(0), precision);

  // transform a point on the far plane (z = -zfar in view space for RH).
  mathkata::Vector<T, 4> far_point(0, 0, -zfar, 1);
  mathkata::Vector<T, 4> far_clip = proj * far_point;
  T far_ndc_z = far_clip.z / far_clip.w;
  EXPECT_NEAR(far_ndc_z, static_cast<T>(1), precision);
}
TEST_SCALAR_F(PerspectiveDirectXDepth, 1e-5f, 1e-10)

// Test that the default perspective (no DepthRange argument) uses OpenGL
// convention, matching the explicit OpenGL call.
template <class T>
void PerspectiveDefaultIsOpenGL_Test(const T& precision) {
  const T fovy = std::numbers::pi_v<T> / 3;
  const T aspect = static_cast<T>(1.6);
  const T znear = static_cast<T>(1.0);
  const T zfar = static_cast<T>(500.0);

  mathkata::Matrix<T, 4> default_proj =
      mathkata::Matrix<T, 4>::perspective(fovy, aspect, znear, zfar);
  mathkata::Matrix<T, 4> opengl_proj = mathkata::Matrix<T, 4>::perspective(
      fovy, aspect, znear, zfar, mathkata::DepthRange::kOpenGL);

  for (int i = 0; i < 16; ++i) {
    EXPECT_NEAR(default_proj[i], opengl_proj[i], precision);
  }
}
TEST_SCALAR_F(PerspectiveDefaultIsOpenGL, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test orthographic matrix calculation.
template <class T>
void ortho_Test(const T& precision) {
  using LH = std::integral_constant<mathkata::Handedness,
                                    mathkata::Handedness::kLeftHanded>;
  // clang-format off
  const MatrixExpectation<T, 4, 4> kTestCasesRH[] = {
    {
      "normalized",
      mathkata::Matrix<T, 4, 4>::ortho(0, 2, 0, 2, 2, 0),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              -1, -1, 1, 1),
    },
    {
      "normalized RH",
      mathkata::Matrix<T, 4, 4>::ortho(0, 2, 0, 2, 2, 0),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              -1, -1, 1, 1),
    },
    {
      "narrow RH",
      mathkata::Matrix<T, 4, 4>::ortho(1, 3, 0, 2, 2, 0),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              -2, -1, 1, 1),

    },
    {
      "squat RH",
      mathkata::Matrix<T, 4, 4>::ortho(0, 2, 1, 3, 2, 0),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              -1, -2, 1, 1),

    },
    {
      "deep RH",
      mathkata::Matrix<T, 4, 4>::ortho(0, 2, 0, 2, 3, 1),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              -1, -1, 2, 1),

    },
  };
  const MatrixExpectation<T, 4, 4> kTestCasesLH[] = {
    {
      "normalized LH",
      mathkata::Matrix<T, 4, 4>::template ortho<LH::value>(0, 2, 0, 2, 2, 0),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, -1, 0,
                              -1, -1, 1, 1),
    },
    {
      "Canonical LH",
      mathkata::Matrix<T, 4, 4>::template ortho<LH::value>(1, 3, 1, 3, 1, 3),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                             -2,-2,-2, 1),

    },
  };
  // clang-format on
  VerifyMatrixExpectations(
      kTestCasesRH, sizeof(kTestCasesRH) / sizeof(kTestCasesRH[0]), precision);
  VerifyMatrixExpectations(
      kTestCasesLH, sizeof(kTestCasesLH) / sizeof(kTestCasesLH[0]), precision);
}
TEST_SCALAR_F(ortho, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test that the OpenGL orthographic matrix maps the near plane to z = -1
// and the far plane to z = 1.
template <class T>
void OrthoOpenGLDepth_Test(const T& precision) {
  const T left = static_cast<T>(-10);
  const T right = static_cast<T>(10);
  const T bottom = static_cast<T>(-10);
  const T top = static_cast<T>(10);
  const T znear = static_cast<T>(1);
  const T zfar = static_cast<T>(100);

  mathkata::Matrix<T, 4> proj = mathkata::Matrix<T, 4>::ortho(
      left, right, bottom, top, znear, zfar, mathkata::DepthRange::kOpenGL);

  // transform a point on the near plane (z = -znear in view space for RH).
  mathkata::Vector<T, 4> near_point(0, 0, -znear, 1);
  mathkata::Vector<T, 4> near_clip = proj * near_point;
  T near_ndc_z = near_clip.z / near_clip.w;
  EXPECT_NEAR(near_ndc_z, static_cast<T>(-1), precision);

  // transform a point on the far plane (z = -zfar in view space for RH).
  mathkata::Vector<T, 4> far_point(0, 0, -zfar, 1);
  mathkata::Vector<T, 4> far_clip = proj * far_point;
  T far_ndc_z = far_clip.z / far_clip.w;
  EXPECT_NEAR(far_ndc_z, static_cast<T>(1), precision);
}
TEST_SCALAR_F(OrthoOpenGLDepth, 1e-5f, 1e-10)

// Test that the DirectX orthographic matrix maps the near plane to z = 0
// and the far plane to z = 1.
template <class T>
void OrthoDirectXDepth_Test(const T& precision) {
  const T left = static_cast<T>(-10);
  const T right = static_cast<T>(10);
  const T bottom = static_cast<T>(-10);
  const T top = static_cast<T>(10);
  const T znear = static_cast<T>(1);
  const T zfar = static_cast<T>(100);

  mathkata::Matrix<T, 4> proj = mathkata::Matrix<T, 4>::ortho(
      left, right, bottom, top, znear, zfar, mathkata::DepthRange::kDirectX);

  // transform a point on the near plane (z = -znear in view space for RH).
  mathkata::Vector<T, 4> near_point(0, 0, -znear, 1);
  mathkata::Vector<T, 4> near_clip = proj * near_point;
  T near_ndc_z = near_clip.z / near_clip.w;
  EXPECT_NEAR(near_ndc_z, static_cast<T>(0), precision);

  // transform a point on the far plane (z = -zfar in view space for RH).
  mathkata::Vector<T, 4> far_point(0, 0, -zfar, 1);
  mathkata::Vector<T, 4> far_clip = proj * far_point;
  T far_ndc_z = far_clip.z / far_clip.w;
  EXPECT_NEAR(far_ndc_z, static_cast<T>(1), precision);
}
TEST_SCALAR_F(OrthoDirectXDepth, 1e-5f, 1e-10)

// Test that the default ortho (no DepthRange argument) uses OpenGL convention,
// matching the explicit OpenGL call.
template <class T>
void OrthoDefaultIsOpenGL_Test(const T& precision) {
  const T left = static_cast<T>(-5);
  const T right = static_cast<T>(5);
  const T bottom = static_cast<T>(-5);
  const T top = static_cast<T>(5);
  const T znear = static_cast<T>(0.5);
  const T zfar = static_cast<T>(200);

  mathkata::Matrix<T, 4> default_proj =
      mathkata::Matrix<T, 4>::ortho(left, right, bottom, top, znear, zfar);
  mathkata::Matrix<T, 4> opengl_proj = mathkata::Matrix<T, 4>::ortho(
      left, right, bottom, top, znear, zfar, mathkata::DepthRange::kOpenGL);

  for (int i = 0; i < 16; ++i) {
    EXPECT_NEAR(default_proj[i], opengl_proj[i], precision);
  }
}
TEST_SCALAR_F(OrthoDefaultIsOpenGL, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test look-at matrix calculation.
template <class T>
void lookAt_Test(const T& precision) {
  using RH = std::integral_constant<mathkata::Handedness,
                                    mathkata::Handedness::kRightHanded>;
  using LH = std::integral_constant<mathkata::Handedness,
                                    mathkata::Handedness::kLeftHanded>;
  // clang-format off
  const MatrixExpectation<T, 4, 4> kTestCasesRH[] = {
    {
      "default RH origin along z",
      mathkata::Matrix<T, 4, 4>::lookAt(
          mathkata::Vector<T, 3>(0, 0, 1), mathkata::Vector<T, 3>(0, 0, 0),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(-1, 0, 0, 0,
                               0, 1, 0, 0,
                               0, 0, -1,0,
                               0, 0, 0, 1),
    },
    {
      "right-handed diagonal along diagonal",
      mathkata::Matrix<T, 4, 4>::template lookAt<RH::value>(
          mathkata::Vector<T, 3>(0, 0, 0), mathkata::Vector<T, 3>(1, 1, 1),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(
          static_cast<T>(0.707106781), static_cast<T>(-0.408248290),
              static_cast<T>(0.577350269), 0,
          0, static_cast<T>(0.816496581), static_cast<T>(0.577350269), 0,
          static_cast<T>(-0.707106781), static_cast<T>(-0.408248290),
              static_cast<T>(0.577350269), 0,
          0, 0, static_cast<T>(-1.732050808), 1),
    },
    {
      "right-handed origin along z",
      mathkata::Matrix<T, 4, 4>::template lookAt<RH::value>(
          mathkata::Vector<T, 3>(0, 0, 1), mathkata::Vector<T, 3>(0, 0, 0),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(-1, 0, 0, 0,
                               0, 1, 0, 0,
                               0, 0, -1,0,
                               0, 0, 0, 1),
    },
    {
      "right-handed origin along x",
      mathkata::Matrix<T, 4, 4>::template lookAt<RH::value>(
          mathkata::Vector<T, 3>(1, 0, 0), mathkata::Vector<T, 3>(0, 0, 0),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(0, 0, -1, 0,
                              0, 1, 0, 0,
                              1, 0, 0, 0,
                              0, 0, 0, 1),
    },
    {
      "right-handed origin along y",
      mathkata::Matrix<T, 4, 4>::template lookAt<RH::value>(
          mathkata::Vector<T, 3>(0, 1, 0), mathkata::Vector<T, 3>(0, 0, 0),
          mathkata::Vector<T, 3>(1, 0, 0)),
      mathkata::Matrix<T, 4, 4>(0, 1, 0, 0,
                              0, 0, -1, 0,
                              -1, 0, 0, 0,
                              0,  0, 0, 1),
    },
    {
      "right-handed translated eye along x",
      mathkata::Matrix<T, 4, 4>::template lookAt<RH::value>(
          mathkata::Vector<T, 3>(2, 1, 1), mathkata::Vector<T, 3>(1, 1, 1),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(0, 0, -1, 0,
                              0, 1, 0, 0,
                              1, 0, 0, 0,
                             -1,-1, 1, 1),
    },
  };
  const MatrixExpectation<T, 4, 4> kTestCasesLH[] = {
    {
      "left-handed origin along z",
      mathkata::Matrix<T, 4, 4>::template lookAt<LH::value>(
          mathkata::Vector<T, 3>(0, 0, 1), mathkata::Vector<T, 3>(0, 0, 0),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              0, 0, 0, 1),
    },
    {
      "origin along diagonal",
      mathkata::Matrix<T, 4, 4>::template lookAt<LH::value>(
          mathkata::Vector<T, 3>(0, 0, 0), mathkata::Vector<T, 3>(1, 1, 1),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(
          static_cast<T>(-0.707106781), static_cast<T>(-0.408248290),
              static_cast<T>(-0.577350269), 0,
          0, static_cast<T>(0.816496580), static_cast<T>(-0.577350269), 0,
          static_cast<T>(0.707106781), static_cast<T>(-0.408248290),
              static_cast<T>(-0.577350269), 0,
          0, 0, static_cast<T>(1.732050808), 1),
    },
    {
      "origin along z",
      mathkata::Matrix<T, 4, 4>::template lookAt<LH::value>(
          mathkata::Vector<T, 3>(0, 0, 2), mathkata::Vector<T, 3>(0, 0, 0),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              0, 0, 0, 1),
    },
    {
      "origin along x",
      mathkata::Matrix<T, 4, 4>::template lookAt<LH::value>(
          mathkata::Vector<T, 3>(1, 0, 0), mathkata::Vector<T, 3>(0, 0, 0),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(0, 0, 1, 0,
                              0, 1, 0, 0,
                              -1, 0, 0, 0,
                              0, 0, 0, 1),
    },
    {
      "origin along y",
      mathkata::Matrix<T, 4, 4>::template lookAt<LH::value>(
          mathkata::Vector<T, 3>(0, 1, 0), mathkata::Vector<T, 3>(0, 0, 0),
          mathkata::Vector<T, 3>(1, 0, 0)),
      mathkata::Matrix<T, 4, 4>(0, 1, 0, 0,
                              0, 0, 1, 0,
                              1, 0, 0, 0,
                              0, 0, 0, 1),
    },
    {
      "translated eye, looking along z",
      mathkata::Matrix<T, 4, 4>::template lookAt<LH::value>(
          mathkata::Vector<T, 3>(1, 1, 2), mathkata::Vector<T, 3>(1, 1, 1),
          mathkata::Vector<T, 3>(0, 1, 0)),
      mathkata::Matrix<T, 4, 4>(1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              -1, -1, -1, 1),
    },
  };
  // clang-format on
  VerifyMatrixExpectations(
      kTestCasesRH, sizeof(kTestCasesRH) / sizeof(kTestCasesRH[0]), precision);
  VerifyMatrixExpectations(
      kTestCasesLH, sizeof(kTestCasesLH) / sizeof(kTestCasesLH[0]), precision);
}
TEST_SCALAR_F(lookAt, FLOAT_PRECISION, kLookAtDoublePrecision)

// Test unProject calculation.
template <class T>
void unProject_Test(const T& precision) {
  // clang-format off
  mathkata::Matrix<T, 4, 4> modelView =
      mathkata::Matrix<T, 4, 4>(-1, 0,                   0, 0,
                               0, 1,                   0, 0,
                               0, 0,                  -1, 0,
                               0, 0, static_cast<T>(-10), 1);
  mathkata::Matrix<T, 4, 4> projection =
      mathkata::Matrix<T, 4, 4>(
          static_cast<T>(1.81066),  0,                            0,   0,
                        0, static_cast<T>(2.41421342),            0,   0,
                        0,          0,   static_cast<T>(-1.00001991), -1,
                        0,          0,  static_cast<T>(-0.200001985),  0);
  // clang-format on
  mathkata::Vector<T, 3> result;
  bool success = mathkata::Matrix<T, 4, 4>::unProject(
      mathkata::Vector<T, 3>(754, 1049, 1), modelView, projection, 1600, 1200,
      &result);
  EXPECT_TRUE(success);
  EXPECT_NEAR(result.x, 319.00242400912055, 300.0 * precision);
  EXPECT_NEAR(result.y, 3113.7409399625253, 3000.0 * precision);
  EXPECT_NEAR(result.z, 10035.303114023569, 10000.0 * precision);
}
TEST_SCALAR_F(unProject, kUnProjectFloatPrecision, DOUBLE_PRECISION)

// Test unProject returns false for a singular (all-zero) matrix.
template <class T>
void UnProject_SingularMatrix_Test(const T& precision) {
  (void)precision;
  // A zero matrix is singular and cannot be inverted.
  mathkata::Matrix<T, 4, 4> singular =
      mathkata::Matrix<T, 4, 4>(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  mathkata::Matrix<T, 4, 4> identity = mathkata::Matrix<T, 4, 4>::identity();
  mathkata::Vector<T, 3> window_coord(400, 300, static_cast<T>(0.5));
  mathkata::Vector<T, 3> result;
  // Singular model-view matrix.
  bool success_mv = mathkata::Matrix<T, 4, 4>::unProject(
      window_coord, singular, identity, 800, 600, &result);
  EXPECT_FALSE(success_mv);
  // Singular projection matrix.
  bool success_proj = mathkata::Matrix<T, 4, 4>::unProject(
      window_coord, identity, singular, 800, 600, &result);
  EXPECT_FALSE(success_proj);
}
TEST_SCALAR_F(UnProject_SingularMatrix, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test matrix transposition.
template <class T, int d>
void transpose_Test(const T& precision) {
  (void)precision;
  mathkata::Matrix<T, d> matrix;
  for (int i = 0; i < d * d; ++i) {
    matrix[i] = static_cast<T>(i);
  }
  mathkata::Matrix<T, d> transpose = matrix.transpose();
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(matrix(i, j), transpose(j, i), static_cast<T>(0));
    }
  }
}

TEST_ALL_F(transpose, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test getColumn returns the correct column vector from the matrix.
template <class T, int d>
void getColumn_Test(const T& precision) {
  (void)precision;
  mathkata::Matrix<T, d> matrix;
  for (int i = 0; i < d * d; ++i) {
    matrix[i] = static_cast<T>(i);
  }
  for (int col = 0; col < d; ++col) {
    mathkata::Vector<T, d> column = matrix.getColumn(col);
    for (int row = 0; row < d; ++row) {
      EXPECT_EQ(matrix(row, col), column[row])
          << "col=" << col << " row=" << row;
    }
  }
}
TEST_ALL_F(getColumn, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test that getColumn returns a mutable reference that can modify the matrix.
template <class T, int d>
void GetColumnMutable_Test(const T& precision) {
  (void)precision;
  mathkata::Matrix<T, d> matrix(static_cast<T>(0));
  for (int col = 0; col < d; ++col) {
    mathkata::Vector<T, d> new_col;
    for (int row = 0; row < d; ++row) {
      new_col[row] = static_cast<T>(col * d + row + 1);
    }
    matrix.getColumn(col) = new_col;
  }
  for (int col = 0; col < d; ++col) {
    for (int row = 0; row < d; ++row) {
      EXPECT_EQ(matrix(row, col), static_cast<T>(col * d + row + 1))
          << "col=" << col << " row=" << row;
    }
  }
}
TEST_ALL_F(GetColumnMutable, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test getRow returns the correct row vector from the matrix.
template <class T, int d>
void getRow_Test(const T& precision) {
  (void)precision;
  mathkata::Matrix<T, d> matrix;
  for (int i = 0; i < d * d; ++i) {
    matrix[i] = static_cast<T>(i);
  }
  for (int row = 0; row < d; ++row) {
    mathkata::Vector<T, d> r = matrix.getRow(row);
    for (int col = 0; col < d; ++col) {
      EXPECT_EQ(matrix(row, col), r[col]) << "row=" << row << " col=" << col;
    }
  }
}
TEST_ALL_F(getRow, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test that getRow and getColumn are consistent with each other and with
// element access. The i-th element of getRow(r) should equal the r-th
// element of getColumn(i).
template <class T, int d>
void GetRowColumnConsistency_Test(const T& precision) {
  (void)precision;
  mathkata::Matrix<T, d> matrix;
  for (int i = 0; i < d * d; ++i) {
    matrix[i] = static_cast<T>(i * 3 + 1);
  }
  for (int row = 0; row < d; ++row) {
    mathkata::Vector<T, d> r = matrix.getRow(row);
    for (int col = 0; col < d; ++col) {
      mathkata::Vector<T, d> c = matrix.getColumn(col);
      EXPECT_EQ(r[col], c[row]) << "row=" << row << " col=" << col;
      EXPECT_EQ(r[col], matrix(row, col)) << "row=" << row << " col=" << col;
    }
  }
}
TEST_ALL_F(GetRowColumnConsistency, FLOAT_PRECISION, DOUBLE_PRECISION)

// Return one of the numbers:
//   offset, offset + width/d, offset + 2*width/d, ... , offset + (d-1)*width/d
// For each i=0..d-1, the number returned is different.
//
// The order of the numbers is determined by 'prime' which should be a prime
// number > d.
template <class T, int d>
static T WellSpacedNumber(int i, int prime, T width, T offset) {
  // Reorder the i's from 0..(d-1) by using a prime number.
  // The numbers get reordered (i.e. no duplicates) because 'd' and 'prime' are
  // relatively prime.
  const int remapped = ((i + 1) * prime) % d;

  // Map to [0,1) range. That is, inclusive of 0, exclusive of 1.
  const T zero_to_one = static_cast<T>(remapped) / static_cast<T>(d);

  // Map to [offset, offset + width) range.
  const T offset_and_scaled = zero_to_one * width + offset;
  return offset_and_scaled;
}

// Generate a (probably) invertable matrix. I haven't gone through the math
// to verify that it's actually invertable for all d, but for small d it is.
//
// We adjusting each element of an identity matrix by a small amount.
// The amount must be small so that the matrix stays reasonably close to
// the identity. Matrices become progressively less numerically stable the
// further the get from identity.
template <class T, int d>
static mathkata::Matrix<T, d> InvertableMatrix() {
  mathkata::Matrix<T, d> invertable = mathkata::Matrix<T, d>::identity();

  for (int i = 0; i < d; ++i) {
    // The width and offset constants are arbitrary. We do want to keep the
    // pseudo-random values centered near 0 though.
    const T rand_i = WellSpacedNumber<T, d>(i, 7, 0.8f, -0.33f);

    for (int j = 0; j < d; ++j) {
      const T rand_j = WellSpacedNumber<T, d>(j, 13, 0.6f, -0.4f);
      invertable(i, j) += rand_i * rand_j;
    }
  }
  return invertable;
}

template <class T, int d>
static void ExpectEqualMatrices(const mathkata::Matrix<T, d>& a,
                                const mathkata::Matrix<T, d>& b, T precision) {
  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(a(i, j), b(i, j), precision);
    }
  }
}

// Test matrix operator*() by multiplying an invertable matrix by its
// inverse. Should end up with identity.
template <class T, int d>
void MultiplyOperatorInverse_Test(const T& precision) {
  typedef typename mathkata::Matrix<T, d> Mat;
  const Mat identity = Mat::identity();
  const Mat invertable = InvertableMatrix<T, d>();

  // Use operator*() to go way from the identity and then get back to it.
  Mat product = identity;
  product *= invertable;
  product *= invertable.inverse();

  // Test that operator*() gets is back to where we need to go.
  ExpectEqualMatrices(product, identity, precision);
}

TEST_ALL_F(MultiplyOperatorInverse, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test static operator*(a, b) by multiplying an invertable matrix by its
// inverse. Should end up with identity.
template <class T, int d>
void ExternalMultiplyOperatorInverse_Test(const T& precision) {
  typedef typename mathkata::Matrix<T, d> Mat;
  const Mat identity = Mat::identity();
  const Mat invertable = InvertableMatrix<T, d>();

  // Use operator*() to go way from the identity and then get back to it.
  Mat product = identity;
  product = product * invertable;
  product = product * invertable.inverse();

  // Test that operator*() gets is back to where we need to go.
  ExpectEqualMatrices(product, identity, precision);
}

TEST_ALL_F(ExternalMultiplyOperatorInverse, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test matrix operator*() by multiplying a non-zero matrix by identity.
// Should be no change.
template <class T, int d>
void MultiplyOperatorIdentity_Test(const T& precision) {
  typedef typename mathkata::Matrix<T, d> Mat;
  const Mat identity = Mat::identity();
  const Mat invertable = InvertableMatrix<T, d>();

  // Use operator*() to multipy by the identity.
  Mat product = invertable;
  product *= identity;

  // Test that operator*() didn't change anything when multiplying by identity.
  ExpectEqualMatrices(product, invertable, precision);
}

TEST_ALL_F(MultiplyOperatorIdentity, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test static operator*(a, b) by multiplying a non-zero matrix by identity.
// Should be no change.
template <class T, int d>
void ExternalMultiplyOperatorIdentity_Test(const T& precision) {
  typedef typename mathkata::Matrix<T, d> Mat;
  const Mat identity = Mat::identity();
  const Mat invertable = InvertableMatrix<T, d>();

  // Use operator*() to multipy by the identity.
  const Mat product = invertable * identity;

  // Test that operator*() didn't change anything when multiplying by identity.
  ExpectEqualMatrices(product, invertable, precision);
}

TEST_ALL_F(ExternalMultiplyOperatorIdentity, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test matrix operator*() by multiplying a non-zero matrix by zero.
// Should be no change.
template <class T, int d>
void MultiplyOperatorZero_Test(const T& precision) {
  typedef typename mathkata::Matrix<T, d> Mat;
  const Mat zero(static_cast<T>(0));
  const Mat invertable = InvertableMatrix<T, d>();

  // Use operator*() to multipy by the zero.
  Mat product = invertable;
  product *= zero;

  // Test that operator*() didn't change anything when multiplying by zero.
  ExpectEqualMatrices(product, zero, precision);
}

TEST_ALL_F(MultiplyOperatorZero, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test static operator*(a, b) by multiplying a non-zero matrix by zero.
// Should be no change.
template <class T, int d>
void ExternalMultiplyOperatorZero_Test(const T& precision) {
  typedef typename mathkata::Matrix<T, d> Mat;
  const Mat zero(static_cast<T>(0));
  const Mat invertable = InvertableMatrix<T, d>();

  // Use operator*() to multipy by the zero.
  const Mat product = invertable * zero;

  // Test that operator*() didn't change anything when multiplying by zero.
  ExpectEqualMatrices(product, zero, precision);
}

TEST_ALL_F(ExternalMultiplyOperatorZero, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test Matrix<>::toAffineTransform().
template <class T>
void Mat4ToAffine_Test(const T&) {
  typedef typename mathkata::Matrix<T, 4> Mat4;
  typedef typename mathkata::Matrix<T, 4, 3> Affine;
  const Mat4 indices4(0, 1, 2, 0, 4, 5, 6, 0, 8, 9, 10, 0, 12, 13, 14, 1);
  const Affine indices_affine(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14);

  const Affine to_affine = Mat4::toAffineTransform(indices4);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      EXPECT_EQ(to_affine(i, j), indices_affine(i, j));
    }
  }
}

TEST_SCALAR_F(Mat4ToAffine, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test Matrix<>::fromAffineTransform().
template <class T>
void Mat4FromAffine_Test(const T&) {
  typedef typename mathkata::Matrix<T, 4> Mat4;
  typedef typename mathkata::Matrix<T, 4, 3> Affine;
  const Mat4 indices4(0, 1, 2, 0, 4, 5, 6, 0, 8, 9, 10, 0, 12, 13, 14, 1);
  const Affine indices_affine(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14);

  const Mat4 to_mat4 = Mat4::fromAffineTransform(indices_affine);
  ExpectEqualMatrices(to_mat4, indices4, static_cast<T>(0));
}

TEST_SCALAR_F(Mat4FromAffine, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test converting back and forth via Matrix<>::To/fromAffineTransform().
template <class T>
void Mat4ToAndFromAffine_Test(const T&) {
  typedef typename mathkata::Matrix<T, 4> Mat4;
  typedef typename mathkata::Matrix<T, 4, 3> Affine;
  const Mat4 indices4(0, 1, 2, 0, 4, 5, 6, 0, 8, 9, 10, 0, 12, 13, 14, 1);
  const Affine indices_affine(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14);

  // Convert to/from AffineTransform and check to ensure the result is the same
  // as the original.
  const Mat4 converted =
      Mat4::fromAffineTransform(Mat4::toAffineTransform(indices4));

  ExpectEqualMatrices(indices4, converted, static_cast<T>(0));

  // Perform a normal mat4 * mat4 multiplication and compare its result with
  // multiplications involving conversions.
  const Mat4 mat4_multiplication = indices4 * indices4;
  const Mat4 affine_multiplication =
      Mat4::fromAffineTransform(indices_affine)
      * Mat4::fromAffineTransform(indices_affine);
  const Mat4 affine_and_mat4_multiplication =
      indices4 * Mat4::fromAffineTransform(indices_affine);

  ExpectEqualMatrices(mat4_multiplication, affine_multiplication,
                      static_cast<T>(0));
  ExpectEqualMatrices(mat4_multiplication, affine_and_mat4_multiplication,
                      static_cast<T>(0));

  // Ensure that the result from the multiplication produces the expected
  // AffineTransform result.
  const Affine expected_result(20, 68, 116, 176, 23, 83, 143, 216, 26, 98, 170,
                               256);
  const Affine affine_result = Mat4::toAffineTransform(affine_multiplication);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      EXPECT_EQ(expected_result(i, j), affine_result(i, j));
    }
  }
}

TEST_SCALAR_F(Mat4ToAndFromAffine, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test constructing a non-square matrix from a pointer array.
// This verifies that the stride is Rows (not Cols) for column-major layout.
template <class T>
void MatrixPointerConstructorNonSquare_Test(const T& precision) {
  // A 4x3 matrix (4 rows, 3 cols) stored column-major:
  // Column 0: {1, 2, 3, 4}, Column 1: {5, 6, 7, 8}, Column 2: {9, 10, 11, 12}
  const T data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  mathkata::Matrix<T, 4, 3> mat(data);
  // Verify each element using (row, col) access.
  EXPECT_NEAR(mat(0, 0), T(1), precision);
  EXPECT_NEAR(mat(1, 0), T(2), precision);
  EXPECT_NEAR(mat(2, 0), T(3), precision);
  EXPECT_NEAR(mat(3, 0), T(4), precision);
  EXPECT_NEAR(mat(0, 1), T(5), precision);
  EXPECT_NEAR(mat(1, 1), T(6), precision);
  EXPECT_NEAR(mat(2, 1), T(7), precision);
  EXPECT_NEAR(mat(3, 1), T(8), precision);
  EXPECT_NEAR(mat(0, 2), T(9), precision);
  EXPECT_NEAR(mat(1, 2), T(10), precision);
  EXPECT_NEAR(mat(2, 2), T(11), precision);
  EXPECT_NEAR(mat(3, 2), T(12), precision);
}

TEST_SCALAR_F(MatrixPointerConstructorNonSquare, FLOAT_PRECISION,
              DOUBLE_PRECISION)

// Test extracting the 3x3 rotation Matrix portion from a 4x4 Matrix.
template <class T>
void Mat4ToRotationMatrix_Test(const T&) {
  typedef typename mathkata::Matrix<T, 4> Mat4;
  typedef typename mathkata::Matrix<T, 3> Mat3;
  const Mat4 input(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
  const Mat3 expect(1, 2, 3, 5, 6, 7, 9, 10, 11);
  Mat3 result = input.toRotationMatrix(input);
  for (int i = 0; i < 9; i++) {
    EXPECT_EQ(expect[i], result[i]);
  }
}

TEST_SCALAR_F(Mat4ToRotationMatrix, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test that rotationX/Y/Z produce correct results with double precision.
// This verifies that the trig functions use std::cos/std::sin (which handle
// double) rather than cosf/sinf (which truncate to float).
template <class T>
void RotationPrecision_Test(const T& precision) {
  typedef mathkata::Matrix<T, 3> Mat3;
  const T angle = static_cast<T>(1.0);
  const T c = std::cos(angle);
  const T s = std::sin(angle);

  Mat3 rx = Mat3::rotationX(angle);
  EXPECT_NEAR(rx(0, 0), T(1), precision);
  EXPECT_NEAR(rx(1, 1), c, precision);
  EXPECT_NEAR(rx(2, 1), s, precision);
  EXPECT_NEAR(rx(1, 2), -s, precision);
  EXPECT_NEAR(rx(2, 2), c, precision);

  Mat3 ry = Mat3::rotationY(angle);
  EXPECT_NEAR(ry(0, 0), c, precision);
  EXPECT_NEAR(ry(2, 0), -s, precision);
  EXPECT_NEAR(ry(1, 1), T(1), precision);
  EXPECT_NEAR(ry(0, 2), s, precision);
  EXPECT_NEAR(ry(2, 2), c, precision);

  Mat3 rz = Mat3::rotationZ(angle);
  EXPECT_NEAR(rz(0, 0), c, precision);
  EXPECT_NEAR(rz(1, 0), s, precision);
  EXPECT_NEAR(rz(0, 1), -s, precision);
  EXPECT_NEAR(rz(1, 1), c, precision);
  EXPECT_NEAR(rz(2, 2), T(1), precision);
}

TEST_SCALAR_F(RotationPrecision, FLOAT_PRECISION, DOUBLE_PRECISION)
// Test hadamardProduct (component-wise multiplication).
template <class T, int d>
void hadamardProduct_Test(const T& precision) {
  mathkata::Matrix<T, d> m1;
  mathkata::Matrix<T, d> m2;
  for (int i = 0; i < d * d; ++i) {
    m1[i] = static_cast<T>(i + 1);
    m2[i] = static_cast<T>(d * d - i);
  }
  mathkata::Matrix<T, d> result =
      mathkata::Matrix<T, d>::hadamardProduct(m1, m2);
  for (int i = 0; i < d * d; ++i) {
    EXPECT_NEAR(result[i], m1[i] * m2[i], precision);
  }
}

TEST_ALL_F(hadamardProduct, FLOAT_PRECISION, DOUBLE_PRECISION)

// This will test converting from a translation into a matrix and back again.
// Test the compilation of basic matrix operations given in the sample file.
// This will test transforming a vector with a matrix.
TEST_F(MatrixTests, MatrixSample) {
  using namespace mathkata;
  /// @doxysnippetstart Chapter04_Matrices.md Matrix_Sample
  Vector<float, 3> trans(3.f, 2.f, 8.f);
  Vector<float, 3> rotation(0.4f, 1.4f, 0.33f);
  Vector<float, 3> vector(4.f, 8.f, 1.f);

  Quaternion<float> rotQuat = Quaternion<float>::fromEulerAngles(rotation);
  Matrix<float, 3> rotMatrix = rotQuat.toMatrix();
  Matrix<float, 4> transMatrix = Matrix<float, 4>::fromTranslationVector(trans);
  Matrix<float, 4> rotHMatrix = Matrix<float, 4>::fromRotationMatrix(rotMatrix);

  Matrix<float, 4> matrix = transMatrix * rotHMatrix;
  Vector<float, 3> rotatedVector = matrix * vector;
  /// @doxysnippetend
  const float precision = 1e-2f;
  EXPECT_NEAR(5.14f, rotatedVector[0], precision);
  EXPECT_NEAR(10.11f, rotatedVector[1], precision);
  EXPECT_NEAR(4.74f, rotatedVector[2], precision);
}

// This will test the == matrices operator. The template paramter d corresponds
// to the number of rows and columns.
template <class T, int d>
void Equal_Test(const T& precision) {
  mathkata::Matrix<T, d> expected;
  for (int i = 0; i < d * d; ++i) {
    expected[i] = static_cast<T>(i * precision);
  }
  mathkata::Matrix<T, d> copy(expected);
  EXPECT_TRUE(expected == copy);

  mathkata::Matrix<T, d> close(expected - static_cast<T>(1));
  EXPECT_FALSE(expected == close);
}
TEST_ALL_F(Equal, FLOAT_PRECISION, DOUBLE_PRECISION)

// This will test the != matrices operator. The template paramter d corresponds
// to the number of rows and columns.
template <class T, int d>
void NotEqual_Test(const T& precision) {
  mathkata::Matrix<T, d> expected;
  for (int i = 0; i < d * d; ++i) {
    expected[i] = static_cast<T>(i * precision);
  }
  mathkata::Matrix<T, d> copy(expected);
  EXPECT_FALSE(expected != copy);

  mathkata::Matrix<T, d> close(expected - static_cast<T>(1));
  EXPECT_TRUE(expected != close);
}
TEST_ALL_F(NotEqual, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test that operator== and operator!= correctly detect differences in each
// individual element of the matrix. This verifies that the inequality check
// uses logical OR (short-circuit) rather than bitwise OR, and that every
// element position is properly compared.
template <class T, int d>
void EqualityPerElement_Test(const T& precision) {
  (void)precision;
  mathkata::Matrix<T, d> base;
  for (int i = 0; i < d * d; ++i) {
    base[i] = static_cast<T>(i + 1);
  }

  // A copy should be equal.
  mathkata::Matrix<T, d> copy(base);
  EXPECT_TRUE(base == copy);
  EXPECT_FALSE(base != copy);

  // Changing any single element should make the matrices not equal.
  for (int i = 0; i < d * d; ++i) {
    mathkata::Matrix<T, d> modified(base);
    modified[i] = base[i] + static_cast<T>(100);
    EXPECT_FALSE(base == modified)
        << "Element " << i << " difference not detected by operator==";
    EXPECT_TRUE(base != modified)
        << "Element " << i << " difference not detected by operator!=";
  }
}
TEST_ALL_F(EqualityPerElement, FLOAT_PRECISION, DOUBLE_PRECISION)

// Simple class that represents a possible compatible type for a vector.
// That is, it's just an array of T of length d, so can be loaded and
// stored from mathkata::Vector<T,d> using toType() and fromType().
template <class T, int d>
struct SimpleMatrix {
  T values[d * d];
};

// This will test the fromType() conversion functions.
template <class T, int d>
void fromType_Test(const T& precision) {
  typedef SimpleMatrix<T, d> CompatibleT;
  typedef mathkata::Matrix<T, d> MatrixT;

  CompatibleT compatible;
  for (int i = 0; i < d * d; ++i) {
    compatible.values[i] = static_cast<T>(i * precision);
  }

#ifdef MATHKATA_COMPILE_WITH_PADDING
  // With padding, vec3s take up 4-floats worth of memory, so byte-wise
  // conversion won't work.
  if (sizeof(CompatibleT) != sizeof(MatrixT)) {
    EXPECT_EQ(d, 3);
    return;
  }
#endif  // MATHKATA_COMPILE_WITH_PADDING

  const MatrixT matrix = MatrixT::fromType(compatible);

  for (int i = 0; i < d * d; ++i) {
    EXPECT_EQ(compatible.values[i], matrix[i]);
  }
}
TEST_ALL_F(fromType, 0.0f, 0.0)

// This will test the toType() conversion functions.
template <class T, int d>
void toType_Test(const T& precision) {
  typedef SimpleMatrix<T, d> CompatibleT;
  typedef mathkata::Matrix<T, d> MatrixT;

  MatrixT matrix;
  for (int i = 0; i < d * d; ++i) {
    matrix[i] = static_cast<T>(i * precision);
  }

#ifdef MATHKATA_COMPILE_WITH_PADDING
  // With padding, vec3s take up 4-floats worth of memory, so byte-wise
  // conversion won't work.
  if (sizeof(CompatibleT) != sizeof(MatrixT)) {
    EXPECT_EQ(d, 3);
    return;
  }
#endif  // MATHKATA_COMPILE_WITH_PADDING

  const CompatibleT compatible = MatrixT::template toType<CompatibleT>(matrix);

  for (int i = 0; i < d * d; ++i) {
    EXPECT_EQ(compatible.values[i], matrix[i]);
  }
}
TEST_ALL_F(toType, 0.0f, 0.0)

// This will test a roundtrip through fromType() and toType().
// Converts SimpleMatrix -> Matrix -> SimpleMatrix and verifies the values
// are preserved. This exercises the memcpy-based type-punning path.
template <class T, int d>
void FromTypeToTypeRoundtrip_Test(const T& precision) {
  typedef SimpleMatrix<T, d> CompatibleT;
  typedef mathkata::Matrix<T, d> MatrixT;

  CompatibleT original;
  for (int i = 0; i < d * d; ++i) {
    original.values[i] = static_cast<T>(i * precision + static_cast<T>(1));
  }

#ifdef MATHKATA_COMPILE_WITH_PADDING
  // With padding, vec3s take up 4-floats worth of memory, so byte-wise
  // conversion won't work.
  if (sizeof(CompatibleT) != sizeof(MatrixT)) {
    EXPECT_EQ(d, 3);
    return;
  }
#endif  // MATHKATA_COMPILE_WITH_PADDING

  // SimpleMatrix -> Matrix -> SimpleMatrix
  const MatrixT matrix = MatrixT::fromType(original);
  const CompatibleT roundtripped =
      MatrixT::template toType<CompatibleT>(matrix);

  for (int i = 0; i < d * d; ++i) {
    EXPECT_EQ(original.values[i], roundtripped.values[i]);
  }
}
TEST_ALL_F(FromTypeToTypeRoundtrip, 0.0f, 0.0)

template <class T, int d>
void OutputStream_Test(const T&) {
  mathkata::Matrix<T, d> matrix;
  for (int i = 0; i < d * d; ++i) {
    matrix[i] = static_cast<T>(i);
  }

  std::stringstream ss;
  ss << matrix;

  switch (d) {
    case 1:
      EXPECT_EQ("((0))", ss.str());
      break;
    case 2:
      EXPECT_EQ("((0, 1), (2, 3))", ss.str());
      break;
    case 3:
      EXPECT_EQ("((0, 1, 2), (3, 4, 5), (6, 7, 8))", ss.str());
      break;
    case 4:
      EXPECT_EQ(
          "((0, 1, 2, 3), (4, 5, 6, 7), (8, 9, 10, 11), (12, 13, 14, 15))",
          ss.str());
      break;
  }
}
TEST_ALL_F(OutputStream, 0.0f, 0.0)
TEST_F(MatrixTests, OutputStream_Test_float_1) {
  OutputStream_Test<float, 1>(0.0f);
}

// Test non-square matrix multiplication via mathkata::multiply().
// Multiplies a 2x3 matrix by a 3x4 matrix and verifies the 2x4 result.
template <class T>
void MultiplyNonSquare_2x3_times_3x4_Test(const T& precision) {
  // m1 is 2 rows x 3 cols (column-major storage: col0={1,4}, col1={2,5},
  // col2={3,6}).
  //   | 1  2  3 |
  //   | 4  5  6 |
  const T data1[] = {T(1), T(4), T(2), T(5), T(3), T(6)};
  mathkata::Matrix<T, 2, 3> m1(data1);

  // m2 is 3 rows x 4 cols (column-major storage).
  //   | 7  8  9  10 |
  //   | 11 12 13 14 |
  //   | 15 16 17 18 |
  const T data2[] = {T(7), T(11), T(15), T(8),  T(12), T(16),
                     T(9), T(13), T(17), T(10), T(14), T(18)};
  mathkata::Matrix<T, 3, 4> m2(data2);

  mathkata::Matrix<T, 2, 4> result = mathkata::multiply(m1, m2);

  // Expected result (2x4):
  //   row0: 1*7+2*11+3*15=74   1*8+2*12+3*16=80   1*9+2*13+3*17=86
  //   1*10+2*14+3*18=92 row1: 4*7+5*11+6*15=173  4*8+5*12+6*16=188
  //   4*9+5*13+6*17=203  4*10+5*14+6*18=218
  EXPECT_NEAR(result(0, 0), T(74), precision);
  EXPECT_NEAR(result(1, 0), T(173), precision);
  EXPECT_NEAR(result(0, 1), T(80), precision);
  EXPECT_NEAR(result(1, 1), T(188), precision);
  EXPECT_NEAR(result(0, 2), T(86), precision);
  EXPECT_NEAR(result(1, 2), T(203), precision);
  EXPECT_NEAR(result(0, 3), T(92), precision);
  EXPECT_NEAR(result(1, 3), T(218), precision);
}

TEST_F(MatrixTests, MultiplyNonSquare_2x3_times_3x4_float) {
  MultiplyNonSquare_2x3_times_3x4_Test<float>(FLOAT_PRECISION);
}
TEST_F(MatrixTests, MultiplyNonSquare_2x3_times_3x4_double) {
  MultiplyNonSquare_2x3_times_3x4_Test<double>(DOUBLE_PRECISION);
}

// Test that multiply() also works correctly for square matrices and matches
// operator*.
template <class T, int d>
void MultiplySquareMatchesOperator_Test(const T& precision) {
  T x1[d * d], x2[d * d];
  for (int i = 0; i < d * d; ++i) x1[i] = rand() / static_cast<T>(RAND_MAX);
  for (int i = 0; i < d * d; ++i) x2[i] = rand() / static_cast<T>(RAND_MAX);
  mathkata::Matrix<T, d> m1(x1), m2(x2);

  mathkata::Matrix<T, d> via_operator = m1 * m2;
  mathkata::Matrix<T, d> via_multiply = mathkata::multiply(m1, m2);

  for (int i = 0; i < d; ++i) {
    for (int j = 0; j < d; ++j) {
      EXPECT_NEAR(via_operator(i, j), via_multiply(i, j), precision);
    }
  }
}
TEST_ALL_F(MultiplySquareMatchesOperator, FLOAT_PRECISION, DOUBLE_PRECISION)

// Test multiply() with a 4x3 times 3x2 producing a 4x2 result.
template <class T>
void MultiplyNonSquare_4x3_times_3x2_Test(const T& precision) {
  // m1 is 4 rows x 3 cols.
  //   | 1  2  3 |
  //   | 4  5  6 |
  //   | 7  8  9 |
  //   | 10 11 12|
  const T data1[] = {T(1), T(4),  T(7), T(10), T(2), T(5),
                     T(8), T(11), T(3), T(6),  T(9), T(12)};
  mathkata::Matrix<T, 4, 3> m1(data1);

  // m2 is 3 rows x 2 cols.
  //   | 2  1 |
  //   | 0  3 |
  //   | 4  2 |
  const T data2[] = {T(2), T(0), T(4), T(1), T(3), T(2)};
  mathkata::Matrix<T, 3, 2> m2(data2);

  mathkata::Matrix<T, 4, 2> result = mathkata::multiply(m1, m2);

  // Expected result (4x2):
  //   row0: 1*2+2*0+3*4=14   1*1+2*3+3*2=13
  //   row1: 4*2+5*0+6*4=32   4*1+5*3+6*2=31
  //   row2: 7*2+8*0+9*4=50   7*1+8*3+9*2=49
  //   row3: 10*2+11*0+12*4=68  10*1+11*3+12*2=67
  EXPECT_NEAR(result(0, 0), T(14), precision);
  EXPECT_NEAR(result(1, 0), T(32), precision);
  EXPECT_NEAR(result(2, 0), T(50), precision);
  EXPECT_NEAR(result(3, 0), T(68), precision);
  EXPECT_NEAR(result(0, 1), T(13), precision);
  EXPECT_NEAR(result(1, 1), T(31), precision);
  EXPECT_NEAR(result(2, 1), T(49), precision);
  EXPECT_NEAR(result(3, 1), T(67), precision);
}

TEST_F(MatrixTests, MultiplyNonSquare_4x3_times_3x2_float) {
  MultiplyNonSquare_4x3_times_3x2_Test<float>(FLOAT_PRECISION);
}
TEST_F(MatrixTests, MultiplyNonSquare_4x3_times_3x2_double) {
  MultiplyNonSquare_4x3_times_3x2_Test<double>(DOUBLE_PRECISION);
}

// Test multiply() with a 1x3 row vector times a 3x1 column vector (dot
// product).
template <class T>
void MultiplyNonSquare_1x3_times_3x1_Test(const T& precision) {
  const T data1[] = {T(2), T(3), T(4)};
  mathkata::Matrix<T, 1, 3> m1(data1);

  const T data2[] = {T(5), T(6), T(7)};
  mathkata::Matrix<T, 3, 1> m2(data2);

  mathkata::Matrix<T, 1, 1> result = mathkata::multiply(m1, m2);

  // Expected: 2*5 + 3*6 + 4*7 = 10 + 18 + 28 = 56
  EXPECT_NEAR(result(0, 0), T(56), precision);
}

TEST_F(MatrixTests, MultiplyNonSquare_1x3_times_3x1_float) {
  MultiplyNonSquare_1x3_times_3x1_Test<float>(FLOAT_PRECISION);
}
TEST_F(MatrixTests, MultiplyNonSquare_1x3_times_3x1_double) {
  MultiplyNonSquare_1x3_times_3x1_Test<double>(DOUBLE_PRECISION);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

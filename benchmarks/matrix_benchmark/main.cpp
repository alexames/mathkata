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
#include <cmath>
#include <cstdio>
#include <random>

#include "benchmark_common.h"
#include "mathkata/matrix.h"
#include "mathkata/utilities.h"

// Number of elements to iterate over
static const size_t kMatrixSize = 1000;
// Number of iterations of each operation.
static const size_t kIterations = 50;

using mathkata::Matrix;

static std::mt19937& BenchRng() {
  static std::mt19937 rng(42);
  return rng;
}

template <class T>
T BenchRandom() {
  std::uniform_real_distribution<T> dist(static_cast<T>(0), static_cast<T>(1));
  return dist(BenchRng());
}

#define MATRIX_DIMENSIONS 4

typedef float T;

typedef Matrix<T, MATRIX_DIMENSIONS> TestMatrix;

// This test creates a number of matrices and performs some mathematical
// operations on them in order to measure expected performance of matrix
// operations.
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  // Create an array of matrices containing random values.
  TestMatrix* const matrices = new TestMatrix[kMatrixSize];
  TestMatrix mul = TestMatrix::identity();
  for (size_t i = 0; i < kMatrixSize; ++i) {
    TestMatrix mat;
    for (size_t j = 0; j < MATRIX_DIMENSIONS; ++j) {
      mat[static_cast<int>(j)] = BenchRandom<T>();
    }
    matrices[i] = mat;
  }
  // Start matrix benchmark, running a number of loops for more accurate
  // numbers.
  printf("Running matrix benchmark (%s)...\n", MATHKATA_BUILD_OPTIONS_STRING);
  Timer timer;
  PERFTEST_2D_VECTOR_LOOP(kIterations, kMatrixSize) mul += matrices[j];
  PERFTEST_2D_VECTOR_LOOP(kIterations, kMatrixSize) mul *= matrices[j];

#if MATRIX_DIMENSIONS == 4
  PERFTEST_2D_VECTOR_LOOP(kIterations, kMatrixSize) {
    mathkata::Vector<T, MATRIX_DIMENSIONS> tmp =
        matrices[j]
        * mathkata::Vector<T, MATRIX_DIMENSIONS>(
            matrices[i](0, 0), matrices[i](1, 0), matrices[i](2, 0),
            matrices[i](3, 0));
    mul -= TestMatrix::outerProduct(tmp, tmp);
  }
#endif  // MATRIX_DIMENSIONS == 4
  PERFTEST_2D_VECTOR_LOOP(kIterations, kMatrixSize) {
    mul += matrices[j] * BenchRandom<T>();
  }
  // End matrix performance code
  double elapsed = timer.GetElapsedSeconds();
  printf("Took %f seconds\n", elapsed);
  delete[] matrices;
  return 0;
}

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
#include "mathkata/utilities.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "gtest/gtest.h"

class UtilitiesTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// Verify that the return type of RoundUpToTypeBoundary is size_t.
TEST_F(UtilitiesTests, RoundUpToTypeBoundary_ReturnType) {
  auto result = mathkata::RoundUpToTypeBoundary<int>(0);
  static_assert(std::is_same<decltype(result), size_t>::value,
                "RoundUpToTypeBoundary should return size_t");
}

// Zero should remain zero regardless of type alignment.
TEST_F(UtilitiesTests, RoundUpToTypeBoundary_Zero) {
  EXPECT_EQ(0u, mathkata::RoundUpToTypeBoundary<char>(0));
  EXPECT_EQ(0u, mathkata::RoundUpToTypeBoundary<int16_t>(0));
  EXPECT_EQ(0u, mathkata::RoundUpToTypeBoundary<int32_t>(0));
  EXPECT_EQ(0u, mathkata::RoundUpToTypeBoundary<int64_t>(0));
  EXPECT_EQ(0u, mathkata::RoundUpToTypeBoundary<double>(0));
}

// Values already aligned should be returned unchanged.
TEST_F(UtilitiesTests, RoundUpToTypeBoundary_AlreadyAligned) {
  // char has alignment 1, so every value is already aligned.
  EXPECT_EQ(1u, mathkata::RoundUpToTypeBoundary<char>(1));
  EXPECT_EQ(5u, mathkata::RoundUpToTypeBoundary<char>(5));

  // int32_t typically has alignment 4.
  size_t align4 = alignof(int32_t);
  EXPECT_EQ(align4, mathkata::RoundUpToTypeBoundary<int32_t>(align4));
  EXPECT_EQ(align4 * 2, mathkata::RoundUpToTypeBoundary<int32_t>(align4 * 2));
  EXPECT_EQ(align4 * 100,
            mathkata::RoundUpToTypeBoundary<int32_t>(align4 * 100));
}

// Values that are not aligned should be rounded up.
TEST_F(UtilitiesTests, RoundUpToTypeBoundary_RoundsUp) {
  // For int32_t (alignment 4): 1 -> 4, 2 -> 4, 3 -> 4, 5 -> 8.
  size_t align4 = alignof(int32_t);
  if (align4 == 4) {
    EXPECT_EQ(4u, mathkata::RoundUpToTypeBoundary<int32_t>(1));
    EXPECT_EQ(4u, mathkata::RoundUpToTypeBoundary<int32_t>(2));
    EXPECT_EQ(4u, mathkata::RoundUpToTypeBoundary<int32_t>(3));
    EXPECT_EQ(8u, mathkata::RoundUpToTypeBoundary<int32_t>(5));
    EXPECT_EQ(8u, mathkata::RoundUpToTypeBoundary<int32_t>(6));
    EXPECT_EQ(8u, mathkata::RoundUpToTypeBoundary<int32_t>(7));
  }

  // For double (alignment 8): 1 -> 8, 7 -> 8, 9 -> 16.
  size_t align8 = alignof(double);
  if (align8 == 8) {
    EXPECT_EQ(8u, mathkata::RoundUpToTypeBoundary<double>(1));
    EXPECT_EQ(8u, mathkata::RoundUpToTypeBoundary<double>(7));
    EXPECT_EQ(16u, mathkata::RoundUpToTypeBoundary<double>(9));
    EXPECT_EQ(16u, mathkata::RoundUpToTypeBoundary<double>(15));
  }
}

// Verify behavior with char (alignment 1): every value is already aligned.
TEST_F(UtilitiesTests, RoundUpToTypeBoundary_CharAlignment) {
  for (size_t i = 0; i < 100; ++i) {
    EXPECT_EQ(i, mathkata::RoundUpToTypeBoundary<char>(i));
  }
}

// Verify the function works with large values that would overflow uint32_t.
TEST_F(UtilitiesTests, RoundUpToTypeBoundary_LargeValues) {
  // A value larger than UINT32_MAX to verify size_t is used properly.
  if (sizeof(size_t) > 4) {
    size_t large_value = static_cast<size_t>(UINT32_MAX) + 1;
    size_t align = alignof(int32_t);
    // large_value (2^32) should already be aligned to any power-of-2 <= 2^32.
    EXPECT_EQ(large_value,
              mathkata::RoundUpToTypeBoundary<int32_t>(large_value));

    // 2^32 + 1 should round up to 2^32 + align.
    size_t expected = large_value + align;
    EXPECT_EQ(expected,
              mathkata::RoundUpToTypeBoundary<int32_t>(large_value + 1));
  }
}

// Test with various struct types to ensure it works generically.
TEST_F(UtilitiesTests, RoundUpToTypeBoundary_VariousTypes) {
  // int16_t typically has alignment 2.
  size_t align2 = alignof(int16_t);
  if (align2 == 2) {
    EXPECT_EQ(2u, mathkata::RoundUpToTypeBoundary<int16_t>(1));
    EXPECT_EQ(2u, mathkata::RoundUpToTypeBoundary<int16_t>(2));
    EXPECT_EQ(4u, mathkata::RoundUpToTypeBoundary<int16_t>(3));
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

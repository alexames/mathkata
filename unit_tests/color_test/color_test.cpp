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
#include "mathkata/color.h"

#include "gtest/gtest.h"
#include "precision.h"

class ColorTests : public ::testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

// --- Construction ---

TEST_F(ColorTests, DefaultConstruction) {
  mathkata::Color c;
  EXPECT_EQ(c.r, 0);
  EXPECT_EQ(c.g, 0);
  EXPECT_EQ(c.b, 0);
  EXPECT_EQ(c.a, 255);
}

TEST_F(ColorTests, ComponentConstruction) {
  mathkata::Color c(10, 20, 30, 40);
  EXPECT_EQ(c.r, 10);
  EXPECT_EQ(c.g, 20);
  EXPECT_EQ(c.b, 30);
  EXPECT_EQ(c.a, 40);
}

TEST_F(ColorTests, ComponentConstructionDefaultAlpha) {
  mathkata::Color c(100, 150, 200);
  EXPECT_EQ(c.r, 100);
  EXPECT_EQ(c.g, 150);
  EXPECT_EQ(c.b, 200);
  EXPECT_EQ(c.a, 255);
}

// --- Named Constants ---

TEST_F(ColorTests, NamedConstants) {
  EXPECT_EQ(mathkata::Color::Black, mathkata::Color(0, 0, 0, 255));
  EXPECT_EQ(mathkata::Color::White, mathkata::Color(255, 255, 255, 255));
  EXPECT_EQ(mathkata::Color::Red, mathkata::Color(255, 0, 0, 255));
  EXPECT_EQ(mathkata::Color::Green, mathkata::Color(0, 255, 0, 255));
  EXPECT_EQ(mathkata::Color::Blue, mathkata::Color(0, 0, 255, 255));
  EXPECT_EQ(mathkata::Color::Yellow, mathkata::Color(255, 255, 0, 255));
  EXPECT_EQ(mathkata::Color::Cyan, mathkata::Color(0, 255, 255, 255));
  EXPECT_EQ(mathkata::Color::Magenta, mathkata::Color(255, 0, 255, 255));
  EXPECT_EQ(mathkata::Color::Transparent, mathkata::Color(0, 0, 0, 0));
}

// --- Equality ---

TEST_F(ColorTests, Equality) {
  mathkata::Color a(10, 20, 30, 40);
  mathkata::Color b(10, 20, 30, 40);
  mathkata::Color c(10, 20, 30, 50);

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
  EXPECT_FALSE(a != b);
  EXPECT_TRUE(a != c);
}

// --- Clamped Addition ---

TEST_F(ColorTests, Addition) {
  mathkata::Color a(100, 50, 200, 100);
  mathkata::Color b(50, 30, 20, 50);
  mathkata::Color result = a + b;
  EXPECT_EQ(result.r, 150);
  EXPECT_EQ(result.g, 80);
  EXPECT_EQ(result.b, 220);
  EXPECT_EQ(result.a, 150);
}

TEST_F(ColorTests, AdditionOverflow) {
  mathkata::Color a(200, 250, 255, 128);
  mathkata::Color b(100, 100, 1, 200);
  mathkata::Color result = a + b;
  EXPECT_EQ(result.r, 255);
  EXPECT_EQ(result.g, 255);
  EXPECT_EQ(result.b, 255);
  EXPECT_EQ(result.a, 255);
}

// --- Clamped Subtraction ---

TEST_F(ColorTests, Subtraction) {
  mathkata::Color a(100, 80, 200, 150);
  mathkata::Color b(50, 30, 20, 50);
  mathkata::Color result = a - b;
  EXPECT_EQ(result.r, 50);
  EXPECT_EQ(result.g, 50);
  EXPECT_EQ(result.b, 180);
  EXPECT_EQ(result.a, 100);
}

TEST_F(ColorTests, SubtractionUnderflow) {
  mathkata::Color a(10, 5, 0, 50);
  mathkata::Color b(50, 100, 1, 200);
  mathkata::Color result = a - b;
  EXPECT_EQ(result.r, 0);
  EXPECT_EQ(result.g, 0);
  EXPECT_EQ(result.b, 0);
  EXPECT_EQ(result.a, 0);
}

// --- Modulation ---

TEST_F(ColorTests, Modulation) {
  // White * any color = that color.
  mathkata::Color white = mathkata::Color::White;
  mathkata::Color c(100, 150, 200, 255);
  mathkata::Color result = white * c;
  EXPECT_EQ(result.r, 100);
  EXPECT_EQ(result.g, 150);
  EXPECT_EQ(result.b, 200);
  EXPECT_EQ(result.a, 255);
}

TEST_F(ColorTests, ModulationBlack) {
  // Black * any color = black (alpha modulated separately).
  mathkata::Color black = mathkata::Color::Black;
  mathkata::Color c(100, 150, 200, 255);
  mathkata::Color result = black * c;
  EXPECT_EQ(result.r, 0);
  EXPECT_EQ(result.g, 0);
  EXPECT_EQ(result.b, 0);
  EXPECT_EQ(result.a, 255);
}

TEST_F(ColorTests, ModulationHalf) {
  // Modulation with 128 (~50%) should roughly halve the values.
  mathkata::Color a(200, 100, 50, 255);
  mathkata::Color half(128, 128, 128, 255);
  mathkata::Color result = a * half;
  // (200 * 128 + 127) / 255 = 100
  EXPECT_EQ(result.r, 100);
  // (100 * 128 + 127) / 255 = 50
  EXPECT_EQ(result.g, 50);
  // (50 * 128 + 127) / 255 = 25
  EXPECT_EQ(result.b, 25);
  EXPECT_EQ(result.a, 255);
}

// --- Compound Assignment ---

TEST_F(ColorTests, CompoundAddition) {
  mathkata::Color c(100, 50, 200, 100);
  c += mathkata::Color(50, 30, 100, 50);
  EXPECT_EQ(c.r, 150);
  EXPECT_EQ(c.g, 80);
  EXPECT_EQ(c.b, 255);
  EXPECT_EQ(c.a, 150);
}

TEST_F(ColorTests, CompoundSubtraction) {
  mathkata::Color c(100, 50, 200, 100);
  c -= mathkata::Color(50, 30, 20, 50);
  EXPECT_EQ(c.r, 50);
  EXPECT_EQ(c.g, 20);
  EXPECT_EQ(c.b, 180);
  EXPECT_EQ(c.a, 50);
}

TEST_F(ColorTests, CompoundModulation) {
  mathkata::Color c(200, 100, 50, 255);
  c *= mathkata::Color::White;
  EXPECT_EQ(c.r, 200);
  EXPECT_EQ(c.g, 100);
  EXPECT_EQ(c.b, 50);
  EXPECT_EQ(c.a, 255);
}

// --- normalized Conversion ---

TEST_F(ColorTests, toNormalized) {
  mathkata::Color c(255, 0, 128, 255);
  mathkata::Vector<float, 4> v = c.toNormalized();
  EXPECT_NEAR(v[0], 1.0f, FLOAT_PRECISION);
  EXPECT_NEAR(v[1], 0.0f, FLOAT_PRECISION);
  EXPECT_NEAR(v[2], 128.0f / 255.0f, FLOAT_PRECISION);
  EXPECT_NEAR(v[3], 1.0f, FLOAT_PRECISION);
}

TEST_F(ColorTests, fromNormalized) {
  mathkata::Vector<float, 4> v(1.0f, 0.0f, 0.5f, 1.0f);
  mathkata::Color c = mathkata::Color::fromNormalized(v);
  EXPECT_EQ(c.r, 255);
  EXPECT_EQ(c.g, 0);
  EXPECT_EQ(c.b, 128);
  EXPECT_EQ(c.a, 255);
}

TEST_F(ColorTests, FromNormalizedClamped) {
  mathkata::Vector<float, 4> v(1.5f, -0.5f, 0.5f, 2.0f);
  mathkata::Color c = mathkata::Color::fromNormalized(v);
  EXPECT_EQ(c.r, 255);
  EXPECT_EQ(c.g, 0);
  EXPECT_EQ(c.b, 128);
  EXPECT_EQ(c.a, 255);
}

TEST_F(ColorTests, NormalizedRoundTrip) {
  mathkata::Color original(42, 128, 200, 100);
  mathkata::Vector<float, 4> normalized = original.toNormalized();
  mathkata::Color recovered = mathkata::Color::fromNormalized(normalized);
  EXPECT_EQ(recovered, original);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

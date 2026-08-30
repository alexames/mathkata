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
#include <cstdio>
#include <type_traits>

#include "gtest/gtest.h"
#include "mathkata/aabb.h"
#include "mathkata/capsule.h"
#include "mathkata/color.h"
#include "mathkata/frustum.h"
#include "mathkata/matrix.h"
#include "mathkata/plane.h"
#include "mathkata/quaternion.h"
#include "mathkata/ray.h"
#include "mathkata/rect.h"
#include "mathkata/sphere.h"
#include "mathkata/transform.h"
#include "mathkata/utilities.h"
#include "mathkata/vector.h"

namespace {

using mathkata::AABB;
using mathkata::Capsule;
using mathkata::Frustum;
using mathkata::Line;
using mathkata::LineSegment;
using mathkata::Matrix;
using mathkata::Plane;
using mathkata::Quaternion;
using mathkata::Ray;
using mathkata::Sphere;
using mathkata::Vector;
using mathkata::VectorPacked;

// Deleting the default constructor is the whole mechanism: it is what turns
// a value silently left unassigned into a compile error.
static_assert(!std::is_default_constructible_v<Vector<float, 2>>);
static_assert(!std::is_default_constructible_v<Vector<float, 3>>);
static_assert(!std::is_default_constructible_v<Vector<float, 4>>);
static_assert(!std::is_default_constructible_v<Vector<float, 5>>);
static_assert(!std::is_default_constructible_v<Vector<int, 2>>);
static_assert(!std::is_default_constructible_v<Vector<double, 4>>);
static_assert(!std::is_default_constructible_v<VectorPacked<float, 3>>);
static_assert(!std::is_default_constructible_v<VectorPacked<float, 5>>);
static_assert(!std::is_default_constructible_v<Matrix<float, 4, 4>>);
static_assert(!std::is_default_constructible_v<Quaternion<float>>);
static_assert(!std::is_default_constructible_v<AABB<float, 3>>);
static_assert(!std::is_default_constructible_v<Capsule<float, 3>>);
static_assert(!std::is_default_constructible_v<Frustum<float>>);
static_assert(!std::is_default_constructible_v<Plane<float>>);
static_assert(!std::is_default_constructible_v<Ray<float, 3>>);
static_assert(!std::is_default_constructible_v<Line<float, 3>>);
static_assert(!std::is_default_constructible_v<LineSegment<float, 3>>);
static_assert(!std::is_default_constructible_v<Sphere<float, 3>>);

// The types that assign every member for you keep their default constructor.
static_assert(std::is_default_constructible_v<mathkata::Rect<float>>);
static_assert(std::is_default_constructible_v<mathkata::Color>);
static_assert(std::is_default_constructible_v<mathkata::Transform<float>>);

// The point of the deletion: a type that forgets to initialize its vector
// member stops compiling, instead of holding whatever the stack left there.
struct Forgetful {
  Vector<float, 2> position;
  int tag;
};
static_assert(!std::is_default_constructible_v<Forgetful>);

// The same type with the member spelled out stays constructible.
struct Careful {
  Vector<float, 2> position{0.0f, 0.0f};
  int tag{0};
};
static_assert(std::is_default_constructible_v<Careful>);

/// @brief An element that counts how many times it is copied.
///
/// UninitializedArray's contract is that it constructs its elements in place.
/// Nothing about the mathkata types themselves can show that, so this stands in
/// for one: it has the same shape (no default constructor, a static
/// uninitialized()) and a copy constructor that leaves a trace.
struct CountingElement {
  /// @brief Number of copies made since a test last reset it.
  static int copies;

  int value;

  CountingElement() = delete;
  CountingElement(const CountingElement& other) : value(other.value) {
    ++copies;
  }
  CountingElement& operator=(const CountingElement&) = default;

  /// @brief Create an element without assigning its value.
  ///
  /// @return An element whose value is not assigned.
  static CountingElement uninitialized() { return CountingElement(Tag{}); }

 private:
  /// Selects the constructor that leaves the value unassigned.
  struct Tag {};

  /// @brief Create an element without assigning its value.
  ///
  /// @param tag Unused; selects this constructor.
  explicit CountingElement(Tag tag) { static_cast<void>(tag); }
};

int CountingElement::copies = 0;

}  // namespace

class UninitializedTests : public ::testing::Test {};

// There is deliberately no test that uninitialized() leaves the destination
// bytes untouched. Two platform details make that unobservable rather than
// untrue: a trivially copyable type small enough to return in registers is
// stored to the destination by the caller, and GCC folds a constexpr
// uninitialized() to a zeroed value when nothing overwrites it. Neither
// reaches real code, where every element is assigned immediately -- the
// generated hot paths are unchanged -- but both defeat a byte comparison.
// What is portable is that no copy is made, which the array test below counts.

// The array form has to be free for the same reason, and it is the one that is
// easy to get wrong: building the elements in a helper that returns the array
// by value copies them out of a temporary. A byte comparison cannot see that,
// since copying indeterminate bytes can reproduce the same pattern, so the
// element counts its own copies instead.
TEST_F(UninitializedTests, UninitializedArrayConstructsInPlace) {
  CountingElement::copies = 0;

  mathkata::UninitializedArray<CountingElement, 4> values;
  values[0].value = 7;

  EXPECT_EQ(CountingElement::copies, 0);
  EXPECT_EQ(values[0].value, 7);
  EXPECT_EQ(values.size(), 4u);
}

// An uninitialized value is still a usable object: assigning every element and
// reading it back is the case the factory exists for.
TEST_F(UninitializedTests, UninitializedValuesAcceptAssignment) {
  auto v = Vector<float, 4>::uninitialized();
  v[0] = 1.0f;
  v[1] = 2.0f;
  v[2] = 3.0f;
  v[3] = 4.0f;

  EXPECT_EQ(v[0], 1.0f);
  EXPECT_EQ(v[3], 4.0f);

  auto m = Matrix<float, 2, 2>::uninitialized();
  for (int i = 0; i < 4; ++i) {
    m[i] = static_cast<float>(i);
  }

  EXPECT_EQ(m[0], 0.0f);
  EXPECT_EQ(m[3], 3.0f);

  auto q = Quaternion<float>::uninitialized();
  q = Quaternion<float>(1.0f, 0.0f, 0.0f, 0.0f);

  EXPECT_EQ(q.scalar(), 1.0f);
}

// The geometry types hold vectors, so their uninitialized() has to hand each
// member its own uninitialized vector rather than default-construct it.
TEST_F(UninitializedTests, GeometryTypesAcceptAssignment) {
  auto box = AABB<float, 3>::uninitialized();
  box.min = Vector<float, 3>(1.0f, 2.0f, 3.0f);
  box.max = Vector<float, 3>(4.0f, 5.0f, 6.0f);
  EXPECT_EQ(box.min[0], 1.0f);
  EXPECT_EQ(box.max[2], 6.0f);

  auto capsule = Capsule<float, 3>::uninitialized();
  capsule.radius = 2.0f;
  EXPECT_EQ(capsule.radius, 2.0f);

  auto plane = Plane<float>::uninitialized();
  plane.normal = Vector<float, 3>(0.0f, 1.0f, 0.0f);
  plane.distance = 5.0f;
  EXPECT_EQ(plane.distance, 5.0f);

  auto sphere = Sphere<float, 3>::uninitialized();
  sphere.radius = 3.0f;
  EXPECT_EQ(sphere.radius, 3.0f);

  auto ray = Ray<float, 3>::uninitialized();
  ray.origin = Vector<float, 3>(0.0f, 0.0f, 0.0f);
  EXPECT_EQ(ray.origin[1], 0.0f);

  auto line = Line<float, 3>::uninitialized();
  line.point = Vector<float, 3>(7.0f, 0.0f, 0.0f);
  EXPECT_EQ(line.point[0], 7.0f);

  auto segment = LineSegment<float, 3>::uninitialized();
  segment.start = Vector<float, 3>(1.0f, 1.0f, 1.0f);
  EXPECT_EQ(segment.start[2], 1.0f);

  // Frustum is the array case: six planes, each needing its own value.
  auto frustum = Frustum<float>::uninitialized();
  frustum.planes[0] = Plane<float>(Vector<float, 3>(1.0f, 0.0f, 0.0f), 9.0f);
  EXPECT_EQ(frustum.planes[0].distance, 9.0f);
}

// Dimension 5 has no specialization, so it is the only way to reach the
// primary Vector and VectorPacked templates.
TEST_F(UninitializedTests, PrimaryTemplatesAcceptAssignment) {
  auto v = Vector<float, 5>::uninitialized();
  for (int i = 0; i < 5; ++i) {
    v[i] = static_cast<float>(i);
  }
  EXPECT_EQ(v[4], 4.0f);

  auto packed = VectorPacked<float, 5>::uninitialized();
  for (int i = 0; i < 5; ++i) {
    packed.data_[i] = static_cast<float>(i * 2);
  }
  EXPECT_EQ(packed.data_[4], 8.0f);
}

// `T values[Count];` does not compile for these types, so the library owes
// callers a replacement for it.
TEST_F(UninitializedTests, UninitializedArrayGivesEveryElement) {
  mathkata::UninitializedArray<Vector<float, 3>, 4> axes;
  static_assert(axes.size() == 4);

  for (std::size_t i = 0; i < axes.size(); ++i) {
    axes[i] = Vector<float, 3>(static_cast<float>(i), 0.0f, 0.0f);
  }

  EXPECT_EQ(axes[0][0], 0.0f);
  EXPECT_EQ(axes[3][0], 3.0f);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

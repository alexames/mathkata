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
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <new>
#include <type_traits>

#include "gtest/gtest.h"
#include "mathkata/aabb.h"
#include "mathkata/capsule.h"
#include "mathkata/frustum.h"
#include "mathkata/matrix.h"
#include "mathkata/plane.h"
#include "mathkata/quaternion.h"
#include "mathkata/ray.h"
#include "mathkata/sphere.h"
#include "mathkata/utilities.h"
#include "mathkata/vector.h"

// The types under test leave their elements indeterminate on purpose, which
// only `= default` can express: it keeps `T v;` free while still letting
// `T v{}` zero the object. Value-initializing on the stack and reading zeroes
// would prove nothing, since stack bytes are usually zero already.
// Constructing into storage filled with 0x7F removes that luck: a byte the
// constructor never writes reads back as part of ~3.4e38, never as zero.
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

constexpr std::byte kPoison{0x7F};

// A user-provided default constructor would run in place of the zeroing that
// value-initialization performs, so every zero checked below depends on these
// constructors staying trivial.
static_assert(std::is_trivially_default_constructible_v<Vector<float, 2>>);
static_assert(std::is_trivially_default_constructible_v<Vector<float, 3>>);
static_assert(std::is_trivially_default_constructible_v<Vector<float, 4>>);
static_assert(std::is_trivially_default_constructible_v<Vector<float, 5>>);
static_assert(std::is_trivially_default_constructible_v<Vector<int, 2>>);
static_assert(std::is_trivially_default_constructible_v<Vector<double, 4>>);
static_assert(
    std::is_trivially_default_constructible_v<VectorPacked<float, 3>>);
static_assert(
    std::is_trivially_default_constructible_v<VectorPacked<float, 5>>);
static_assert(std::is_trivially_default_constructible_v<Matrix<float, 4, 4>>);
static_assert(std::is_trivially_default_constructible_v<Quaternion<float>>);
static_assert(std::is_trivially_default_constructible_v<AABB<float, 3>>);
static_assert(std::is_trivially_default_constructible_v<Capsule<float, 3>>);
static_assert(std::is_trivially_default_constructible_v<Frustum<float>>);
static_assert(std::is_trivially_default_constructible_v<Plane<float>>);
static_assert(std::is_trivially_default_constructible_v<Ray<float, 3>>);
static_assert(std::is_trivially_default_constructible_v<Line<float, 3>>);
static_assert(std::is_trivially_default_constructible_v<LineSegment<float, 3>>);
static_assert(std::is_trivially_default_constructible_v<Sphere<float, 3>>);

/// @brief Constructs a T with T{} inside storage pre-filled with kPoison.
///
/// The object is never destroyed, which the trivially-destructible constraint
/// makes harmless. For an aggregate T, T{} is aggregate initialization rather
/// than value initialization, so its members zero but its padding does not.
///
/// @tparam T Type to construct; must be trivially destructible.
template <typename T>
class PoisonedValue {
 public:
  PoisonedValue() {
    std::fill(m_storage.begin(), m_storage.end(), kPoison);
    m_value = new (static_cast<void*>(m_storage.data())) T{};
  }

  PoisonedValue(const PoisonedValue&) = delete;
  PoisonedValue& operator=(const PoisonedValue&) = delete;

  /// @brief The constructed object.
  /// @return A reference valid until this PoisonedValue dies.
  const T& operator*() const { return *m_value; }

  /// @brief Member access on the constructed object.
  /// @return A pointer valid until this PoisonedValue dies.
  const T* operator->() const { return m_value; }

 private:
  static_assert(std::is_trivially_destructible_v<T>);

  alignas(T) std::array<std::byte, sizeof(T)> m_storage{};
  T* m_value{nullptr};
};

/// @brief Whether every byte of the object representation of v is zero.
///
/// Padding is the subtle half of the guarantee, and it is where the types
/// diverge: Vector<float, 3> carries a fourth lane addressable through simd3
/// in a padded SIMD build, and Plane, Sphere and Capsule pad their trailing
/// scalar out to the vector's alignment.
///
/// @tparam T Type whose object representation is inspected.
/// @param v Object whose bytes to inspect; not retained.
/// @return true if every byte, padding included, is zero.
template <typename T>
bool allBytesZero(const T& v) {
  std::array<std::byte, sizeof(T)> bytes{};
  std::memcpy(bytes.data(), &v, sizeof(T));
  return std::all_of(bytes.begin(), bytes.end(),
                     [](std::byte b) { return b == std::byte{0}; });
}

}  // namespace

class ZeroInitTests : public ::testing::Test {};

TEST_F(ZeroInitTests, Vector2ValueInitializesToZero) {
  const PoisonedValue<Vector<float, 2>> v;

  EXPECT_EQ(v->x, 0.0f);
  EXPECT_EQ(v->y, 0.0f);
  EXPECT_TRUE(allBytesZero(*v));
}

TEST_F(ZeroInitTests, Vector3ValueInitializesToZero) {
  const PoisonedValue<Vector<float, 3>> v;

  EXPECT_EQ(v->x, 0.0f);
  EXPECT_EQ(v->y, 0.0f);
  EXPECT_EQ(v->z, 0.0f);
  EXPECT_TRUE(allBytesZero(*v));
}

TEST_F(ZeroInitTests, Vector4ValueInitializesToZero) {
  const PoisonedValue<Vector<float, 4>> v;

  EXPECT_EQ(v->x, 0.0f);
  EXPECT_EQ(v->y, 0.0f);
  EXPECT_EQ(v->z, 0.0f);
  EXPECT_EQ(v->w, 0.0f);
  EXPECT_TRUE(allBytesZero(*v));
}

// Dimension 5 has no specialization, so it is the only way to reach the primary
// Vector and VectorPacked templates.
TEST_F(ZeroInitTests, PrimaryTemplatesValueInitializeToZero) {
  const PoisonedValue<Vector<float, 5>> v;
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ((*v)[i], 0.0f) << "element " << i;
  }
  EXPECT_TRUE(allBytesZero(*v));

  const PoisonedValue<VectorPacked<float, 5>> packed;
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(packed->data_[i], 0.0f) << "element " << i;
  }
  EXPECT_TRUE(allBytesZero(*packed));
}

TEST_F(ZeroInitTests, VectorPackedValueInitializesToZero) {
  const PoisonedValue<VectorPacked<float, 3>> v;

  EXPECT_EQ(v->x, 0.0f);
  EXPECT_EQ(v->y, 0.0f);
  EXPECT_EQ(v->z, 0.0f);
  EXPECT_TRUE(allBytesZero(*v));
}

TEST_F(ZeroInitTests, MatrixValueInitializesToZero) {
  const PoisonedValue<Matrix<float, 4, 4>> m;

  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ((*m)[i], 0.0f) << "element " << i;
  }
  EXPECT_TRUE(allBytesZero(*m));
}

TEST_F(ZeroInitTests, QuaternionValueInitializesToZero) {
  const PoisonedValue<Quaternion<float>> q;

  EXPECT_EQ(q->scalar(), 0.0f);
  EXPECT_EQ(q->vector()[0], 0.0f);
  EXPECT_EQ(q->vector()[1], 0.0f);
  EXPECT_EQ(q->vector()[2], 0.0f);
  EXPECT_TRUE(allBytesZero(*q));
}

TEST_F(ZeroInitTests, GeometryTypesValueInitializeToZero) {
  const PoisonedValue<AABB<float, 3>> box;
  EXPECT_EQ(box->min[0], 0.0f);
  EXPECT_EQ(box->max[2], 0.0f);
  EXPECT_TRUE(allBytesZero(*box));

  const PoisonedValue<Capsule<float, 3>> capsule;
  EXPECT_EQ(capsule->start[0], 0.0f);
  EXPECT_EQ(capsule->end[2], 0.0f);
  EXPECT_EQ(capsule->radius, 0.0f);
  EXPECT_TRUE(allBytesZero(*capsule));

  const PoisonedValue<Plane<float>> plane;
  EXPECT_EQ(plane->normal[1], 0.0f);
  EXPECT_EQ(plane->distance, 0.0f);
  EXPECT_TRUE(allBytesZero(*plane));

  const PoisonedValue<Sphere<float, 3>> sphere;
  EXPECT_EQ(sphere->center[0], 0.0f);
  EXPECT_EQ(sphere->radius, 0.0f);
  EXPECT_TRUE(allBytesZero(*sphere));

  const PoisonedValue<Ray<float, 3>> ray;
  EXPECT_EQ(ray->origin[0], 0.0f);
  EXPECT_EQ(ray->direction[0], 0.0f);
  EXPECT_TRUE(allBytesZero(*ray));

  const PoisonedValue<Line<float, 3>> line;
  EXPECT_EQ(line->point[0], 0.0f);
  EXPECT_EQ(line->direction[0], 0.0f);
  EXPECT_TRUE(allBytesZero(*line));

  const PoisonedValue<LineSegment<float, 3>> segment;
  EXPECT_EQ(segment->start[0], 0.0f);
  EXPECT_EQ(segment->end[0], 0.0f);
  EXPECT_TRUE(allBytesZero(*segment));

  const PoisonedValue<Frustum<float>> frustum;
  EXPECT_EQ(frustum->planes[0].normal[0], 0.0f);
  EXPECT_EQ(frustum->planes[5].distance, 0.0f);
  EXPECT_TRUE(allBytesZero(*frustum));
}

// A Vector member of an enclosing type zeroes only because the Vector's own
// default constructor is trivial.
TEST_F(ZeroInitTests, VectorMemberOfEnclosingTypeZeroes) {
  struct Aggregate {
    Vector<float, 2> position;
    int tag;
  };

  const PoisonedValue<Aggregate> aggregate;
  EXPECT_EQ(aggregate->position.x, 0.0f);
  EXPECT_EQ(aggregate->position.y, 0.0f);
  EXPECT_EQ(aggregate->tag, 0);
}

// Default-initialization writes nothing: a member initializer here would make
// every `Vector<T, D> v;` in a hot loop pay for a store. The bytes are
// inspected as std::byte rather than read back as floats, which would be a
// read of an indeterminate value.
TEST_F(ZeroInitTests, DefaultInitializationStillWritesNothing) {
  using Vec4 = Vector<float, 4>;
  alignas(Vec4) std::array<std::byte, sizeof(Vec4)> storage{};
  std::fill(storage.begin(), storage.end(), kPoison);

  new (static_cast<void*>(storage.data())) Vec4;

  EXPECT_TRUE(std::all_of(storage.begin(), storage.end(),
                          [](std::byte b) { return b == kPoison; }));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  printf("%s (%s)\n", argv[0], MATHKATA_BUILD_OPTIONS_STRING);
  return RUN_ALL_TESTS();
}

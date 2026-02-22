# MathKata

MathKata is a C++ math library for games, focused on simplicity and efficiency.
It is a modernized fork of Google's archived
[MathFu](https://github.com/google/mathfu) library.

## Features

**Core types:**

- `Vector<T, D>` - N-dimensional vector (1D through 4D, with SIMD-optimized
  specializations for `float` vectors)
- `Matrix<T, Rows, Cols>` - N x M matrix (with SIMD-optimized 4x4 `float`
  specialization)
- `Quaternion<T>` - unit quaternion for 3D rotations

**Geometry types:**

- `Rect<T>` - 2D axis-aligned rectangle
- `AABB<T, D>` - N-dimensional axis-aligned bounding box
- `Sphere<T, D>` - N-dimensional bounding sphere
- `Capsule<T, D>` - N-dimensional capsule (line segment + radius)
- `Ray<T, D>`, `Line<T, D>`, `LineSegment<T, D>` - ray, line, and line segment
- `Plane<T>` - 3D plane (normal + distance)
- `Transform<T>` - position + rotation + scale transform
- `Frustum<T>` - view frustum (6 planes)

**Utilities:**

- Intersection tests (ray-sphere, ray-AABB, ray-plane, AABB-AABB,
  sphere-sphere, sphere-AABB, point containment)
- GLSL and HLSL type aliases (`vec2`, `mat4`, `float4x4`, etc.)
- `constexpr` support for all functions that don't require transcendentals or
  SIMD intrinsics

## Improvements over MathFu

- **C++20** - requires a C++20-capable compiler
- **constexpr** - most constructors, operators, and pure-arithmetic functions are
  `constexpr`
- **xsimd** - SIMD backend replaced from the unmaintained vectorial library to
  [xsimd](https://github.com/xtensor-stack/xsimd), a modern header-only SIMD
  abstraction library
- **Compact Quaternion** - quaternion storage changed from separate scalar +
  `Vector<T, 3>` to a single `Vector<T, 4>`
- **Geometry types** - added Rect, AABB, Sphere, Capsule, Ray, Line,
  LineSegment, Plane, Transform, Frustum, and intersection tests
- **vcpkg** - dependencies managed via vcpkg instead of git submodules

## Requirements

- C++20 compiler (MSVC 2022, GCC 11+, Clang 14+)
- CMake 3.14+
- [vcpkg](https://vcpkg.io/) (for xsimd and googletest dependencies)

## Building

```bash
# Clone
git clone https://github.com/alexames/mathfu2.git
cd mathfu2

# Configure (Visual Studio - recommended on Windows)
cmake -G "Visual Studio 17 2022" -A x64 -B Build

# Configure (Ninja - requires compiler in PATH)
cmake -G Ninja -B BuildNinja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build Build --config Release

# Run tests
cd Build && ctest --output-on-failure -C Release
```

### CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `mathkata_enable_simd` | `ON` | Use SIMD implementations when available |
| `mathkata_build_benchmarks` | `ON` | Build benchmarks |
| `mathkata_build_tests` | `ON` | Build unit tests |

## SIMD support

When `mathkata_enable_simd` is enabled, MathKata uses
[xsimd](https://github.com/xtensor-stack/xsimd) to provide SIMD-optimized
implementations for `Vector<float, 3>`, `Vector<float, 4>`, and
`Matrix<float, 4, 4>`:

- **x86/x64**: SSE2 (with SSE4.1 enabled for GCC/Clang)
- **ARM**: NEON

Non-SIMD fallbacks are used automatically when SIMD is disabled or for types
that don't have SIMD specializations (e.g. `double` vectors,
`Vector<float, 2>`).

## Usage

```cpp
#include "mathkata/vector.h"
#include "mathkata/matrix.h"
#include "mathkata/quaternion.h"

using namespace mathkata;

// Vectors
Vector<float, 3> position(1.0f, 2.0f, 3.0f);
Vector<float, 3> direction = position.Normalized();
float length = position.Length();

// Matrices
Matrix<float, 4> view = Matrix<float, 4>::LookAt(
    Vector<float, 3>(0, 0, 5),   // eye
    Vector<float, 3>(0, 0, 0),   // target
    Vector<float, 3>(0, 1, 0));  // up

// Quaternions
Quaternion<float> rotation =
    Quaternion<float>::FromAngleAxis(0.5f, Vector<float, 3>(0, 1, 0));
Vector<float, 3> rotated = rotation.Rotate(position);
```

## License

MathKata is licensed under the Apache License, Version 2.0.
See [LICENSE](LICENSE) for the full license text.

Originally developed by Google Inc. Forked and maintained by
[Alex Ames](https://github.com/alexames).

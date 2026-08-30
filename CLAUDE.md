# Claude Code Project Instructions

## Project Overview

MathKata is a header-only C++ math library for games, forked from Google's
archived [MathFu](https://github.com/google/mathfu). The repository is
[alexames/mathkata](https://github.com/alexames/mathkata).

### Key Types

| Header | Types |
|--------|-------|
| `vector.h` | `Vector<T, D>` (1D-4D, SIMD float specializations) |
| `matrix.h` | `Matrix<T, Rows, Cols>` (SIMD 4x4 float specialization) |
| `quaternion.h` | `Quaternion<T>` (stores as `Vector<T, 4>`) |
| `rect.h` | `Rect<T>` (2D axis-aligned rectangle) |
| `aabb.h` | `AABB<T, D>` (N-dimensional bounding box) |
| `sphere.h` | `Sphere<T, D>` (N-dimensional bounding sphere) |
| `capsule.h` | `Capsule<T, D>` (line segment + radius) |
| `ray.h` | `Ray<T, D>`, `Line<T, D>`, `LineSegment<T, D>` |
| `plane.h` | `Plane<T>` (normal + distance) |
| `transform.h` | `Transform<T>` (position + rotation + scale) |
| `frustum.h` | `Frustum<T>` (6 clipping planes) |
| `intersections.h` | Free functions for intersection tests |
| `glsl_mappings.h` | GLSL-style aliases (`vec2`, `mat4`, etc.) |
| `hlsl_mappings.h` | HLSL-style aliases (`float4x4`, etc.) |
| `constants.h` | Common math constants |
| `utilities.h` | SIMD alignment, build options, helper macros |
| `io.h` | `operator<<` for vectors and matrices |

All headers live under `include/mathkata/`. Internal SIMD specializations are
in `include/mathkata/internal/`.

### Architecture

- **Header-only**: No .cpp files to compile; just add the include path
- **C++20 required**: Uses `std::numbers`, concepts
- **constexpr**: Most constructors, operators, and pure-arithmetic functions are
  `constexpr`. Functions using sqrt/sin/cos/SIMD intrinsics remain `inline`.
- **SIMD via xsimd**: `Vector<float, 3>`, `Vector<float, 4>`, and
  `Matrix<float, 4, 4>` have SIMD-optimized specializations using
  [xsimd](https://github.com/xtensor-stack/xsimd). The compatibility layer is
  in `internal/simd_helpers.h`.
- **Namespace**: `mathkata`
- **Macro prefix**: `MATHKATA_`

### Dependencies (via vcpkg)

- **xsimd** - SIMD abstraction (SSE2/SSE4.1 on x86, NEON on ARM)
- **gtest** - Google Test for unit tests

---

## Build Commands

```bash
# Configure (Visual Studio - recommended on Windows, no vcvars needed)
cmake -G "Visual Studio 17 2022" -A x64 -B Build

# Configure (Ninja - requires compiler in PATH)
cmake -G Ninja -B BuildNinja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build Build --config Release

# Run tests
cd Build && ctest --output-on-failure -C Release
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `mathkata_enable_simd` | `ON` | SIMD optimizations |
| `mathkata_build_benchmarks` | `ON` | Build benchmarks |
| `mathkata_build_tests` | `ON` | Build unit tests |

---

## Releasing a New Version

MathKata is distributed via a custom vcpkg registry at
`alexames/vcpkg-registry` (one directory up from this repo at
`../vcpkg-registry`). To release a new version:

1. Ensure all changes are merged to `main` and tests pass.
2. Get the HEAD commit hash:
   ```bash
   git rev-parse HEAD
   ```
3. From the vcpkg-registry directory, run `vcpkger.py update`:
   ```bash
   cd ../vcpkg-registry
   python vcpkger.py update mathkata \
     --github-repo alexames/mathkata \
     --commit-hash <HASH> \
     --version <VERSION>
   ```
   The script will update `ports/mathkata/portfile.cmake` (REF and SHA512)
   and `ports/mathkata/vcpkg.json` (version number). It pauses for review —
   verify that the header-only portfile structure is preserved (it should use
   `file(INSTALL ...)` to copy headers, not `vcpkg_cmake_configure`).
4. If running non-interactively (e.g., from Claude Code), complete the
   remaining steps manually after the script updates the port files:
   ```bash
   git add ports/mathkata/ && git commit -m "[mathkata] Updated port to version <VERSION>"
   git rev-parse HEAD:ports/mathkata   # get the git-tree hash
   ```
   Then prepend a new entry to `versions/m-/mathkata.json` with the git-tree
   hash, update the baseline in `versions/baseline.json`, amend the commit
   to include the version files, and push.
5. Push the registry changes:
   ```bash
   git push
   ```

### Version Numbering

Follow [Semantic Versioning](https://semver.org/):
- **Major** (X.0.0): Breaking API changes (e.g., renaming public functions)
- **Minor** (0.X.0): New features, backwards-compatible
- **Patch** (0.0.X): Bug fixes, backwards-compatible

---

## Git Workflow

### Never Commit Directly to Main

All changes go through feature/bugfix branches with PRs. The main branch
should consist entirely of merge commits.

### Branch Naming

| Type | Prefix | Example |
|------|--------|---------|
| Feature | `feature/` | `feature/geometry-types` |
| Bug fix | `bug/` | `bug/simd-alignment` |
| Refactor | `refactor/` | `refactor/quaternion-storage` |
| Docs | `docs/` | `docs/api-reference` |
| Tests | `test/` | `test/intersection-coverage` |

### Commit Guidelines

- Small, focused commits (one logical change each)
- Descriptive messages starting with a capital letter
- **No conventional commit prefixes** (`feat:`, `fix:`, etc.)
- Run clang-format on changed files before committing

### Pre-Commit Checklist

- [ ] `clang-format -i` applied to all changed `.h` and `.cpp` files
- [ ] Code compiles without warnings
- [ ] All tests pass (`ctest --output-on-failure -C Release`)
- [ ] Branch named appropriately

---

## Testing

Tests use Google Test. Each test module builds in multiple variants when SIMD
is enabled:

- `<name>_tests` - default configuration
- `<name>_simd_padding_tests` - SIMD + padding
- `<name>_simd_no_padding_tests` - SIMD + no padding
- `<name>_no_simd_tests` - SIMD disabled

This produces 5390 total test cases across 16 test modules.

Test files are in `unit_tests/<type>_test/<type>_test.cpp` with a shared
`unit_tests/precision.h` header for floating-point comparison tolerances.

---

## Naming Conventions

- **Function names**: Use `camelCase` for all public free functions and member
  functions (e.g., `crossProduct`, `normalize`, `dotProduct`). Do not use
  PascalCase or snake_case for function names.
- **Type names**: Use `PascalCase` for types and classes (e.g., `Vector`,
  `Matrix`, `Quaternion`).
- **Macro names**: Use `MATHKATA_` prefix with `UPPER_SNAKE_CASE`.
- **Namespace**: `mathkata` (lowercase).

---

## Code Patterns

### Adding a New Header Type

1. Create `include/mathkata/mytype.h` with include guard `MATHKATA_MYTYPE_H_`
2. Put everything in `namespace mathkata`
3. Use `constexpr` for pure-arithmetic functions, `inline` for anything using
   sqrt/sin/cos/SIMD
4. Add `operator==`, `operator!=` as free functions
5. Add unit tests in `unit_tests/mytype_test/mytype_test.cpp`
6. Register the test in `unit_tests/CMakeLists.txt` via `test_executables()`

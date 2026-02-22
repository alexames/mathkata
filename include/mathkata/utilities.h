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
#ifndef MATHKATA_UTILITIES_H_
#define MATHKATA_UTILITIES_H_

#include <algorithm>
#include <cassert>
#include <climits>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <type_traits>

/// @file mathkata/utilities.h Utilities
/// @brief Utility macros and functions.

/// @addtogroup mathkata_build_config
///
/// By default MathKata will attempt to build with SIMD optimizations enabled
/// based upon the target architecture and compiler options.  However, it's
/// possible to change the default build configuration using the following
/// macros:
///
/// @li @ref MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT
/// @li @ref MATHKATA_COMPILE_FORCE_PADDING
///
/// <table>
/// <tr>
///   <th>MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT</th>
///   <th>MATHKATA_COMPILE_FORCE_PADDING</th>
///   <th>Configuration</th>
/// </tr>
/// <tr>
///   <td><em>undefined</em></td>
///   <td><em>undefined</em> or 1</td>
///   <td>Default build configuration, SIMD optimization is enabled based upon
///       the target architecture, compiler options and MathKata library
///       support.</td>
/// </tr>
/// <tr>
///   <td><em>undefined</em></td>
///   <td>0</td>
///   <td>If SIMD is supported, padding of data structures is disabled.  See
///       @ref MATHKATA_COMPILE_FORCE_PADDING for more information.</td>
/// </tr>
/// <tr>
///   <td><em>defined</em></td>
///   <td><em>undefined/0/1</em></td>
///   <td>Builds MathKata with explicit SIMD optimization disabled.  The
///   compiler
///       could still potentially optimize some code paths with SIMD
///       instructions based upon the compiler options.</td>
/// </tr>
/// </table>

#ifdef DOXYGEN
/// @addtogroup mathkata_build_config
/// @{
/// @def MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT
/// @brief Disable SIMD build configuration.
///
/// When defined, this macro <b>disables</b> the default behavior of trying to
/// build the library with SIMD enabled  based upon the target architecture
/// and compiler options.
///
/// To use this build option, this macro <b>must</b> be defined in all modules
/// of the project.
#define MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT
/// @}
#endif  // DOXYGEN
#if !defined(MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT)
#if defined(__SSE__)
#define MATHKATA_COMPILE_WITH_SIMD
#elif defined(__ARM_NEON__)
#define MATHKATA_COMPILE_WITH_SIMD
#elif defined(_M_X64) || defined(_M_AMD64)  // MSVC x64: SSE2 always available
#define MATHKATA_COMPILE_WITH_SIMD
#elif defined(_M_IX86_FP)  // MSVC x86
#if _M_IX86_FP >= 1        // SSE enabled
#define MATHKATA_COMPILE_WITH_SIMD
#endif  // _M_IX86_FP >= 1
#endif
#endif  // !defined(MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT)

#ifdef DOXYGEN
/// @addtogroup mathkata_build_config
/// @{
/// @def MATHKATA_COMPILE_FORCE_PADDING
/// @brief Enable / disable padding of data structures.
///
/// By default, when @ref MATHKATA_COMPILE_FORCE_PADDING is <b>not</b> defined,
/// data structures are padded when SIMD is enabled
/// (i.e when @ref MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT is also not defined).
///
/// If @ref MATHKATA_COMPILE_FORCE_PADDING is defined as <b>1</b>, all data
/// structures are padded to a power of 2 size which enables more efficient
/// SIMD operations.  This  is the default build configuration when SIMD is
/// enabled.
///
/// If @ref MATHKATA_COMPILE_FORCE_PADDING is defined as <b>0</b>, all data
/// structures are packed by the compiler (with no padding) even when the SIMD
/// build configuration is enabled.  This build option can be useful in the
/// rare occasion an application is CPU memory bandwidth constrained, at the
/// expense of additional instructions to copy to / from SIMD registers.
///
/// To use this build option, this macro <b>must</b> be defined in all modules
/// of the project.
///
/// @see MATHKATA_COMPILE_WITHOUT_SIMD_SUPPORT
#define MATHKATA_COMPILE_FORCE_PADDING
/// @}
#endif  // DOXYGEN

#ifdef MATHKATA_COMPILE_WITH_SIMD
/// @cond MATHKATA_INTERNAL
/// @addtogroup mathkata_build_config
/// @{
/// @def MATHKATA_COMPILE_WITH_PADDING
/// @brief Enable padding of data structures to be efficient with SIMD.
///
/// When defined, this option enables padding of some data structures (e.g
/// @ref vec3) to be more efficient with SIMD operations.  This option is
/// only applicable when @ref MATHKATA_COMPILE_WITHOUT_SIMD is not defined and
/// the target architecture and compiler support SIMD.
///
/// To use this build option, this macro <b>must</b> be defined in all modules
/// of the project.
/// @see MATHKATA_COMPILE_FORCE_PADDING
#define MATHKATA_COMPILE_WITH_PADDING
/// @}
#if defined(MATHKATA_COMPILE_FORCE_PADDING)
#if MATHKATA_COMPILE_FORCE_PADDING == 1
#if !defined(MATHKATA_COMPILE_WITH_PADDING)
#define MATHKATA_COMPILE_WITH_PADDING
#endif  // !defined(MATHKATA_COMPILE_WITH_PADDING)
#else
#if defined(MATHKATA_COMPILE_WITH_PADDING)
#undef MATHKATA_COMPILE_WITH_PADDING
#endif  // MATHKATA_COMPILE_WITH_PADDING
#endif  // MATHKATA_COMPILE_FORCE_PADDING == 1
#endif  // MATHKATA_COMPILE_FORCE_PADDING
/// @endcond
#endif  // MATHKATA_COMPILE_WITH_SIMD

/// @cond MATHKATA_INTERNAL
// Generate string which contains build options for the library.
#if defined(MATHKATA_COMPILE_WITH_SIMD)
#define MATHKATA_BUILD_OPTIONS_SIMD "[simd]"
#else
#define MATHKATA_BUILD_OPTIONS_SIMD "[no simd]"
#endif  // defined(MATHKATA_COMPILE_WITH_SIMD)
#if defined(MATHKATA_COMPILE_WITH_PADDING)
#define MATHKATA_BUILD_OPTIONS_PADDING "[padding]"
#else
#define MATHKATA_BUILD_OPTIONS_PADDING "[no padding]"
#endif  // defined(MATHKATA_COMPILE_WITH_PADDING)
/// @endcond

/// @addtogroup mathkata_version
/// @{
/// @def MATHKATA_BUILD_OPTIONS_STRING
/// @brief String that describes the library's build configuration.
#define MATHKATA_BUILD_OPTIONS_STRING \
  (MATHKATA_BUILD_OPTIONS_SIMD " " MATHKATA_BUILD_OPTIONS_PADDING)
/// @}

/// @cond MATHKATA_INTERNAL
/// Unroll an loop up to 4 iterations, where iterator is the identifier
/// used in each operation (e.g "i"), number_of_iterations is a constant which
/// specifies the number of times to perform the operation and "operation" is
/// the statement to execute for each iteration of the loop (e.g data[i] = v).
#define MATHKATA_UNROLLED_LOOP(iterator, number_of_iterations, operation) \
  {                                                                       \
    const int iterator = 0;                                               \
    {                                                                     \
      operation;                                                          \
    }                                                                     \
    if ((number_of_iterations) > 1) {                                     \
      const int iterator = 1;                                             \
      {                                                                   \
        operation;                                                        \
      }                                                                   \
      if ((number_of_iterations) > 2) {                                   \
        const int iterator = 2;                                           \
        {                                                                 \
          operation;                                                      \
        }                                                                 \
        if ((number_of_iterations) > 3) {                                 \
          const int iterator = 3;                                         \
          {                                                               \
            operation;                                                    \
          }                                                               \
          if ((number_of_iterations) > 4) {                               \
            for (int iterator = 4; iterator < (number_of_iterations);     \
                 ++iterator) {                                            \
              operation;                                                  \
            }                                                             \
          }                                                               \
        }                                                                 \
      }                                                                   \
    }                                                                     \
  }
/// @endcond

namespace mathkata {

/// @addtogroup mathkata_version
/// @{

/// @var kVersion
/// @brief String which identifies the current version of MathKata.
static constexpr const char *kVersion = "2.0.0";

/// @}

/// @addtogroup mathkata_utilities
/// @{

/// @brief Clamp x within [lower, upper].
/// @anchor mathkata_Clamp
///
/// @note Results are undefined if lower > upper.
///
/// @param x Value to clamp.
/// @param lower Lower value of the range.
/// @param upper Upper value of the range.
/// @returns Clamped value.
template <class T>
constexpr T Clamp(const T &x, const T &lower, const T &upper) {
  return std::max<T>(lower, std::min<T>(x, upper));
}

/// @brief Linearly interpolate between range_start and range_end, based on
/// percent.
/// @anchor mathkata_Lerp
///
/// @param range_start Start of the range.
/// @param range_end End of the range.
/// @param percent Value between 0.0 and 1.0 used to interpolate between
/// range_start and range_end.  Where a value of 0.0 results in a return
/// value of range_start and 1.0 results in a return value of range_end.
/// @return Value between range_start and range_end.
///
/// @tparam T Type of the range to interpolate over.
/// @tparam T2 Type of the value used to perform interpolation
///         (e.g float or double).
template <class T, class T2>
constexpr T Lerp(const T &range_start, const T &range_end, const T2 &percent) {
  return range_start + (range_end - range_start) * percent;
}

/// @brief Linearly interpolate between range_start and range_end, based on
/// percent.
/// @anchor mathkata_Lerp2
///
/// @param range_start Start of the range.
/// @param range_end End of the range.
/// @param percent Value between 0.0 and 1.0 used to interpolate between
/// range_start and range_end.  Where a value of 0.0 results in a return
/// value of range_start and 1.0 results in a return value of range_end.
/// @return Value between range_start and range_end.
///
/// @tparam T Type of the range to interpolate over.
template <class T>
constexpr T Lerp(const T &range_start, const T &range_end, const T &percent) {
  return Lerp<T, T>(range_start, range_end, percent);
}

/// @brief Check if val is within [range_start..range_end).
/// @anchor mathkata_InRange
///
/// @param val Value to be tested.
/// @param range_start Starting point of the range (inclusive).
/// @param range_end Ending point of the range (non-inclusive).
/// @return Bool indicating success.
///
/// @tparam T Type of values to test.
template <class T>
constexpr bool InRange(T val, T range_start, T range_end) {
  return val >= range_start && val < range_end;
}

/// @brief Round a value up to the nearest power of 2 (integer overload).
///
/// For integer types, uses bit manipulation that works correctly for any
/// integer width (8, 16, 32, 64-bit, etc.).
///
/// @note If @p x is 0, the return value is 0.
/// @note If @p x is already a power of 2, it is returned unchanged.
/// @note If the result would overflow the type (i.e. @p x is greater than
///       the largest power of 2 representable by T), the behavior is
///       undefined due to unsigned overflow wrapping to 0.
///
/// @param x Value to round up. Must be non-negative for signed types.
/// @returns Value rounded up to the nearest power of 2.
template <std::integral T>
constexpr T RoundUpToPowerOf2(T x) {
  // Use unsigned arithmetic to avoid undefined behavior with signed shifts.
  typedef typename std::make_unsigned<T>::type U;
  if (x <= 1) return x;
  U u = static_cast<U>(x - 1);
  // Smear the highest set bit down to all lower bits. Each shift doubles the
  // range of bits that are set, so we need log2(bit_width) iterations.
  // We iterate over all possible shift amounts; for types smaller than the
  // shift, the compiler will optimize away the redundant operations.
  for (unsigned shift = 1; shift < sizeof(T) * CHAR_BIT; shift <<= 1) {
    u |= u >> shift;
  }
  return static_cast<T>(u + 1);
}

/// @brief Round a value up to the nearest power of 2 (floating-point
/// overload).
///
/// For floating-point types, uses logarithmic computation.
///
/// @note If @p x is 0, the return value is 0.
/// @note Results may be imprecise for very large floating-point values due
///       to floating-point arithmetic limitations.
///
/// @param x Value to round up. Must be non-negative.
/// @returns Value rounded up to the nearest power of 2.
template <std::floating_point T>
T RoundUpToPowerOf2(T x) {
  if (x <= 0) return x;
  return static_cast<T>(
      pow(static_cast<T>(2), ceil(log(x) / log(static_cast<T>(2)))));
}

/// @brief Round a value up to the type's alignment boundary.
///
/// Uses the bitmask trick `(v + (align-1)) & ~(align-1)` which requires
/// alignment to be a power of 2. This is guaranteed by the C++ standard for
/// all types, but is documented here via static_assert for clarity.
///
/// @param v Value to round up.
/// @returns Value rounded up to the type's alignment boundary.
template <typename T>
constexpr size_t RoundUpToTypeBoundary(size_t v) {
  static_assert((alignof(T) & (alignof(T) - 1)) == 0,
                "alignof(T) must be a power of 2");
  return (v + alignof(T) - 1) & ~(alignof(T) - 1);
}

/// @}

/// @addtogroup mathkata_allocator
///
/// If you use MathKata with SIMD (SSE in particular), you need to have all
/// your allocations be 16-byte aligned (which isn't the case with the default
/// allocators on most platforms except OS X).
///
/// You can either use simd_allocator, which solves the problem for
/// any STL containers, but not for manual dynamic allocations or the
/// new/delete override MATHKATA_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE will
/// solve it for all allocations, at the cost of MATHKATA_ALIGNMENT bytes per
/// allocation.

/// @addtogroup mathkata_allocator
/// @{

/// @def MATHKATA_ALIGNMENT
/// @brief Alignment (in bytes) of memory allocated by AllocateAligned.
///
/// @see mathkata::AllocateAligned()
/// @see mathkata::simd_allocator
#define MATHKATA_ALIGNMENT 16

/// @brief Allocate an aligned block of memory.
/// @anchor mathkata_AllocateAligned
///
/// This function allocates a block of memory aligned to MATHKATA_ALIGNMENT
/// bytes.
///
/// @param n Size of memory to allocate.
/// @return Pointer to aligned block of allocated memory or nullptr if
/// allocation failed.
inline void *AllocateAligned(size_t n) {
#if defined(_MSC_VER)
  return _aligned_malloc(n, MATHKATA_ALIGNMENT);
#else
  return std::aligned_alloc(MATHKATA_ALIGNMENT, n);
#endif
}

/// @brief Deallocate a block of memory allocated with AllocateAligned().
/// @anchor mathkata_FreeAligned
///
/// @param p Pointer to memory to deallocate.
inline void FreeAligned(void *p) {
#if defined(_MSC_VER)
  _aligned_free(p);
#else
  std::free(p);
#endif
}

/// @brief SIMD-safe memory allocator, for use with STL types like std::vector.
///
/// For example:
/// <blockquote><code><pre>
/// std::vector<vec4, mathkata::simd_allocator<vec4>> myvector;
/// </pre></code></blockquote>
///
/// @see MATHKATA_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE
/// @tparam T type allocated by this object.
template <typename T>
class simd_allocator : public std::allocator<T> {
 public:
  /// Size type.
  typedef size_t size_type;
  /// Pointer of type T.
  typedef T *pointer;
  /// Const pointer of type T.
  typedef const T *const_pointer;

  /// Constructs a simd_allocator.
  simd_allocator() throw() : std::allocator<T>() {}
  /// @brief Constructs and copies a simd_allocator.
  ///
  /// @param a Allocator to copy.
  simd_allocator(const simd_allocator &a) throw() : std::allocator<T>(a) {}
  /// @brief Constructs and copies a simd_allocator.
  ///
  /// @param a Allocator to copy.
  /// @tparam U type of the object allocated by the allocator to copy.
  template <class U>
  simd_allocator(const simd_allocator<U> &a) throw() : std::allocator<T>(a) {}
  /// @brief Destructs a simd_allocator.
  ~simd_allocator() throw() {}

  /// @brief Obtains an allocator of a different type.
  ///
  /// @tparam  _Tp1 type of the new allocator.
  template <typename _Tp1>
  struct rebind {
    /// @brief Allocator of type _Tp1.
    typedef simd_allocator<_Tp1> other;
  };

  /// @brief Allocate memory for object T.
  ///
  /// @param n Number of types to allocate.
  /// @return Pointer to the newly allocated memory.
  pointer allocate(size_type n) {
    return reinterpret_cast<pointer>(AllocateAligned(n * sizeof(T)));
  }

  /// Deallocate memory referenced by pointer p.
  ///
  /// @param p Pointer to memory to deallocate.
  void deallocate(pointer p, size_type) { FreeAligned(p); }
};

/// @def MATHKATA_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE
/// @brief Macro which overrides the default new and delete allocators.
///
/// To globally override new and delete, simply add the line:
/// <blockquote><code><pre>
/// MATHKATA_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE
/// </pre></code></blockquote>
/// to the end of your main .cpp file.
#define MATHKATA_DEFINE_GLOBAL_SIMD_AWARE_NEW_DELETE                           \
  void *operator new(std::size_t n) { return mathkata::AllocateAligned(n); }   \
  void *operator new[](std::size_t n) { return mathkata::AllocateAligned(n); } \
  void operator delete(void *p) noexcept { mathkata::FreeAligned(p); }         \
  void operator delete[](void *p) noexcept { mathkata::FreeAligned(p); }       \
  void *operator new(std::size_t n, const std::nothrow_t &) noexcept {         \
    return mathkata::AllocateAligned(n);                                       \
  }                                                                            \
  void *operator new[](std::size_t n, const std::nothrow_t &) noexcept {       \
    return mathkata::AllocateAligned(n);                                       \
  }                                                                            \
  void operator delete(void *p, const std::nothrow_t &) noexcept {             \
    mathkata::FreeAligned(p);                                                  \
  }                                                                            \
  void operator delete[](void *p, const std::nothrow_t &) noexcept {           \
    mathkata::FreeAligned(p);                                                  \
  }

/// @def MATHKATA_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE
/// @brief Macro which defines the new and delete for MathKata classes.
#define MATHKATA_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE                     \
  static void *operator new(std::size_t n) {                            \
    return mathkata::AllocateAligned(n);                                \
  }                                                                     \
  static void *operator new[](std::size_t n) {                          \
    return mathkata::AllocateAligned(n);                                \
  }                                                                     \
  static void *operator new(std::size_t /*n*/, void *p) { return p; }   \
  static void *operator new[](std::size_t /*n*/, void *p) { return p; } \
  static void operator delete(void *p) { mathkata::FreeAligned(p); }    \
  static void operator delete[](void *p) { mathkata::FreeAligned(p); }  \
  static void operator delete(void * /*p*/, void * /*place*/) {}        \
  static void operator delete[](void * /*p*/, void * /*place*/) {}

/// @}

}  // namespace mathkata

#endif  // MATHKATA_UTILITIES_H_

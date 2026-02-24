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
#ifndef MATHKATA_INTERNAL_SIMD_HELPERS_H_
#define MATHKATA_INTERNAL_SIMD_HELPERS_H_

/// @file mathkata/internal/simd_helpers.h
/// @brief SIMD helper functions and types using xsimd.
///
/// Provides a compatibility layer with the same function signatures as the
/// former vectorial dependency, implemented using xsimd.

#ifdef MATHKATA_COMPILE_WITH_SIMD

#include <xsimd/xsimd.hpp>

namespace mathkata {

/// @brief The xsimd architecture to use for 128-bit SIMD operations.
#if defined(__SSE2__) || defined(_M_X64) || defined(_M_AMD64) \
    || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
using simd_arch_t = xsimd::sse2;
#elif defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
using simd_arch_t = xsimd::sse2;
#elif defined(__ARM_NEON__) || defined(__ARM_NEON) || defined(__aarch64__)
using simd_arch_t = xsimd::neon;
#endif

/// @brief xsimd batch type for 4 single-precision floats.
using batch4f = xsimd::batch<float, simd_arch_t>;

/// @brief Raw SIMD register type for use in unions (__m128 or float32x4_t).
using simd4f = batch4f::register_type;

// ---- Creation ----

inline simd4f simd4f_create(float x, float y, float z, float w) {
  return batch4f(x, y, z, w);
}

inline simd4f simd4f_splat(float s) { return batch4f(s); }

inline simd4f simd4f_zero() { return batch4f(0.0f); }

// ---- Arithmetic ----

inline simd4f simd4f_add(simd4f a, simd4f b) { return batch4f(a) + batch4f(b); }

inline simd4f simd4f_sub(simd4f a, simd4f b) { return batch4f(a) - batch4f(b); }

inline simd4f simd4f_mul(simd4f a, simd4f b) { return batch4f(a) * batch4f(b); }

inline simd4f simd4f_div(simd4f a, simd4f b) { return batch4f(a) / batch4f(b); }

/// @brief Fused multiply-add: a * b + c.
inline simd4f simd4f_madd(simd4f a, simd4f b, simd4f c) {
  return xsimd::fma(batch4f(a), batch4f(b), batch4f(c));
}

// ---- Load / Store ----

inline simd4f simd4f_uload2(const float* p) {
  return batch4f(p[0], p[1], 0.0f, 0.0f);
}

inline simd4f simd4f_uload3(const float* p) {
  return batch4f(p[0], p[1], p[2], 0.0f);
}

inline simd4f simd4f_uload4(const float* p) {
  return batch4f::load_unaligned(p);
}

inline void simd4f_ustore2(simd4f v, float* p) {
  batch4f b(v);
  p[0] = b.get(0);
  p[1] = b.get(1);
}

inline void simd4f_ustore3(simd4f v, float* p) {
  batch4f b(v);
  p[0] = b.get(0);
  p[1] = b.get(1);
  p[2] = b.get(2);
}

inline void simd4f_ustore4(simd4f v, float* p) {
  batch4f(v).store_unaligned(p);
}

// ---- Element access ----

inline float simd4f_get_x(simd4f v) { return batch4f(v).get(0); }
inline float simd4f_get_y(simd4f v) { return batch4f(v).get(1); }
inline float simd4f_get_z(simd4f v) { return batch4f(v).get(2); }
inline float simd4f_get_w(simd4f v) { return batch4f(v).get(3); }

// ---- Splat element ----

inline simd4f simd4f_splat_x(simd4f v) { return batch4f(batch4f(v).get(0)); }
inline simd4f simd4f_splat_y(simd4f v) { return batch4f(batch4f(v).get(1)); }
inline simd4f simd4f_splat_z(simd4f v) { return batch4f(batch4f(v).get(2)); }
inline simd4f simd4f_splat_w(simd4f v) { return batch4f(batch4f(v).get(3)); }

// ---- Dot products ----

inline float simd4f_dot3_scalar(simd4f a, simd4f b) {
  batch4f prod = batch4f(a) * batch4f(b);
  return prod.get(0) + prod.get(1) + prod.get(2);
}

inline simd4f simd4f_dot3(simd4f a, simd4f b) {
  return simd4f_splat(simd4f_dot3_scalar(a, b));
}

inline simd4f simd4f_dot4(simd4f a, simd4f b) {
  batch4f prod = batch4f(a) * batch4f(b);
  return batch4f(prod.get(0) + prod.get(1) + prod.get(2) + prod.get(3));
}

// ---- Cross product ----

inline simd4f simd4f_cross3(simd4f a, simd4f b) {
  batch4f ba(a), bb(b);
  return batch4f(ba.get(1) * bb.get(2) - ba.get(2) * bb.get(1),
                 ba.get(2) * bb.get(0) - ba.get(0) * bb.get(2),
                 ba.get(0) * bb.get(1) - ba.get(1) * bb.get(0), 0.0f);
}

// ---- min / max ----

inline simd4f simd4f_min(simd4f a, simd4f b) {
  return xsimd::min(batch4f(a), batch4f(b));
}

inline simd4f simd4f_max(simd4f a, simd4f b) {
  return xsimd::max(batch4f(a), batch4f(b));
}

// ---- Math ----

inline simd4f simd4f_sqrt(simd4f v) { return xsimd::sqrt(batch4f(v)); }

inline simd4f simd4f_rsqrt(simd4f v) { return xsimd::rsqrt(batch4f(v)); }

inline simd4f simd4f_reciprocal(simd4f v) { return batch4f(1.0f) / batch4f(v); }

// ---- length / normalize ----

inline simd4f simd4f_length3(simd4f v) {
  return simd4f_sqrt(simd4f_dot3(v, v));
}

inline simd4f simd4f_length4(simd4f v) {
  return simd4f_sqrt(simd4f_dot4(v, v));
}

inline simd4f simd4f_normalize3(simd4f v) {
  return simd4f_div(v, simd4f_length3(v));
}

inline simd4f simd4f_normalize4(simd4f v) {
  return simd4f_div(v, simd4f_length4(v));
}

// ---- Utility ----

/// @brief Zero out the W component, preserving X, Y, Z.
inline simd4f simd4f_zero_w(simd4f v) {
  batch4f b(v);
  return batch4f(b.get(0), b.get(1), b.get(2), 0.0f);
}

}  // namespace mathkata

#endif  // MATHKATA_COMPILE_WITH_SIMD

#endif  // MATHKATA_INTERNAL_SIMD_HELPERS_H_

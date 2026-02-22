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
#ifndef MATHFU_MATRIX_4X4_SIMD_H_
#define MATHFU_MATRIX_4X4_SIMD_H_

#include "mathfu/matrix.h"

/// @file mathfu/internal/matrix_4x4_simd.h
/// @brief SIMD-optimized 4x4 float matrix operations.
///
/// This file provides SIMD specializations for Matrix<float, 4, 4> multiply
/// operations. The Matrix class stores columns as Vector<float, 4>, which
/// already uses simd4f when SIMD is enabled, so no full class specialization
/// is needed -- only the performance-critical free functions are replaced.

#ifdef MATHFU_COMPILE_WITH_SIMD

#include "mathfu/internal/simd_helpers.h"

namespace mathfu {

/// @cond MATHFU_INTERNAL
/// @brief SIMD-optimized 4x4 float matrix multiplication.
///
/// Each output column is computed as:
///   out_col = m1.col0 * splat(m2_col.x)
///           + m1.col1 * splat(m2_col.y)
///           + m1.col2 * splat(m2_col.z)
///           + m1.col3 * splat(m2_col.w)
template <>
inline void TimesHelper(const Matrix<float, 4, 4>& m1,
                        const Matrix<float, 4, 4>& m2,
                        Matrix<float, 4, 4>* out_m) {
  const simd4f c0 = m1.GetColumn(0).simd4;
  const simd4f c1 = m1.GetColumn(1).simd4;
  const simd4f c2 = m1.GetColumn(2).simd4;
  const simd4f c3 = m1.GetColumn(3).simd4;

  for (int i = 0; i < 4; ++i) {
    const simd4f m2_col = m2.GetColumn(i).simd4;
    out_m->GetColumn(i).simd4 = simd4f_madd(
        c0, simd4f_splat_x(m2_col),
        simd4f_madd(c1, simd4f_splat_y(m2_col),
                    simd4f_madd(c2, simd4f_splat_z(m2_col),
                                simd4f_mul(c3, simd4f_splat_w(m2_col)))));
  }
}
/// @endcond

/// @cond MATHFU_INTERNAL
/// @brief SIMD-optimized 4x4 float matrix * 4-element vector multiply.
///
/// result = col0 * splat(v.x) + col1 * splat(v.y)
///        + col2 * splat(v.z) + col3 * splat(v.w)
template <>
inline Vector<float, 4> operator*(const Matrix<float, 4, 4>& m,
                                  const Vector<float, 4>& v) {
  const simd4f c0 = m.GetColumn(0).simd4;
  const simd4f c1 = m.GetColumn(1).simd4;
  const simd4f c2 = m.GetColumn(2).simd4;
  const simd4f c3 = m.GetColumn(3).simd4;
  const simd4f sv = v.simd4;

  return Vector<float, 4>(simd4f_madd(
      c0, simd4f_splat_x(sv),
      simd4f_madd(c1, simd4f_splat_y(sv),
                  simd4f_madd(c2, simd4f_splat_z(sv),
                              simd4f_mul(c3, simd4f_splat_w(sv))))));
}
/// @endcond

}  // namespace mathfu

#endif  // MATHFU_COMPILE_WITH_SIMD

#endif  // MATHFU_MATRIX_4X4_SIMD_H_

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
#ifndef MATHFU_VECTOR_2_SIMD_H_
#define MATHFU_VECTOR_2_SIMD_H_

#include "mathfu/internal/vector_2.h"
#include "mathfu/utilities.h"

/// @file mathfu/internal/vector_2_simd.h MathFu Vector<T, 2> Specialization
/// @brief 2-dimensional specialization of mathfu::Vector for SIMD optimized
/// builds.
/// @see mathfu::Vector
///
/// No SIMD specialization is provided for 2D vectors. The generic template
/// from vector_2.h is used for all types including float. The former NEON-only
/// specialization was removed when vectorial was replaced by xsimd.

#endif  // MATHFU_VECTOR_2_SIMD_H_

/*
 * Copyright 2017 Google Inc. All rights reserved.
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
#ifndef MATHKATA_HLSL_MAPPINGS_H_
#define MATHKATA_HLSL_MAPPINGS_H_

#include "mathkata/matrix.h"
#include "mathkata/quaternion.h"
#include "mathkata/vector.h"

/// @file mathkata/hlsl_mappings.h
/// @brief HLSL compatible data types.
/// @addtogroup mathkata_hlsl
///
/// To simplify the use of MathKata template classes and make it possible to
/// write code that looks similar to
/// <a
/// href="https://msdn.microsoft.com/en-us/library/windows/desktop/bb509587(v=vs.85).aspx">HLSL</a>
/// data types in C++, MathKata provides a set of data types that are similar in
/// style to HLSL Vector and Matrix data types.

/// @brief Namespace for MathKata library.
namespace mathkata {

/// @addtogroup mathkata_hlsl
/// @{

/// Scalar unsigned integer
typedef unsigned int uint;
typedef unsigned int dword;

/// 2-dimensional <code>float</code> Vector.
typedef Vector<float, 2> float2;
/// 3-dimensional <code>float</code> Vector.
typedef Vector<float, 3> float3;
/// 4-dimensional <code>float</code> Vector.
typedef Vector<float, 4> float4;

/// 2-dimensional <code>int</code> Vector.
typedef Vector<int, 2> int2;
/// 3-dimensional <code>int</code> Vector.
typedef Vector<int, 3> int3;
/// 4-dimensional <code>int</code> Vector.
typedef Vector<int, 4> int4;

/// 2-dimensional <code>uint</code> Vector.
typedef Vector<uint, 2> uint2;
/// 3-dimensional <code>uint</code> Vector.
typedef Vector<uint, 3> uint3;
/// 4-dimensional <code>uint</code> Vector.
typedef Vector<uint, 4> uint4;

/// 1x1 <code>float</code> Matrix.
typedef Matrix<float, 1, 1> float1x1;
/// 2x2 <code>float</code> Matrix.
typedef Matrix<float, 2, 2> float2x2;
/// 3x3 <code>float</code> Matrix.
typedef Matrix<float, 3, 3> float3x3;
/// 4x4 <code>float</code> Matrix.
typedef Matrix<float, 4, 4> float4x4;

/// 1x1 <code>double</code> Matrix.
typedef Matrix<double, 1, 1> double1x1;
/// 2x2 <code>double</code> Matrix.
typedef Matrix<double, 2, 2> double2x2;
/// 3x3 <code>double</code> Matrix.
typedef Matrix<double, 3, 3> double3x3;
/// 4x4 <code>double</code> Matrix.
typedef Matrix<double, 4, 4> double4x4;

/// 1x1 <code>int</code> Matrix.
typedef Matrix<int, 1, 1> int1x1;
/// 2x2 <code>int</code> Matrix.
typedef Matrix<int, 2, 2> int2x2;
/// 3x3 <code>int</code> Matrix.
typedef Matrix<int, 3, 3> int3x3;
/// 4x4 <code>int</code> Matrix.
typedef Matrix<int, 4, 4> int4x4;

/// 1x1 <code>uint</code> Matrix.
typedef Matrix<uint, 1, 1> uint1x1;
/// 2x2 <code>uint</code> Matrix.
typedef Matrix<uint, 2, 2> uint2x2;
/// 3x3 <code>uint</code> Matrix.
typedef Matrix<uint, 3, 3> uint3x3;
/// 4x4 <code>uint</code> Matrix.
typedef Matrix<uint, 4, 4> uint4x4;

/// @}

}  // namespace mathkata

#endif  // MATHKATA_HLSL_MAPPINGS_H_

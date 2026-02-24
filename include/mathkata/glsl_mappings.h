/*
 * Copyright 2014 Google Inc. All rights reserved.
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
#ifndef MATHKATA_GLSL_MAPPINGS_H_
#define MATHKATA_GLSL_MAPPINGS_H_

#include "mathkata/affine_transform_2d.h"
#include "mathkata/color.h"
#include "mathkata/matrix.h"
#include "mathkata/quaternion.h"
#include "mathkata/rect.h"
#include "mathkata/vector.h"

/// @file mathkata/glsl_mappings.h
/// @brief GLSL compatible data types.
/// @addtogroup mathkata_glsl
///
/// To simplify the use of MathKata template classes and make it possible to
/// write code that looks similar to
/// <a href="http://www.opengl.org/documentation/glsl/">GLSL</a> in C++,
/// MathKata provides a set of data types that are similar in style to
/// GLSL Vector and Matrix data types.

/// @brief Namespace for MathKata library.
namespace mathkata {

/// @addtogroup mathkata_glsl
/// @{

/// 2-dimensional <code>float</code> Vector.
typedef Vector<float, 2> vec2;
/// 3-dimensional <code>float</code> Vector.
typedef Vector<float, 3> vec3;
/// 4-dimensional <code>float</code> Vector.
typedef Vector<float, 4> vec4;

/// 2-dimensional <code>int</code> Vector.
typedef Vector<int, 2> vec2i;
/// 3-dimensional <code>int</code> Vector.
typedef Vector<int, 3> vec3i;
/// 4-dimensional <code>int</code> Vector.
typedef Vector<int, 4> vec4i;

/// 2x2 <code>float</code> Matrix.
typedef Matrix<float, 2, 2> mat2;
/// 3x3 <code>float</code> Matrix.
typedef Matrix<float, 3, 3> mat3;
/// 4x4 <code>float</code> Matrix.
typedef Matrix<float, 4, 4> mat4;

/// 2-dimensional <code>float</code> packed Vector (VectorPacked).
typedef VectorPacked<float, 2> vec2_packed;
/// 3-dimensional <code>float</code> packed Vector (VectorPacked).
typedef VectorPacked<float, 3> vec3_packed;
/// 4-dimensional <code>float</code> packed Vector (VectorPacked).
typedef VectorPacked<float, 4> vec4_packed;

/// 2-dimensional <code>int</code> packed Vector (VectorPacked).
typedef VectorPacked<int, 2> vec2i_packed;
/// 3-dimensional <code>int</code> packed Vector (VectorPacked).
typedef VectorPacked<int, 3> vec3i_packed;
/// 4-dimensional <code>int</code> packed Vector (VectorPacked).
typedef VectorPacked<int, 4> vec4i_packed;

/// Float-based quaternion.  Note that this is not technically
/// a GLES type, but is included for convenience.
typedef mathkata::Quaternion<float> quat;

/// Rect composed of type <code>float</code>.
typedef Rect<float> rectf;
/// Rect composed of type <code>double</code>.
typedef Rect<double> rectd;
/// Rect composed of type <code>int</code>.
typedef Rect<int> recti;

/// 2D affine transform composed of type <code>float</code>.
typedef AffineTransform2D<float> affine2d;

/// RGBA color with 8-bit components.
typedef Color color;

/// @brief reflect incident vector off a surface with the given normal.
///
/// @param incident The incoming direction vector.
/// @param normal The surface normal (must be normalized).
/// @return The reflected direction.
/// Matches GLSL reflect() semantics.
template <class T, int d>
inline Vector<T, d> reflect(const Vector<T, d>& incident,
                            const Vector<T, d>& normal) {
  return Vector<T, d>::reflect(incident, normal);
}

/// @brief Compute the refracted direction using Snell's law.
///
/// @param incident The incoming direction vector (must be normalized).
/// @param normal The surface normal (must be normalized).
/// @param eta The ratio of indices of refraction (n1/n2).
/// @return The refracted direction, or zero vector for total internal
///         reflection.
/// Matches GLSL refract() semantics.
template <class T, int d>
inline Vector<T, d> refract(const Vector<T, d>& incident,
                            const Vector<T, d>& normal, T eta) {
  return Vector<T, d>::refract(incident, normal, eta);
}

/// @}

}  // namespace mathkata

#endif  // MATHKATA_GLSL_MAPPINGS_H_

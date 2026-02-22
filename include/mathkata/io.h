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
#ifndef MATHKATA_IO_H_
#define MATHKATA_IO_H_

#include <ostream>

#include "mathkata/color.h"
#include "mathkata/matrix.h"
#include "mathkata/quaternion.h"
#include "mathkata/vector.h"

namespace mathkata {

/// @brief Print the vector contents to the output stream.
template <typename T, int d>
inline std::ostream& operator<<(std::ostream& os, const Vector<T, d>& v) {
  os << "(" << v[0];
  for (int i = 1; i < d; ++i) {
    os << ", " << v[i];
  }
  return os << ")";
}

/// @brief Print the matrix contents to the output stream.
///
/// Each column is printed as a vector, and all columns are grouped together.
/// For example, a 3x3 identity matrix is printed as:
/// ((1, 0, 0), (0, 1, 0), (0, 0, 1))
template <typename T, int rows, int columns>
inline std::ostream& operator<<(std::ostream& os,
                                const Matrix<T, rows, columns>& m) {
  os << "(" << m.GetColumn(0);
  for (int i = 1; i < columns; ++i) {
    os << ", " << m.GetColumn(i);
  }
  return os << ")";
}

/// @brief Print the quaternion contents to the output stream.
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Quaternion<T>& q) {
  return os << "(" << q[0] << ", " << q[1] << ", " << q[2] << ", " << q[3]
            << ")";
}

/// @brief Print the color contents to the output stream.
inline std::ostream& operator<<(std::ostream& os, const Color& c) {
  return os << "(" << static_cast<int>(c.r) << ", " << static_cast<int>(c.g)
            << ", " << static_cast<int>(c.b) << ", " << static_cast<int>(c.a)
            << ")";
}

}  // namespace mathkata

#endif  // MATHKATA_IO_H_

/*
 * Copyright 2024 Google Inc. All rights reserved.
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
#ifndef MATHFU_FRUSTUM_H_
#define MATHFU_FRUSTUM_H_

#include <cmath>

#include "mathfu/aabb.h"
#include "mathfu/matrix.h"
#include "mathfu/plane.h"
#include "mathfu/sphere.h"

namespace mathfu {

/// @addtogroup mathfu_frustum
/// @{
/// @class Frustum "mathfu/frustum.h"
/// @brief View frustum of type T represented as 6 planes.
///
/// Frustum stores 6 planes (near, far, left, right, top, bottom) that define
/// a view frustum volume. Provides extraction from a view-projection matrix
/// using the Gribb/Hartmann method, and culling tests against points, AABBs,
/// and spheres.
///
/// @tparam T type of Frustum elements.
template <class T>
struct Frustum {
  /// @brief Indices for the six frustum planes.
  enum FrustumPlane {
    kNear = 0,
    kFar,
    kLeft,
    kRight,
    kTop,
    kBottom,
    kPlaneCount
  };

  /// @brief The six planes that define the frustum.
  Plane<T> planes[kPlaneCount];

  /// @brief Create an uninitialized Frustum.
  ///
  /// The planes of the Frustum are left uninitialized and have indeterminate
  /// values. This is intentional for performance: use one of the factory
  /// methods or the explicit constructor if you need specific values.
  Frustum() {}

  /// @brief Create a Frustum from six explicit Plane objects.
  ///
  /// @param near_plane The near clipping plane.
  /// @param far_plane The far clipping plane.
  /// @param left_plane The left clipping plane.
  /// @param right_plane The right clipping plane.
  /// @param top_plane The top clipping plane.
  /// @param bottom_plane The bottom clipping plane.
  inline Frustum(const Plane<T>& near_plane, const Plane<T>& far_plane,
                 const Plane<T>& left_plane, const Plane<T>& right_plane,
                 const Plane<T>& top_plane, const Plane<T>& bottom_plane) {
    planes[kNear] = near_plane;
    planes[kFar] = far_plane;
    planes[kLeft] = left_plane;
    planes[kRight] = right_plane;
    planes[kTop] = top_plane;
    planes[kBottom] = bottom_plane;
  }

  /// @brief Extract frustum planes from a combined view-projection matrix.
  ///
  /// Uses the Gribb/Hartmann method to extract the six frustum planes from
  /// a combined view-projection matrix. Each plane is normalized so that the
  /// normal vector has unit length.
  ///
  /// @param vp The combined view-projection matrix.
  /// @return A Frustum with normalized planes extracted from the matrix.
  static inline Frustum<T> FromViewProjection(const Matrix<T, 4, 4>& vp) {
    Frustum<T> frustum;

    // Extract rows of the matrix. In mathfu, operator()(row, col) accesses
    // the element at the given row and column.
    // Row i = (vp(i,0), vp(i,1), vp(i,2), vp(i,3))

    // Left:   row3 + row0
    frustum.planes[kLeft] =
        NormalizePlane(vp(3, 0) + vp(0, 0), vp(3, 1) + vp(0, 1),
                       vp(3, 2) + vp(0, 2), vp(3, 3) + vp(0, 3));

    // Right:  row3 - row0
    frustum.planes[kRight] =
        NormalizePlane(vp(3, 0) - vp(0, 0), vp(3, 1) - vp(0, 1),
                       vp(3, 2) - vp(0, 2), vp(3, 3) - vp(0, 3));

    // Bottom: row3 + row1
    frustum.planes[kBottom] =
        NormalizePlane(vp(3, 0) + vp(1, 0), vp(3, 1) + vp(1, 1),
                       vp(3, 2) + vp(1, 2), vp(3, 3) + vp(1, 3));

    // Top:    row3 - row1
    frustum.planes[kTop] =
        NormalizePlane(vp(3, 0) - vp(1, 0), vp(3, 1) - vp(1, 1),
                       vp(3, 2) - vp(1, 2), vp(3, 3) - vp(1, 3));

    // Near:   row3 + row2
    frustum.planes[kNear] =
        NormalizePlane(vp(3, 0) + vp(2, 0), vp(3, 1) + vp(2, 1),
                       vp(3, 2) + vp(2, 2), vp(3, 3) + vp(2, 3));

    // Far:    row3 - row2
    frustum.planes[kFar] =
        NormalizePlane(vp(3, 0) - vp(2, 0), vp(3, 1) - vp(2, 1),
                       vp(3, 2) - vp(2, 2), vp(3, 3) - vp(2, 3));

    return frustum;
  }

  /// @brief Test whether a point is inside the frustum.
  ///
  /// A point is considered inside if it is on the positive side of all six
  /// planes (signed distance >= 0 for all planes).
  ///
  /// @param point The point to test.
  /// @return true if the point is inside or on the boundary of the frustum.
  inline bool ContainsPoint(const Vector<T, 3>& point) const {
    for (int i = 0; i < kPlaneCount; ++i) {
      if (planes[i].SignedDistance(point) < static_cast<T>(0)) {
        return false;
      }
    }
    return true;
  }

  /// @brief Test whether an AABB intersects the frustum.
  ///
  /// For each frustum plane, the AABB vertex most in the direction of the
  /// plane normal (the "positive vertex" or "p-vertex") is tested. If the
  /// positive vertex is behind the plane, the AABB is fully outside the
  /// frustum.
  ///
  /// @param aabb The axis-aligned bounding box to test.
  /// @return true if the AABB intersects or is inside the frustum.
  inline bool IntersectsAABB(const AABB<T, 3>& aabb) const {
    for (int i = 0; i < kPlaneCount; ++i) {
      // Find the positive vertex: for each axis, pick the component of min
      // or max that is most in the direction of the plane normal.
      Vector<T, 3> p_vertex;
      for (int j = 0; j < 3; ++j) {
        p_vertex[j] = (planes[i].normal[j] >= static_cast<T>(0)) ? aabb.max[j]
                                                                 : aabb.min[j];
      }
      if (planes[i].SignedDistance(p_vertex) < static_cast<T>(0)) {
        return false;
      }
    }
    return true;
  }

  /// @brief Test whether a sphere intersects the frustum.
  ///
  /// For each frustum plane, if the signed distance from the sphere center
  /// to the plane is less than the negative radius, the sphere is fully
  /// outside that plane and therefore outside the frustum.
  ///
  /// @param sphere The sphere to test.
  /// @return true if the sphere intersects or is inside the frustum.
  inline bool IntersectsSphere(const Sphere<T, 3>& sphere) const {
    for (int i = 0; i < kPlaneCount; ++i) {
      if (planes[i].SignedDistance(sphere.center) < -sphere.radius) {
        return false;
      }
    }
    return true;
  }

  /// @brief Get a specific frustum plane.
  ///
  /// @param p The plane index to retrieve.
  /// @return Const reference to the requested plane.
  inline const Plane<T>& GetPlane(FrustumPlane p) const { return planes[p]; }

 private:
  /// @brief Create a normalized Plane from raw (a, b, c, d) coefficients.
  ///
  /// @param a X component of the plane normal.
  /// @param b Y component of the plane normal.
  /// @param c Z component of the plane normal.
  /// @param d Distance coefficient of the plane.
  /// @return A normalized Plane.
  static inline Plane<T> NormalizePlane(T a, T b, T c, T d) {
    const T length = std::sqrt(a * a + b * b + c * c);
    return Plane<T>(Vector<T, 3>(a / length, b / length, c / length),
                    d / length);
  }
};
/// @}

/// @brief Check if two frustums are identical.
///
/// @param f1 Frustum to be tested.
/// @param f2 Other frustum to be tested.
template <class T>
bool operator==(const Frustum<T>& f1, const Frustum<T>& f2) {
  for (int i = 0; i < Frustum<T>::kPlaneCount; ++i) {
    if (f1.planes[i] != f2.planes[i]) {
      return false;
    }
  }
  return true;
}

/// @brief Check if two frustums are <b>not</b> identical.
///
/// @param f1 Frustum to be tested.
/// @param f2 Other frustum to be tested.
template <class T>
bool operator!=(const Frustum<T>& f1, const Frustum<T>& f2) {
  return !(f1 == f2);
}

}  // namespace mathfu

#endif  // MATHFU_FRUSTUM_H_

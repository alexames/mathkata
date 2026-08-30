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
#ifndef MATHKATA_INTERSECTIONS_H_
#define MATHKATA_INTERSECTIONS_H_

#include <algorithm>
#include <cmath>
#include <limits>

#include "mathkata/aabb.h"
#include "mathkata/plane.h"
#include "mathkata/ray.h"
#include "mathkata/sphere.h"
#include "mathkata/vector.h"

/// @file mathkata/intersections.h Intersections
/// @brief intersection test free functions for geometric primitives.
/// @addtogroup mathkata_intersections

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4723)  // suppress "potential divide by 0" warning
#endif

namespace mathkata {

/// @addtogroup mathkata_intersections
/// @{

/// @brief Test whether a ray intersects a sphere.
///
/// Returns true if the ray hits the sphere, and optionally sets @p t_out to
/// the nearest non-negative intersection parameter (the point on the ray
/// closest to the origin that lies on or inside the sphere).
///
/// @param ray The ray to test.
/// @param sphere The sphere to test against.
/// @param t_out Optional pointer to receive the intersection parameter.
/// @return true if the ray intersects the sphere.
template <class T, int N>
inline bool rayIntersectsSphere(const Ray<T, N>& ray,
                                const Sphere<T, N>& sphere,
                                T* t_out = nullptr) {
  const Vector<T, N> oc = ray.origin - sphere.center;
  const T a = Vector<T, N>::dotProduct(ray.direction, ray.direction);
  const T b = static_cast<T>(2) * Vector<T, N>::dotProduct(oc, ray.direction);
  const T c = Vector<T, N>::dotProduct(oc, oc) - sphere.radius * sphere.radius;
  const T discriminant = b * b - static_cast<T>(4) * a * c;

  if (discriminant < static_cast<T>(0)) {
    return false;
  }

  const T sqrt_disc = std::sqrt(discriminant);
  const T inv_2a = static_cast<T>(1) / (static_cast<T>(2) * a);
  const T t0 = (-b - sqrt_disc) * inv_2a;
  const T t1 = (-b + sqrt_disc) * inv_2a;

  // Find the nearest non-negative t.
  T t;
  if (t0 >= static_cast<T>(0)) {
    t = t0;
  } else if (t1 >= static_cast<T>(0)) {
    t = t1;
  } else {
    return false;
  }

  if (t_out) {
    *t_out = t;
  }
  return true;
}

/// @brief Test whether a ray intersects an axis-aligned bounding box.
///
/// Uses the slab method. Returns true if the ray hits the AABB, and optionally
/// sets @p t_out to the nearest non-negative intersection parameter.
///
/// @param ray The ray to test.
/// @param aabb The AABB to test against.
/// @param t_out Optional pointer to receive the intersection parameter.
/// @return true if the ray intersects the AABB.
template <class T, int N>
inline bool rayIntersectsAABB(const Ray<T, N>& ray, const AABB<T, N>& aabb,
                              T* t_out = nullptr) {
  T t_min = static_cast<T>(0);
  T t_max = std::numeric_limits<T>::max();

  for (int i = 0; i < N; ++i) {
    if (std::abs(ray.direction[i]) < std::numeric_limits<T>::epsilon()) {
      // Ray is parallel to slab. No hit if origin not within slab.
      if (ray.origin[i] < aabb.min[i] || ray.origin[i] > aabb.max[i]) {
        return false;
      }
    } else {
      const T inv_d = static_cast<T>(1) / ray.direction[i];
      T t0 = (aabb.min[i] - ray.origin[i]) * inv_d;
      T t1 = (aabb.max[i] - ray.origin[i]) * inv_d;
      if (t0 > t1) {
        std::swap(t0, t1);
      }
      t_min = std::max(t_min, t0);
      t_max = std::min(t_max, t1);
      if (t_min > t_max) {
        return false;
      }
    }
  }

  if (t_out) {
    *t_out = t_min;
  }
  return true;
}

/// @brief Test whether a ray intersects a plane.
///
/// Returns false if the ray is parallel to the plane (dot product of direction
/// and normal is near zero). Returns true if the ray intersects the plane at
/// a non-negative parameter, and optionally sets @p t_out to that parameter.
///
/// @note Plane is 3D only, so this function uses template <class T>.
///
/// @param ray The ray to test.
/// @param plane The plane to test against.
/// @param t_out Optional pointer to receive the intersection parameter.
/// @return true if the ray intersects the plane.
template <class T>
inline bool rayIntersectsPlane(const Ray<T, 3>& ray, const Plane<T>& plane,
                               T* t_out = nullptr) {
  const T denom = Vector<T, 3>::dotProduct(plane.normal, ray.direction);

  if (std::abs(denom) < std::numeric_limits<T>::epsilon()) {
    return false;  // Ray is parallel to the plane.
  }

  const T numer =
      -(Vector<T, 3>::dotProduct(plane.normal, ray.origin) + plane.distance);
  // denom is guaranteed non-zero by the epsilon check above.
  const T t = numer / denom;

  if (t < static_cast<T>(0)) {
    return false;  // intersection is behind the ray origin.
  }

  if (t_out) {
    *t_out = t;
  }
  return true;
}

/// @brief Test whether two AABBs overlap.
///
/// Free function wrapper around AABB::intersects().
///
/// @param a First AABB.
/// @param b Second AABB.
/// @return true if the AABBs overlap (including touching boundaries).
template <class T, int N>
constexpr bool aabbIntersectsAABB(const AABB<T, N>& a, const AABB<T, N>& b) {
  return a.intersects(b);
}

/// @brief Test whether two spheres overlap.
///
/// Free function wrapper around Sphere::intersects().
///
/// @param a First sphere.
/// @param b Second sphere.
/// @return true if the spheres overlap.
template <class T, int N>
constexpr bool sphereIntersectsSphere(const Sphere<T, N>& a,
                                      const Sphere<T, N>& b) {
  return a.intersects(b);
}

/// @brief Test whether a sphere and an AABB overlap.
///
/// Finds the closest point on the AABB to the sphere center, then checks
/// whether the distance from that point to the center is within the radius.
///
/// @param sphere The sphere to test.
/// @param aabb The AABB to test against.
/// @return true if the sphere and AABB overlap.
template <class T, int N>
constexpr bool sphereIntersectsAABB(const Sphere<T, N>& sphere,
                                    const AABB<T, N>& aabb) {
  // Find the closest point on the AABB to the sphere center.
  auto closest = Vector<T, N>::uninitialized();
  for (int i = 0; i < N; ++i) {
    closest[i] = clamp(sphere.center[i], aabb.min[i], aabb.max[i]);
  }
  return Vector<T, N>::distanceSquared(sphere.center, closest)
         <= sphere.radius * sphere.radius;
}

/// @brief Test whether a point is inside an AABB.
///
/// Free function wrapper around AABB::contains().
///
/// @param point The point to test.
/// @param aabb The AABB to test against.
/// @return true if the point is inside or on the boundary of the AABB.
template <class T, int N>
constexpr bool pointInAABB(const Vector<T, N>& point, const AABB<T, N>& aabb) {
  return aabb.contains(point);
}

/// @brief Test whether a point is inside a sphere.
///
/// Free function wrapper around Sphere::contains().
///
/// @param point The point to test.
/// @param sphere The sphere to test against.
/// @return true if the point is inside or on the boundary of the sphere.
template <class T, int N>
constexpr bool pointInSphere(const Vector<T, N>& point,
                             const Sphere<T, N>& sphere) {
  return sphere.contains(point);
}

/// @brief Test whether a point lies on a plane within a given tolerance.
///
/// Returns true if the absolute signed distance from the point to the plane
/// is less than @p epsilon.
///
/// @note Plane is 3D only, so this function uses template <class T>.
///
/// @param point The point to test.
/// @param plane The plane to test against.
/// @param epsilon The tolerance for the distance check.
/// @return true if the point is within epsilon of the plane.
template <class T>
inline bool pointOnPlane(const Vector<T, 3>& point, const Plane<T>& plane,
                         T epsilon) {
  return std::abs(plane.signedDistance(point)) < epsilon;
}

/// @}

}  // namespace mathkata

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif  // MATHKATA_INTERSECTIONS_H_

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
#ifndef MATHKATA_COLOR_H_
#define MATHKATA_COLOR_H_

#include <algorithm>
#include <cstdint>

#include "mathkata/vector.h"

namespace mathkata {

/// @addtogroup mathkata_color
/// @{
/// @class Color "mathkata/color.h"
/// @brief Represents an RGBA color with 8-bit components.
///
/// Color stores red, green, blue, and alpha channels as uint8_t values
/// in the range [0, 255]. Arithmetic operations are clamped to prevent
/// overflow and underflow.
struct Color {
  uint8_t r{0};
  uint8_t g{0};
  uint8_t b{0};
  uint8_t a{255};

  /// @brief Create a default color (opaque black).
  constexpr Color() = default;

  /// @brief Create a color from RGBA components.
  ///
  /// @param r Red component [0, 255].
  /// @param g Green component [0, 255].
  /// @param b Blue component [0, 255].
  /// @param a Alpha component [0, 255], defaults to 255 (fully opaque).
  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
      : r(r), g(g), b(b), a(a) {}

  /// @brief Convert this color to a normalized float vector.
  ///
  /// Each component is divided by 255 to produce a value in [0, 1].
  ///
  /// @return A Vector<float, 4> with normalized RGBA values.
  inline Vector<float, 4> toNormalized() const {
    return Vector<float, 4>(
        static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f,
        static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f);
  }

  /// @brief Create a color from a normalized float vector.
  ///
  /// Each component is multiplied by 255 and clamped to [0, 255].
  ///
  /// @param v A Vector<float, 4> with RGBA values in [0, 1].
  /// @return The corresponding Color.
  static inline Color fromNormalized(const Vector<float, 4>& v) {
    return Color(
        static_cast<uint8_t>(std::clamp(v[0], 0.0f, 1.0f) * 255.0f + 0.5f),
        static_cast<uint8_t>(std::clamp(v[1], 0.0f, 1.0f) * 255.0f + 0.5f),
        static_cast<uint8_t>(std::clamp(v[2], 0.0f, 1.0f) * 255.0f + 0.5f),
        static_cast<uint8_t>(std::clamp(v[3], 0.0f, 1.0f) * 255.0f + 0.5f));
  }

  /// @brief Add a color to this color with clamping.
  constexpr Color& operator+=(const Color& rhs) {
    r = static_cast<uint8_t>(std::min(static_cast<int>(r) + rhs.r, 255));
    g = static_cast<uint8_t>(std::min(static_cast<int>(g) + rhs.g, 255));
    b = static_cast<uint8_t>(std::min(static_cast<int>(b) + rhs.b, 255));
    a = static_cast<uint8_t>(std::min(static_cast<int>(a) + rhs.a, 255));
    return *this;
  }

  /// @brief Subtract a color from this color with clamping.
  constexpr Color& operator-=(const Color& rhs) {
    r = static_cast<uint8_t>(std::max(static_cast<int>(r) - rhs.r, 0));
    g = static_cast<uint8_t>(std::max(static_cast<int>(g) - rhs.g, 0));
    b = static_cast<uint8_t>(std::max(static_cast<int>(b) - rhs.b, 0));
    a = static_cast<uint8_t>(std::max(static_cast<int>(a) - rhs.a, 0));
    return *this;
  }

  /// @brief Modulate this color by another color.
  ///
  /// Each component is multiplied and scaled back to [0, 255] with rounding.
  constexpr Color& operator*=(const Color& rhs) {
    r = static_cast<uint8_t>((static_cast<int>(r) * rhs.r + 127) / 255);
    g = static_cast<uint8_t>((static_cast<int>(g) * rhs.g + 127) / 255);
    b = static_cast<uint8_t>((static_cast<int>(b) * rhs.b + 127) / 255);
    a = static_cast<uint8_t>((static_cast<int>(a) * rhs.a + 127) / 255);
    return *this;
  }

  /// @name Named color constants.
  /// @{
  static const Color Black;
  static const Color White;
  static const Color Red;
  static const Color Green;
  static const Color Blue;
  static const Color Yellow;
  static const Color Cyan;
  static const Color Magenta;
  static const Color Transparent;
  /// @}
};

inline constexpr Color Color::Black{0, 0, 0};
inline constexpr Color Color::White{255, 255, 255};
inline constexpr Color Color::Red{255, 0, 0};
inline constexpr Color Color::Green{0, 255, 0};
inline constexpr Color Color::Blue{0, 0, 255};
inline constexpr Color Color::Yellow{255, 255, 0};
inline constexpr Color Color::Cyan{0, 255, 255};
inline constexpr Color Color::Magenta{255, 0, 255};
inline constexpr Color Color::Transparent{0, 0, 0, 0};
/// @}

/// @brief Add two colors with clamping.
///
/// @param lhs First color.
/// @param rhs Second color.
/// @return The clamped sum.
constexpr Color operator+(const Color& lhs, const Color& rhs) {
  Color result = lhs;
  result += rhs;
  return result;
}

/// @brief Subtract a color from another with clamping.
///
/// @param lhs Color to subtract from.
/// @param rhs Color to subtract.
/// @return The clamped difference.
constexpr Color operator-(const Color& lhs, const Color& rhs) {
  Color result = lhs;
  result -= rhs;
  return result;
}

/// @brief Modulate two colors together.
///
/// Each component is multiplied and scaled back to [0, 255] with rounding.
///
/// @param lhs First color.
/// @param rhs Second color.
/// @return The modulated color.
constexpr Color operator*(const Color& lhs, const Color& rhs) {
  Color result = lhs;
  result *= rhs;
  return result;
}

/// @brief Check if two colors are identical.
///
/// @param lhs Color to be tested.
/// @param rhs Other color to be tested.
constexpr bool operator==(const Color& lhs, const Color& rhs) {
  return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

/// @brief Check if two colors are <b>not</b> identical.
///
/// @param lhs Color to be tested.
/// @param rhs Other color to be tested.
constexpr bool operator!=(const Color& lhs, const Color& rhs) {
  return !(lhs == rhs);
}

}  // namespace mathkata

#endif  // MATHKATA_COLOR_H_

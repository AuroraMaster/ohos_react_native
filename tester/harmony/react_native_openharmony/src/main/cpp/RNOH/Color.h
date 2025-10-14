/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/graphics/Color.h>
#include <iomanip>
#include <memory>
#include <sstream>

namespace rnoh {
using ColorValue = uint32_t;

class Color {
  facebook::react::ColorComponents m_rgba;

  static float clampColorComponent(float value, float maxValue) {
    return std::max(std::min(maxValue, value), 0.0f);
  }

 public:
  static Color from(ColorValue colorValue) {
    auto rgba = facebook::react::colorComponentsFromColor(colorValue);
    return Color(
        rgba.red * 255, rgba.green * 255, rgba.blue * 255, rgba.alpha * 1);
  }

  static Color from(facebook::react::SharedColor sharedColor) {
    if (facebook::react::isColorMeaningful(sharedColor)) {
      auto colorComponents =
          facebook::react::colorComponentsFromColor(sharedColor);
      return Color(
          colorComponents.red,
          colorComponents.green,
          colorComponents.blue,
          colorComponents.alpha);
    }
    return Color(0, 0, 0, 0);
  }

  Color(int red, int green, int blue, double alpha) {
    m_rgba.red = red;
    m_rgba.green = green;
    m_rgba.blue = blue;
    m_rgba.alpha = alpha;
  }

  ColorValue asColorValue() {
    return *facebook::react::colorFromComponents(
        {.red = Color::clampColorComponent(m_rgba.red, 255) / 255,
         .green = Color::clampColorComponent(m_rgba.green, 255) / 255,
         .blue = Color::clampColorComponent(m_rgba.blue, 255) / 255,
         .alpha = Color::clampColorComponent(m_rgba.alpha, 1)});
  }

  Color operator+(const Color& other) {
    return Color(
        m_rgba.red + other.m_rgba.red,
        m_rgba.green + other.m_rgba.green,
        m_rgba.blue + other.m_rgba.blue,
        m_rgba.alpha + other.m_rgba.alpha);
  }

  Color operator*(float multiplier) {
    return Color(
        m_rgba.red * multiplier,
        m_rgba.green * multiplier,
        m_rgba.blue * multiplier,
        m_rgba.alpha * multiplier);
  }

  std::string toRGBA() {
    std::stringstream ss;
    ss << "rgba(" << m_rgba.red << "," << m_rgba.green << "," << m_rgba.blue
       << "," << m_rgba.alpha << ")";
    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream& os, const Color& color) {
    os << "rgba(" << color.m_rgba.red << "," << color.m_rgba.green << ","
       << color.m_rgba.blue << "," << color.m_rgba.alpha << ")";
    return os;
  }
};
} // namespace rnoh
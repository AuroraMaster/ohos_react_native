#include "TouchTarget.h"

std::optional<facebook::react::Transform> invertTransform(
    const facebook::react::Transform& transform);

static constexpr auto infinity =
    std::numeric_limits<facebook::react::Float>::infinity();

auto rnoh::TouchTarget::computeChildPoint(
    Point const& point,
    TouchTarget::Shared const& child) const -> Point {
  auto childLayout = child->getLayoutMetrics();
  auto childTransform = child->getTransform();

  // the center of the view (relative to its origin)
  Point center{
      childLayout.frame.size.width / 2, childLayout.frame.size.height / 2};
  // the center of the view (before applying its transformation),
  // which is the origin of the transformation (relative to parent)
  Point transformationOrigin = childLayout.frame.origin + center;

  auto inverseTransform = invertTransform(childTransform);
  if (!inverseTransform.has_value()) {
    // if the transform matrix is not invertible, the scale in some direction is
    // 0, and so the point cannot be within the transformed view
    return {infinity, infinity};
  }

  // transform the vector from the origin of the transformation
  auto transformedOffsetFromCenter =
      (point + this->getCurrentOffset() - transformationOrigin) *
      inverseTransform.value();

  // add back the offset of the center relative to the origin of the view
  auto localPoint = transformedOffsetFromCenter + center;

  return localPoint;
}

/*
 * Invert 4x4 matrix.
 * Adapted from Mesa's glu library implementation
 * https://gitlab.freedesktop.org/mesa/glu/-/blob/a2b96c7bba8db8fec3e02fb4227a7f7b02cabad1/src/libutil/project.c
 *
 * SGI FREE SOFTWARE LICENSE B (Version 2.0, Sept. 18, 2008)
 * Copyright (C) 1991-2000 Silicon Graphics, Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice including the dates of first publication and
 * either this permission notice or a reference to
 * http://oss.sgi.com/projects/FreeB/
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * SILICON GRAPHICS, INC. BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of Silicon Graphics, Inc.
 * shall not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization from
 * Silicon Graphics, Inc.
 */
static bool gluInvertMatrix(
    const std::array<facebook::react::Float, 16>& m,
    std::array<facebook::react::Float, 16>& invOut) {
  invOut[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] -
      m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] -
      m[13] * m[7] * m[10];
  invOut[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] +
      m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] +
      m[12] * m[7] * m[10];
  invOut[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] +
      m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
  invOut[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] +
      m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] +
      m[12] * m[6] * m[9];
  invOut[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] +
      m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] +
      m[13] * m[3] * m[10];
  invOut[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] -
      m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] -
      m[12] * m[3] * m[10];
  invOut[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] +
      m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] +
      m[12] * m[3] * m[9];
  invOut[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] -
      m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] -
      m[12] * m[2] * m[9];
  invOut[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] +
      m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
  invOut[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] -
      m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
  invOut[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] +
      m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
  invOut[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] +
      m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] +
      m[12] * m[2] * m[5];
  invOut[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -
      m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
  invOut[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] +
      m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
  invOut[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] -
      m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
  invOut[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] +
      m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  auto det = m[0] * invOut[0] + m[1] * invOut[4] + m[2] * invOut[8] +
      m[3] * invOut[12];
  if (det == 0) {
    return false;
  }

  det = 1.0 / det;

  for (size_t i = 0; i < 16; i++) {
    invOut[i] *= det;
  }

  return true;
}

std::optional<facebook::react::Transform> invertTransform(
    const facebook::react::Transform& transform) {
  facebook::react::Transform result;
  auto succeeded = gluInvertMatrix(transform.matrix, result.matrix);
  if (!succeeded) {
    return std::nullopt;
  }
  return result;
}

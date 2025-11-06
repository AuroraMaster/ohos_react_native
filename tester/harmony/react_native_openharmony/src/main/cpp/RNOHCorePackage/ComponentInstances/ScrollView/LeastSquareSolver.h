#ifndef LEASTSQUARESOLVER_H
#define LEASTSQUARESOLVER_H
/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

// Implementation taken from react-native-gesture-handler LeastSquareSolver
// https://github.com/software-mansion/react-native-gesture-handler/blob/main/packages/react-native-gesture-handler/src/web/tools/LeastSquareSolver.ts

#include <cmath>
#include <optional>
#include <vector>

namespace rnoh {

class Vector {
 public:
  Vector(size_t length) : offset_(0), length_(length) {
    elements_.resize(length);
  }

  static Vector
  fromVOL(const std::vector<double>& values, size_t offset, size_t length) {
    Vector result(0);
    result.offset_ = offset;
    result.length_ = length;
    result.elements_ = values;
    return result;
  }

  double get(size_t index) const noexcept {
    return elements_[offset_ + index];
  }

  void set(size_t index, double value) noexcept {
    elements_[offset_ + index] = value;
  }

  double dot(const Vector& other) const noexcept {
    double result = 0;
    for (size_t i = 0; i < length_; ++i) {
      result += get(i) * other.get(i);
    }
    return result;
  }

  double norm() const noexcept {
    return std::sqrt(dot(*this));
  }

 private:
  size_t offset_;
  size_t length_;
  std::vector<double> elements_;
};

class Matrix {
 public:
  Matrix(size_t rows, size_t columns) : columns_(columns) {
    elements_.resize(rows * columns);
  }

  double get(size_t row, size_t column) const noexcept {
    return elements_[row * columns_ + column];
  }

  void set(size_t row, size_t column, double value) noexcept {
    elements_[row * columns_ + column] = value;
  }

  Vector getRow(size_t row) const {
    return Vector::fromVOL(elements_, row * columns_, columns_);
  }

 private:
  size_t columns_;
  std::vector<double> elements_;
};

// An nth degree polynomial fit to a dataset.
class PolynomialFit {
 public:
  // The polynomial coefficients of the fit.
  //
  // For each `i`, the element `coefficients[i]` is the coefficient of
  // the `i`-th power of the variable.
  std::vector<double> coefficients;

  // Creates a polynomial fit of the given degree.
  //
  // There are n + 1 coefficients in a fit of degree n.
  explicit PolynomialFit(size_t degree) {
    coefficients.resize(degree + 1);
  }
};

// Uses the least-squares algorithm to fit a polynomial to a set of data.
class LeastSquareSolver {
 public:
  // Creates a least-squares solver.
  //
  // The [x], [y], and [w] arguments must not be null.
  explicit LeastSquareSolver(
      const std::vector<double>& x,
      const std::vector<double>& y,
      const std::vector<double>& w)
      : x_(x), y_(y), w_(w) {}

  // Fits a polynomial of the given degree to the data points.
  //
  // When there is not enough data to fit a curve null is returned.
  std::optional<PolynomialFit> solve(size_t degree) {
    if (degree > x_.size()) {
      // Not enough data to fit a curve.
      return std::nullopt;
    }

    PolynomialFit result(degree);
    // Shorthands for the purpose of notation equivalence to original C++ code.
    size_t m = x_.size();
    size_t n = degree + 1;

    // Expand the X vector to a matrix A, pre-multiplied by the weights.
    Matrix a(n, m);
    for (size_t h = 0; h < m; ++h) {
      a.set(0, h, w_[h]);
      for (size_t i = 1; i < n; ++i) {
        a.set(i, h, a.get(i - 1, h) * x_[h]);
      }
    }

    // Apply the Gram-Schmidt process to A to obtain its QR decomposition.
    // Orthonormal basis, column-major order.
    Matrix q(n, m);
    // Upper triangular matrix, row-major order.
    Matrix r(n, m);

    for (size_t j = 0; j < n; ++j) {
      for (size_t h = 0; h < m; ++h) {
        q.set(j, h, a.get(j, h));
      }
      for (size_t i = 0; i < j; ++i) {
        double dotProduct = q.getRow(j).dot(q.getRow(i));
        for (size_t h = 0; h < m; ++h) {
          q.set(j, h, q.get(j, h) - dotProduct * q.get(i, h));
        }
      }

      double norm = q.getRow(j).norm();
      if (norm < PRECISION_ERROR_TOLERANCE) {
        // Vectors are linearly dependent or zero so no solution.
        return std::nullopt;
      }

      double inverseNorm = (norm == 0.0) ? 0.0 : 1.0 / norm;
      for (size_t h = 0; h < m; ++h) {
        q.set(j, h, q.get(j, h) * inverseNorm);
      }
      for (size_t i = j; i < n; ++i) {
        r.set(j, i, q.getRow(j).dot(a.getRow(i)));
      }
    }

    // Solve R B = Qt W Y to find B. This is easy because R is upper triangular.
    // We just work from bottom-right to top-left calculating B's coefficients.
    Vector wy(m);
    for (size_t h = 0; h < m; ++h) {
      wy.set(h, y_[h] * w_[h]);
    }
    for (int i = n - 1; i >= 0; --i) {
      result.coefficients[i] = q.getRow(i).dot(wy);
      for (size_t j = n - 1; j > i; --j) {
        result.coefficients[i] -= r.get(i, j) * result.coefficients[j];
      }
      result.coefficients[i] /= r.get(i, i);
    }

    return result;
  }

 private:
  // The x-coordinates of each data point.
  std::vector<double> x_;
  // The y-coordinates of each data point.
  std::vector<double> y_;
  // The weight to use for each data point.
  std::vector<double> w_;

  static constexpr double PRECISION_ERROR_TOLERANCE = 1e-10;
};

} // namespace rnoh

#endif // LEASTSQUARESOLVER_H
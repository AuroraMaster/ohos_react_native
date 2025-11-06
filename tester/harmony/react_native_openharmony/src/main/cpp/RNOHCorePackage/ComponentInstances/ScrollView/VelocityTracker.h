#ifndef VELOCITYTRACKER_H
#define VELOCITYTRACKER_H

/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

// Implementation taken from react-native-gesture-handler VelocityTracker
// https://github.com/software-mansion/react-native-gesture-handler/blob/main/packages/react-native-gesture-handler/src/web/tools/VelocityTracker.ts

#include <sys/types.h>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <optional>
#include <vector>
#include "CircularBuffer.h"
#include "LeastSquareSolver.h"

namespace rnoh {

struct AdaptedEvent {
  double x;
  double y;
  int64_t time;
};

class VelocityTracker {
 private:
  static constexpr double assumePointerMoveStoppedMilliseconds_ = 40.0;
  static constexpr size_t historySize_ = 20;
  static constexpr double horizonMilliseconds_ = 300.0;
  static constexpr size_t minSampleSize_ = 3;

  CircularBuffer<AdaptedEvent> samples_;

 public:
  VelocityTracker() : samples_(historySize_) {}

  void add(double x, double y) noexcept {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
                   .count();

    AdaptedEvent event{x, y, now};
    samples_.push(event);
  }

  std::pair<double, double> getVelocity() {
    auto estimate = getVelocityEstimate();
    if (estimate.has_value()) {
      return *estimate;
    }
    return {0.0, 0.0};
  }

  void reset() noexcept {
    samples_.clear();
  }

 private:
  // Returns an estimate of the velocity of the object being tracked by the
  // tracker given the current information available to the tracker.
  //
  // Information is added using [addPosition].
  //
  // Returns null if there is no data on which to base an estimate.
  std::optional<std::pair<double, double>> getVelocityEstimate() {
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> w;
    std::vector<double> time;

    if (samples_.size() == 0) {
      return std::nullopt;
    }

    size_t sampleCount = 0;
    ssize_t index = static_cast<ssize_t>(samples_.size()) - 1;

    const AdaptedEvent& newestSample = samples_.get(index);
    AdaptedEvent previousSample = newestSample;

    // Starting with the most recent PointAtTime sample, iterate backwards while
    // the samples represent continuous motion.
    while (sampleCount < samples_.size()) {
      const AdaptedEvent& sample = samples_.get(index);

      double age = static_cast<double>(newestSample.time - sample.time);
      double delta =
          std::abs(static_cast<double>(sample.time - previousSample.time));
      previousSample = sample;

      if (age > horizonMilliseconds_ ||
          delta > assumePointerMoveStoppedMilliseconds_) {
        break;
      }

      x.push_back(sample.x);
      y.push_back(sample.y);
      w.push_back(1.0);
      time.push_back(-age);

      sampleCount++;
      index--;
      if (index < 0) {
        break;
      }
    }

    if (sampleCount >= minSampleSize_) {
      LeastSquareSolver xSolver(time, x, w);
      auto xFit = xSolver.solve(2);

      if (xFit.has_value()) {
        LeastSquareSolver ySolver(time, y, w);
        auto yFit = ySolver.solve(2);

        if (yFit.has_value()) {
          double xVelocity = xFit->coefficients[1];
          double yVelocity = yFit->coefficients[1];

          return std::make_pair(xVelocity, yVelocity);
        }
      }
    }

    return std::nullopt;
  }
};

} // namespace rnoh

#endif // VELOCITYTRACKER_H

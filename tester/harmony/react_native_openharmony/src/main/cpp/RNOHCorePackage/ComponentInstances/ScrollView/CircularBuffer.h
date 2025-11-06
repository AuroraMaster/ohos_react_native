#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

// Implementation taken from react-native-gesture-handler CircularBuffer
// https://github.com/software-mansion/react-native-gesture-handler/blob/main/packages/react-native-gesture-handler/src/web/tools/CircularBuffer.ts

#include <vector>

namespace rnoh {
template <typename T>
class CircularBuffer {
 public:
  explicit CircularBuffer(size_t size) : capacity_(size), index_(0), size_(0) {
    buffer_.resize(size);
  }

  void push(const T& element) {
    buffer_[index_] = element;
    index_ = (index_ + 1) % capacity_;
    size_ = std::min(size_ + 1, capacity_);
  }

  const T& get(size_t at) const {
    if (size_ == capacity_) {
      return buffer_[(index_ + at) % capacity_];
    } else {
      return buffer_[at];
    }
  }

  void clear() noexcept {
    std::fill(buffer_.begin(), buffer_.end(), T{});
    index_ = 0;
    size_ = 0;
  }

  size_t size() const noexcept {
    return size_;
  }

 private:
  size_t capacity_;
  std::vector<T> buffer_;
  size_t index_;
  size_t size_;
};

} // namespace rnoh

#endif // CIRCULARBUFFER_H
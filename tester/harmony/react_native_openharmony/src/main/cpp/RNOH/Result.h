/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <optional>
#include <stdexcept>
#include <variant>
#include "RNOH/Assert.h"
#include "RNOH/RNOHError.h"

namespace rnoh {
template <typename TOK>
class Result {
  std::optional<TOK> m_ok;
  const std::exception_ptr m_error;

  template <typename T>
  friend Result<T> Ok(T value);

  template <typename T>
  friend Result<T> Err(const std::exception_ptr& ex);

  Result(std::optional<TOK> okValue, std::exception_ptr error)
      : m_ok(std::move(okValue)), m_error(std::move(error)) {}

 public:
  TOK unwrap() const {
    if (m_error != nullptr) {
      std::rethrow_exception(m_error);
    }
    RNOH_ASSERT(m_ok.has_value());
    return m_ok.value();
  }

  std::exception_ptr unwrapErr() const {
    if (m_error) {
      return m_error;
    } else {
      throw RNOHError("Called unwrapErr on an ok value");
    }
  }

  bool isOk() const {
    return m_error == nullptr;
  }
};

template <typename TOK>
Result<TOK> Ok(TOK value) {
  return Result<TOK>(std::make_optional(std::move(value)), nullptr);
}

template <typename TOK>
Result<TOK> Err(const std::exception_ptr& ex) {
  return Result<TOK>(std::nullopt, ex);
}

} // namespace rnoh
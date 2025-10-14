/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <boost/stacktrace.hpp>
#include <exception>
#include <sstream>
#include <string>

namespace rnoh {

class RNOHError : public std::exception {
 private:
  std::string m_what;
  std::string m_message;
  std::vector<std::string> m_suggestions;
  boost::stacktrace::stacktrace m_stacktrace;

 public:
  RNOHError(
      std::string whatHappened,
      std::vector<std::string> howCanItBeFixed = {})
      : m_message(std::move(whatHappened)),
        m_suggestions(std::move(howCanItBeFixed)) {
    m_stacktrace = boost::stacktrace::stacktrace();

    std::vector<const std::string> lines = {m_message};
    if (!m_suggestions.empty()) {
      lines.emplace_back("Suggestions:");
    }
    for (const std::string& suggestion : m_suggestions) {
      lines.emplace_back(suggestion);
    }
    for (auto& line : lines) {
      m_what.append(line).append("\n");
    }
  }

  const char* what() const noexcept override {
    return m_what.c_str();
  }

  std::string const& getMessage() const {
    return m_message;
  }

  std::vector<std::string> const& getSuggestions() const {
    return m_suggestions;
  }

  std::vector<std::string> getStacktrace() const {
    std::vector<std::string> result;
    // starts with 1, to ignore the frame pointing to RNOHError
    for (size_t i = 1; i < m_stacktrace.size(); ++i) {
      const auto& frame = m_stacktrace[i];
      std::ostringstream oss;
      oss << frame;
      result.push_back(oss.str());
    }
    return result;
  }
};

class FatalRNOHError : public RNOHError {
  using RNOHError::RNOHError;
};
} // namespace rnoh

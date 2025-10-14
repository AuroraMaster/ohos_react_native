/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include "hitrace/trace.h"
#include <string>
#include <sstream>

struct HiTraceSystraceSection {
 public:
  template <typename... ConvertsToStringPiece>
  explicit HiTraceSystraceSection(
      const char *name,
      ConvertsToStringPiece &&...args) {
        std::ostringstream oss;
        (oss << ... << args);
        std::string result = std::string(name) + oss.str();
        OH_HiTrace_StartTrace(result.c_str());
      }

      ~HiTraceSystraceSection() {
        OH_HiTrace_FinishTrace();
      }
};

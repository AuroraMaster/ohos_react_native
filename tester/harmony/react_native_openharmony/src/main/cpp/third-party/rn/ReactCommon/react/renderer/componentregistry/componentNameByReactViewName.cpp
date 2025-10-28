/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "componentNameByReactViewName.h"
#include <glog/logging.h>
#include <algorithm>

namespace facebook::react {

std::string componentNameByReactViewName(std::string viewName) {
  // We need this function only for the transition period;
  // eventually, all names will be unified.

  // TODO T97384889: unify component names between JS - Android - iOS - C++
  std::string rctPrefix("RCT");
  if (std::mismatch(rctPrefix.begin(), rctPrefix.end(), viewName.begin())
          .first == rctPrefix.end()) {
    // If `viewName` has "RCT" prefix, remove it.
    // RNC: patch
    if (viewName == "RCTView") {
      viewName = "View";
    } else if (viewName == "RCTRawText") {
      viewName = "RawText";
    } else if (viewName == "RCTText") {
      viewName = "Text";
    } else if (viewName == "RCTScrollContentView") {
      viewName = "ScrollContentView";
    } else if (viewName == "RCTActivityIndicatorView") {
      viewName = "ActivityIndicatorView";
    } else if (viewName == "RCTScrollView") {
      viewName = "ScrollView";
    } else if (viewName == "RCTVirtualText") {
      viewName = "VirtualText";
    } else if (viewName == "RCTImageView") {
      viewName = "ImageView";
    } else if (viewName == "RCTSinglelineTextInputView") {
      viewName = "SinglelineTextInputView";
    } else if (viewName == "RCTModalHostView") {
      viewName = "ModalHostView";
    } else if (viewName == "RCTRefreshControl") {
      viewName = "RefreshControl";
    } else if (viewName == "RCTSwitch") {
      viewName = "Switch";
    } else if (viewName == "RCTSafeAreaView") {
      viewName = "SafeAreaView";
    } else if (viewName == "RCTMultilineTextInputView") {
      viewName = "MultilineTextInputView";
    } else {
      DLOG(INFO) << "Skipped removing RCT prefix for component: " << viewName;
    }
  }

  // Fabric uses slightly new names for Text components because of differences
  // in semantic.
  if (viewName == "Text") {
    return "Paragraph";
  }

  // TODO T63839307: remove this condition after deleting TextInlineImage from
  // old renderer code
  if (viewName == "TextInlineImage") {
    return "Image";
  }
  if (viewName == "VirtualText") {
    return "Text";
  }

  if (viewName == "ImageView") {
    return "Image";
  }

  if (viewName == "AndroidHorizontalScrollView") {
    return "ScrollView";
  }

  if (viewName == "RKShimmeringView") {
    return "ShimmeringView";
  }

  if (viewName == "RefreshControl") {
    return "PullToRefreshView";
  }

  // We need this temporarily for testing purposes until we have proper
  // implementation of core components.
  // iOS-only
  if (viewName == "ScrollContentView") {
    return "View";
  }

  // iOS-only
  if (viewName == "MultilineTextInputView" ||
      viewName == "SinglelineTextInputView") {
    return "TextInput";
  }

  return viewName;
}

} // namespace facebook::react

/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "TextInputEventEmitter.h"

namespace facebook::react {

static jsi::Value textInputMetricsPayload(
    jsi::Runtime &runtime,
    TextInputMetrics const &textInputMetrics) {
  auto payload = jsi::Object(runtime);

  payload.setProperty(
      runtime,
      "text",
      jsi::String::createFromUtf8(runtime, textInputMetrics.text));

  payload.setProperty(runtime, "eventCount", textInputMetrics.eventCount);

  {
    auto selection = jsi::Object(runtime);
    selection.setProperty(
        runtime, "start", textInputMetrics.selectionRange.location);
    selection.setProperty(
        runtime,
        "end",
        textInputMetrics.selectionRange.location +
            textInputMetrics.selectionRange.length);
    payload.setProperty(runtime, "selection", selection);
  }

  return payload;
};

static jsi::Value scrollMetricsPayload(
    jsi::Runtime& runtime,
    TextInputMetrics const& textInputMetrics) {
  auto payload = jsi::Object(runtime);
  {
    auto contentSize = jsi::Object(runtime);
    contentSize.setProperty(
        runtime, "width", textInputMetrics.contentSize.width);
    contentSize.setProperty(
        runtime, "height", textInputMetrics.contentSize.height);
    payload.setProperty(runtime, "contentSize", contentSize);
  }

  {
    auto contentOffset = jsi::Object(runtime);
    contentOffset.setProperty(runtime, "x", textInputMetrics.contentOffset.x);
    contentOffset.setProperty(runtime, "y", textInputMetrics.contentOffset.y);
    payload.setProperty(runtime, "contentOffset", contentOffset);
  }

  return payload;
};

static jsi::Value keyPressMetricsPayload(
    jsi::Runtime &runtime,
    KeyPressMetrics const &keyPressMetrics) {
  auto payload = jsi::Object(runtime);
  payload.setProperty(runtime, "eventCount", keyPressMetrics.eventCount);

  std::string key;
  if (keyPressMetrics.text.empty()) {
    key = "Backspace";
  } else {
    if (keyPressMetrics.text.front() == '\n') {
      key = "Enter";
    } else if (keyPressMetrics.text.front() == '\t') {
      key = "Tab";
    } else {
      key = keyPressMetrics.text;
    }
  }
  payload.setProperty(
      runtime, "key", jsi::String::createFromUtf8(runtime, key));
  return payload;
};

static jsi::Value onChangeMetricsPayload(
    jsi::Runtime& runtime,
    OnChangeMetrics const& onChangeMetrics) {
  auto payload = jsi::Object(runtime);

  payload.setProperty(
      runtime,
      "text",
      jsi::String::createFromUtf8(runtime, onChangeMetrics.text));

  payload.setProperty(runtime, "eventCount", onChangeMetrics.eventCount);

  return payload;
};

static jsi::Value onContentSizeChangeMetricsPayload(
    jsi::Runtime& runtime,
    Size const& onContentSizeChangeMetrics) {
  auto payload = jsi::Object(runtime);

    auto contentSize = jsi::Object(runtime);
    contentSize.setProperty(
        runtime, "width", onContentSizeChangeMetrics.width);
    contentSize.setProperty(
        runtime, "height", onContentSizeChangeMetrics.height);
    payload.setProperty(runtime, "contentSize", contentSize);

  return payload;
};

void TextInputEventEmitter::onFocus(
    TextInputMetrics const &textInputMetrics) const {
  dispatchTextInputEvent("focus", textInputMetrics);
}

void TextInputEventEmitter::onBlur(
    TextInputMetrics const &textInputMetrics) const {
  dispatchTextInputEvent("blur", textInputMetrics);
}

void TextInputEventEmitter::onChange(
    OnChangeMetrics const &onChangeMetrics) const {
  dispatchEvent("change",
  [onChangeMetrics](jsi::Runtime& runtime) {
    return onChangeMetricsPayload(runtime, onChangeMetrics);
  },
  EventPriority::AsynchronousBatched);
}
void TextInputEventEmitter::onChangeSync(
    TextInputMetrics const &textInputMetrics) const {
  dispatchTextInputEvent(
      "changeSync", textInputMetrics, EventPriority::SynchronousBatched);
}

void TextInputEventEmitter::onContentSizeChange(
    Size const &onContentSizeChangeMetrics) const {
  dispatchEvent(
      "contentSizeChange",
      [onContentSizeChangeMetrics](jsi::Runtime& runtime) {
        return onContentSizeChangeMetricsPayload(runtime, onContentSizeChangeMetrics);
      },
      EventPriority::AsynchronousBatched);
}

void TextInputEventEmitter::onSelectionChange(
    TextInputMetrics const &textInputMetrics) const {
  dispatchTextInputEvent("selectionChange", textInputMetrics);
}

void TextInputEventEmitter::onEndEditing(
    TextInputMetrics const &textInputMetrics) const {
  dispatchTextInputEvent("endEditing", textInputMetrics);
}

void TextInputEventEmitter::onSubmitEditing(
    TextInputMetrics const &textInputMetrics) const {
  dispatchTextInputEvent("submitEditing", textInputMetrics);
}

void TextInputEventEmitter::onKeyPress(
    KeyPressMetrics const &keyPressMetrics) const {
  dispatchEvent(
      "keyPress",
      [keyPressMetrics](jsi::Runtime &runtime) {
        return keyPressMetricsPayload(runtime, keyPressMetrics);
      },
      EventPriority::AsynchronousBatched);
}

void TextInputEventEmitter::onKeyPressSync(
    KeyPressMetrics const &keyPressMetrics) const {
  dispatchEvent(
      "keyPressSync",
      [keyPressMetrics](jsi::Runtime &runtime) {
        return keyPressMetricsPayload(runtime, keyPressMetrics);
      },
      EventPriority::SynchronousBatched);
}

void TextInputEventEmitter::onScroll(
    TextInputMetrics const &textInputMetrics) const {
  dispatchEvent(
      "scroll",
      [textInputMetrics](jsi::Runtime& runtime) {
        return scrollMetricsPayload(runtime, textInputMetrics);
      },
      EventPriority::AsynchronousBatched);
}

void TextInputEventEmitter::dispatchTextInputEvent(
    std::string const &name,
    TextInputMetrics const &textInputMetrics,
    EventPriority priority) const {
  dispatchEvent(
      name,
      [textInputMetrics](jsi::Runtime &runtime) {
        return textInputMetricsPayload(runtime, textInputMetrics);
      },
      priority);
}

} // namespace facebook::react

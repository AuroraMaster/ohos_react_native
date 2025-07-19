/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <arkui/native_node.h>
#include "NativeNodeApi.h"

namespace rnoh {

/**
 * @actor RNOH_LIBRARY
 * A non-singleton class for managing native node operations.
 * This class encapsulates UIContext and provides methods without requiring
 * uiContext to be passed each time. It serves as a higher-level wrapper
 * around the existing NativeNodeApi singleton.
 */
class NodeApi {
 public:
  NodeApi() : m_uiContext(nullptr) {}
  /**
   * @brief Constructor that accepts ArkUI_ContextHandle
   * @param context The context containing UIContext and other relevant
   * information
   */
  explicit NodeApi(ArkUI_ContextHandle uiContext);

  /**
   * @brief Destructor
   */
  ~NodeApi() = default;

  /**
   * @brief Create a new node of specified type
   * @param nodeType The type of node to create
   * @return ArkUI_NodeHandle
   */
  ArkUI_NodeHandle createNode(ArkUI_NodeType nodeType);

  /**
   * @brief Sets the attribute of a node using the encapsulated UIContext
   * @param node Indicates the node whose attribute needs to be set
   * @param attribute Indicates the type of attribute to set
   * @param item Indicates the attribute value
   * @return Returns the error code
   */
  int32_t setAttribute(
      ArkUI_NodeHandle node,
      ArkUI_NodeAttributeType attribute,
      const ArkUI_AttributeItem* item);

  /**
   * @brief Resets an attribute of a node using the encapsulated UIContext
   * @param node Indicates the node whose attribute needs to be reset
   * @param attribute Indicates the type of attribute to reset
   * @return Returns the error code
   */
  int32_t resetAttribute(
      ArkUI_NodeHandle node,
      ArkUI_NodeAttributeType attribute);

  /**
   * @brief Sets the width and height for a component after the measurement.
   *
   * @param node Indicates the target node.
   * @param width Indicates the width.
   * @param height Indicates the height.
   * @return Returns the error code.
   */
  int32_t setMeasuredSize(ArkUI_NodeHandle node, int32_t width, int32_t height);

  /**
   * @brief Inserts a component to the specified position in a parent node.
   *
   * @param parent Indicates the pointer to the parent node.
   * @param child Indicates the pointer to the child node.
   * @param position Indicates the position to which the target child node is to
   * be inserted. If the value is a negative number or invalid, the node is
   * inserted at the end of the parent node.
   * @return Returns the error code.
   */
  int32_t insertChildAt(
      ArkUI_NodeHandle parent,
      ArkUI_NodeHandle child,
      int32_t position);

  /**
   * @brief Adds a component to a parent node.
   *
   * @param parent Indicates the pointer to the parent node.
   * @param child Indicates the pointer to the child node.
   * @return Returns the error code.
   */
  int32_t addChild(ArkUI_NodeHandle parent, ArkUI_NodeHandle child);

  /**
   * @brief the NodeApi is available if the UIContext is not null.
   *
   * @return Returns true if the NodeApi is available, otherwise false.
   */
  bool isAvailable() const {
    return m_uiContext != nullptr;
  }

 private:
  // Helper functions for handling UIContext-specific operations
  ArkUI_NodeHandle createNodeWithUIContext(ArkUI_NodeType nodeType);

  int32_t setAttributeWithUIContext(
      ArkUI_NodeHandle node,
      ArkUI_NodeAttributeType attribute,
      const ArkUI_AttributeItem* item);

  int32_t resetAttributeWithUIContext(
      ArkUI_NodeHandle node,
      ArkUI_NodeAttributeType attribute);

  int32_t setMeasuredSizeWithUIContext(
      ArkUI_NodeHandle node,
      int32_t width,
      int32_t height);

  int32_t insertChildAtWithUIContext(
      ArkUI_NodeHandle parent,
      ArkUI_NodeHandle child,
      int32_t position);

  int32_t addChildWithUIContext(
      ArkUI_NodeHandle parent,
      ArkUI_NodeHandle child);

 private:
  ArkUI_ContextHandle m_uiContext = nullptr;
};
} // namespace rnoh

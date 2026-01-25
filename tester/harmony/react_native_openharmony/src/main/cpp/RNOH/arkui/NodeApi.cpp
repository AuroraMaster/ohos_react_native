/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "NodeApi.h"

#include "DynamicArkUILoader.h"
#include "NativeNodeApi.h"
#include "RNOH/ArkTSBridge.h"
#include "RNOH/ApiVersionCheck.h"

namespace rnoh {

namespace {
// The initial version of OH_ArkUI_RunTaskInScope API
constexpr int32_t OH_SDK_API_LEVEL_20 = 20;
struct SetAttributeTaskData {
  ArkUI_NodeHandle node;
  ArkUI_NodeAttributeType attribute;
  ArkUI_AttributeItem item;
};

struct ResetAttributeTaskData {
  ArkUI_NodeHandle node;
  ArkUI_NodeAttributeType attribute;
};
struct CreateNodeData {
  ArkUI_NodeType type;
  ArkUI_NodeHandle nodeHandle;
};
struct MeasuredSizeData {
  ArkUI_NodeHandle node;
  int32_t width;
  int32_t height;
};

struct NodeChildData {
  ArkUI_NodeHandle parent;
  ArkUI_NodeHandle child;
  int32_t position;
};

// OH_ArkUI_RunTaskInScope is only supported in API Level 20 and above versions
bool IsNewAPIVersion() {
  DynamicArkUILoader::initAPI15();
  auto result = IsAtLeastApi20();
  if (result) {
    DynamicArkUILoader::init();
  }
  return result && DynamicArkUILoader::isAvailable();
}

} // namespace

NodeApi::NodeApi(ArkUI_ContextHandle uiContext) : m_uiContext(uiContext) {}

ArkUI_NodeHandle NodeApi::createNode(ArkUI_NodeType nodeType) {
  if (IsNewAPIVersion() && m_uiContext != nullptr) {
    return createNodeWithUIContext(nodeType);
  }

  return NativeNodeApi::getInstance()->createNode(nodeType);
}

void NodeApi::setAttribute(
    ArkUI_NodeHandle node,
    ArkUI_NodeAttributeType attribute,
    const ArkUI_AttributeItem* item) {
  if (IsNewAPIVersion() && m_uiContext != nullptr) {
    setAttributeWithUIContext(node, attribute, item);
    return;
  }

  maybeThrow(NativeNodeApi::getInstance()->setAttribute(node, attribute, item));
}

void NodeApi::resetAttribute(
    ArkUI_NodeHandle node,
    ArkUI_NodeAttributeType attribute) {
  if (IsNewAPIVersion() && m_uiContext != nullptr) {
    resetAttributeWithUIContext(node, attribute);
    return;
  }

  maybeThrow(NativeNodeApi::getInstance()->resetAttribute(node, attribute));
}

const ArkUI_AttributeItem* NodeApi::getAttribute(
    ArkUI_NodeHandle node,
    ArkUI_NodeAttributeType attribute) {
  return NativeNodeApi::getInstance()->getAttribute(node, attribute);
}

void NodeApi::setMeasuredSize(
    ArkUI_NodeHandle node,
    int32_t width,
    int32_t height) {
  if (IsNewAPIVersion() && m_uiContext != nullptr) {
    setMeasuredSizeWithUIContext(node, width, height);
    return;
  }

  maybeThrow(
      NativeNodeApi::getInstance()->setMeasuredSize(node, width, height));
}

void NodeApi::insertChildAt(
    ArkUI_NodeHandle parent,
    ArkUI_NodeHandle child,
    int32_t position) {
  if (IsNewAPIVersion() && m_uiContext != nullptr) {
    insertChildAtWithUIContext(parent, child, position);
    return;
  }

  maybeThrow(
      NativeNodeApi::getInstance()->insertChildAt(parent, child, position));
}

void NodeApi::addChild(ArkUI_NodeHandle parent, ArkUI_NodeHandle child) {
  if (IsNewAPIVersion() && m_uiContext != nullptr) {
    addChildWithUIContext(parent, child);
    return;
  }

  maybeThrow(NativeNodeApi::getInstance()->addChild(parent, child));
}

ArkUI_NodeHandle NodeApi::createNodeWithUIContext(ArkUI_NodeType nodeType) {
  CreateNodeData data{nodeType, nullptr};
  int32_t result = DynamicArkUILoader::getRunTaskFunction()(
      m_uiContext, &data, [](void* userData) {
        CreateNodeData* data = static_cast<CreateNodeData*>(userData);
        if (data == nullptr) {
          LOG(ERROR)
              << "createNodeWithUIContext callback called with null userData";
          return;
        }
        data->nodeHandle = NativeNodeApi::getInstance()->createNode(data->type);
      });
  if (result == ARKUI_ERROR_CODE_NO_ERROR && data.nodeHandle != nullptr) {
    return data.nodeHandle;
  } else {
    if (data.nodeHandle != nullptr) {
      NativeNodeApi::getInstance()->disposeNode(data.nodeHandle);
    }
    return NativeNodeApi::getInstance()->createNode(nodeType);
  }
}

void NodeApi::setAttributeWithUIContext(
    ArkUI_NodeHandle node,
    ArkUI_NodeAttributeType attribute,
    const ArkUI_AttributeItem* item) {
  auto* taskData = new SetAttributeTaskData{node, attribute, *item};
  static void (*callback)(void*) = [](void* userData) {
    SetAttributeTaskData* data = static_cast<SetAttributeTaskData*>(userData);
    if (data == nullptr) {
      LOG(ERROR)
          << "setAttributeWithUIContext callback called with null userData";
      return;
    }
    NativeNodeApi::getInstance()->setAttribute(
        data->node, data->attribute, &(data->item));
    delete data;
  };
  int32_t result =
      DynamicArkUILoader::getRunTaskFunction()(m_uiContext, taskData, callback);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    delete taskData;
    LOG(ERROR) << "setAttributeWithUIContext failed to run task in scope";
  }
  maybeThrow(result);
}

void NodeApi::resetAttributeWithUIContext(
    ArkUI_NodeHandle node,
    ArkUI_NodeAttributeType attribute) {
  auto* taskData = new ResetAttributeTaskData{node, attribute};
  static void (*callback)(void*) = [](void* userData) {
    ResetAttributeTaskData* data =
        static_cast<ResetAttributeTaskData*>(userData);
    if (data == nullptr) {
      LOG(ERROR)
          << "resetAttributeWithUIContext callback called with null userData";
      return;
    }
    NativeNodeApi::getInstance()->resetAttribute(data->node, data->attribute);
    delete data;
  };
  int32_t result =
      DynamicArkUILoader::getRunTaskFunction()(m_uiContext, taskData, callback);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    delete taskData;
    LOG(ERROR) << "resetAttributeWithUIContext failed to run task in scope";
  }
  maybeThrow(result);
}

void NodeApi::setMeasuredSizeWithUIContext(
    ArkUI_NodeHandle node,
    int32_t width,
    int32_t height) {
  auto* taskData = new MeasuredSizeData{node, width, height};
  static void (*callback)(void*) = [](void* userData) {
    MeasuredSizeData* data = static_cast<MeasuredSizeData*>(userData);
    if (data == nullptr) {
      LOG(ERROR)
          << "setMeasuredSizeWithUIContext callback called with null userData";
      return;
    }
    NativeNodeApi::getInstance()->setMeasuredSize(
        data->node, data->width, data->height);
    delete data;
  };
  int32_t result =
      DynamicArkUILoader::getRunTaskFunction()(m_uiContext, taskData, callback);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    delete taskData;
    LOG(ERROR) << "setMeasuredSizeWithUIContext failed to run task in scope";
  }
  maybeThrow(result);
}

void NodeApi::insertChildAtWithUIContext(
    ArkUI_NodeHandle parent,
    ArkUI_NodeHandle child,
    int32_t position) {
  auto* taskData = new NodeChildData{parent, child, position};
  static void (*callback)(void*) = [](void* userData) {
    NodeChildData* data = static_cast<NodeChildData*>(userData);
    if (data == nullptr) {
      LOG(ERROR)
          << "insertChildAtWithUIContext callback called with null userData";
      return;
    }
    NativeNodeApi::getInstance()->insertChildAt(
        data->parent, data->child, data->position);
    delete data;
  };
  int32_t result =
      DynamicArkUILoader::getRunTaskFunction()(m_uiContext, taskData, callback);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    delete taskData;
    LOG(ERROR) << "insertChildAtWithUIContext failed to run task in scope";
  }
  maybeThrow(result);
}

void NodeApi::addChildWithUIContext(
    ArkUI_NodeHandle parent,
    ArkUI_NodeHandle child) {
  auto* taskData = new NodeChildData{parent, child};
  static void (*callback)(void*) = [](void* userData) {
    NodeChildData* data = static_cast<NodeChildData*>(userData);
    if (data == nullptr) {
      LOG(ERROR) << "addChildWithUIContext callback called with null userData";
      return;
    }
    NativeNodeApi::getInstance()->addChild(data->parent, data->child);
    delete data;
  };
  int32_t result =
      DynamicArkUILoader::getRunTaskFunction()(m_uiContext, taskData, callback);
  if (result != ARKUI_ERROR_CODE_NO_ERROR) {
    delete taskData;
    LOG(ERROR) << "addChildWithUIContext failed to run task in scope";
  }
  maybeThrow(result);
}
} // namespace rnoh

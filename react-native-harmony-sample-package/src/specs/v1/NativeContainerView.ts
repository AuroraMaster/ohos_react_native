/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { HostComponent, ViewProps } from "react-native";
import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";

export interface NativeContainerViewProps extends ViewProps {}

export default codegenNativeComponent<NativeContainerViewProps>(
  "ContainerView",
) as HostComponent<NativeContainerViewProps>;

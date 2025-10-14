/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
import { HostComponent, ViewProps } from "react-native";
import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";

export interface NativeBindSheetViewProps extends ViewProps {}

export default codegenNativeComponent<NativeBindSheetViewProps>(
  "BindSheetView",
) as HostComponent<NativeBindSheetViewProps>;

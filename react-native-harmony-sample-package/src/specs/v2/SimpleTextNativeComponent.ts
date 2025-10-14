/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { ViewProps, HostComponent } from 'react-native';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';

import type { DirectEventHandler } from 'react-native/Libraries/Types/CodegenTypes';

export interface TextChangeEvent {
  text: string;
}

export interface SimpleTextNativeProps extends ViewProps {
  onTextChange: DirectEventHandler<TextChangeEvent>;
  text: string;
}

export default codegenNativeComponent<SimpleTextNativeProps>(
  'SimpleText'
) as HostComponent<SimpleTextNativeProps>;

/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import ArkTSNativeComponent from './specs/codegen-lib/CodegenLibArkTSSampleNativeComponent';
import CppNativeComponent from './specs/codegen-lib/CodegenLibCppSampleNativeComponent';
import { ViewStyle } from 'react-native';

export function CodegenLibSampleComponent({
  style,
  implementation,
  text,
  onMount,
}: {
  style: ViewStyle;
  implementation: 'ArkTS' | 'Cpp';
  onMount: (text: string) => void;
  text: string;
}) {
  if (implementation === 'ArkTS') {
    return (
      <ArkTSNativeComponent
        style={style}
        text={text}
        onMount={(e) => {
          onMount(e.nativeEvent.text);
        }}
      />
    );
  } else {
    return (
      <CppNativeComponent
        style={style}
        text={text}
        onMount={(e) => {
          onMount(e.nativeEvent.text);
        }}
      />
    );
  }
}

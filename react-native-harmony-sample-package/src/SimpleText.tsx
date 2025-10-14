/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import SimpleTextNativeComponent from './specs/v2/SimpleTextNativeComponent';
import { ViewProps } from 'react-native';

type SimpleTextProps = {
  text: string;
  onTextChange: (text: string) => void;
} & ViewProps;

/**
 * Animated.createAnimatedComponent seems to work only with class components.
 */
export class SimpleText extends React.Component<SimpleTextProps> {
  render(): React.ReactNode {
    return (
      <SimpleTextNativeComponent
        {...this.props}
        text={this.props.text}
        onTextChange={(e) => {
          this.props.onTextChange(e.nativeEvent.text);
        }}
      />
    );
  }
}

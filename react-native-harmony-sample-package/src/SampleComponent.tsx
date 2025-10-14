/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, { useRef } from 'react';
import { SampleNativeComponent } from './SampleNativeComponent';
import {
  ColorValue,
  NativeSyntheticEvent,
  StyleProp,
  UIManager,
  ViewStyle,
  findNodeHandle,
  processColor,
} from 'react-native';

export type SampleComponentRef = { toggleFontSize: () => void };

export const SampleComponent = React.forwardRef<
  SampleComponentRef,
  {
    children?: any;
    backgroundColor: string;
    size: number;
    textColor?: number | ColorValue;
    onSampleClick?: (event: NativeSyntheticEvent<{}>) => void;
  }
>(({ children, backgroundColor, size, textColor, onSampleClick }, ref) => {
  const nativeRef = useRef<any>(null);

  React.useImperativeHandle(
    ref,
    () => ({
      toggleFontSize() {
        if (nativeRef?.current) {
          UIManager.dispatchViewManagerCommand(
            findNodeHandle(nativeRef.current),
            'toggleFontSize',
            []
          );
        }
      },
    }),
    []
  );

  const style: StyleProp<ViewStyle> = {
    backgroundColor: backgroundColor,
    width: size,
    height: size,
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    borderColor: 'green',
    borderWidth: 1,
    borderRadius: 4,
    opacity: 1,
  };

  const processedTextColor = processColor(textColor ?? 'white')!;

  return (
    <SampleNativeComponent
      ref={nativeRef}
      style={style}
      size={size}
      children={children}
      textColor={processedTextColor}
      onSampleClick={onSampleClick}
    />
  );
});

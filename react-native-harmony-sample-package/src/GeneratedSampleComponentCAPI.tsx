/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import GeneratedSampleNativeComponent, {
  OutgoingData,
  IncomingData,
  ArgsOfEmitCommandArgs,
  SupportedCommandArgs,
  Commands,
} from './specs/v2/GeneratedSampleNativeComponent';
import { processColor } from 'react-native'

export type GeneratedSampleComponentCAPIRef = {
  emitNativeEvent: (eventType: 'directEvent' | 'bubblingEvent') => void;
  emitCommandArgs: (...args: ArgsOfEmitCommandArgs) => void;
};

export const GeneratedSampleComponentCAPI = React.forwardRef<
  GeneratedSampleComponentCAPIRef,
  {
    hidden?: boolean;
    children?: any;
    testProps: Omit<OutgoingData, 'colorTest'> & { colorTest: string };
    onDirectEvent?: (args: IncomingData) => void;
    onBubblingEvent?: (args: IncomingData) => void;
    onReceivedCommandArgs?: (args: SupportedCommandArgs) => void;
  }
>(
  (
    {
      children,
      hidden,
      testProps: { colorTest, ...otherTestProps },
      onDirectEvent,
      onBubblingEvent,
      onReceivedCommandArgs,
    },
    ref
  ) => {
    const nativeRef = React.useRef<any>(null);

    React.useImperativeHandle(
      ref,
      () => ({
        emitNativeEvent(eventType) {
          if (nativeRef?.current) {
            Commands.emitNativeEvent(nativeRef.current, eventType);
          }
        },
        emitCommandArgs(...args: ArgsOfEmitCommandArgs) {
          if (nativeRef?.current) {
            Commands.emitCommandArgs(nativeRef.current, ...args);
          }
        },
      }),
      []
    );

    return (
      <GeneratedSampleNativeComponent
        ref={nativeRef}
        style={{
          width: '100%',
          height: hidden ? 0 : 272,
          backgroundColor: 'white',
          borderWidth: 2,
          borderColor: 'pink',
        }}
        children={children}
        {...otherTestProps}
        colorTest={processColor(colorTest)!}
        onBubblingEvent={(e) => {
          onBubblingEvent?.(e.nativeEvent);
        }}
        onDirectEvent={(e) => {
          onDirectEvent?.(e.nativeEvent);
        }}
        onReceivedCommandArgs={(e) => {
          onReceivedCommandArgs?.(e.nativeEvent);
        }}
      />
    );
  }
);

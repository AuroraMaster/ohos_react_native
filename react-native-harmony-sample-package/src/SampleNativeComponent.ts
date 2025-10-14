/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  NativeSyntheticEvent,
  ProcessedColorValue,
  requireNativeComponent,
  ViewProps,
} from 'react-native';

/**
 * NOTE: Using codegen and `codegenNativeComponent` is recommended. Check spec files. The following approaches can be used without codegen.
 */

/**
 *
 * Importing custom native components:
 * NOTE: Those functions need to be in their own files, because hot reloading can trigger them more than once..
 *
 * 1) SampleView JSI binder on CPP side needs to be provided to make this function work.
 */
export const SampleNativeComponent = requireNativeComponent<
  ViewProps & {
    size: number;
    textColor: ProcessedColorValue;
    onSampleClick?: (event: NativeSyntheticEvent<{}>) => void;
  }
>('SampleView');

/**
 * 2) An alternative to JSI binders and `requireNativeComponent` is `registerViewConfig` function (Harmony-only API).
 *
    const SampleView = registerViewConfig('SampleView', () => {
      return {
        uiViewClassName: 'SampleView',
        bubblingEventTypes: {},
        directEventTypes: {},
        validAttributes: {
          ...ReactNativeViewAttributes.UIView,
          size: true,
        },
      };
    });
 */

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {
  AnyJSBundleProvider,
  ComponentBuilderContext,
  FileJSBundleProvider,
  MetroJSBundleProvider,
  ResourceJSBundleProvider,
  RNApp,
  RNOHErrorDialog,
  RNOHLogger,
  TraceJSBundleProviderDecorator,
  RNOHCoreContext
} from '@rnoh/react-native-openharmony';
import { createRNPackages } from '../RNPackagesFactory';
import { RNCVideo, RNC_VIDEO_TYPE } from '@react-native-oh-tpl/react-native-video';
import { RNGestureHandlerModule } from '@react-native-oh-tpl/react-native-gesture-handler';

@Builder
export function buildCustomRNComponent(ctx: ComponentBuilderContext) {
  if (ctx.componentName === RNC_VIDEO_TYPE) {
    RNCVideo({
      ctx: ctx.rnComponentContext,
      tag: ctx.tag
    })
  }
}

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomRNComponent);

@Entry
@Component
struct Index {
  @StorageLink('RNOHCoreContext') private rnohCoreContext: RNOHCoreContext | undefined = undefined;
  @State shouldShow: boolean = false;
  private logger!: RNOHLogger;

  aboutToAppear() {
    this.logger = this.rnohCoreContext!.logger.clone('Index');
    const stopTracing = this.logger.clone('aboutToAppear').startTracing();
    this.shouldShow = true;
    stopTracing();
  }

  onBackPress(): boolean | undefined {
    this.rnohCoreContext!.dispatchBackPress();
    return true;
  }

  build() {
    Column() {
      if (this.rnohCoreContext && this.shouldShow) {
        if (this.rnohCoreContext?.isDebugModeEnabled) {
          RNOHErrorDialog({ ctx: this.rnohCoreContext })
        }
        RNApp({
          rnInstanceConfig: {
            createRNPackages,
            enableNDKTextMeasuring: true,
            enableBackgroundExecutor: false,
            enableCAPIArchitecture: true,
            arkTsComponentNames: [RNC_VIDEO_TYPE]
          },
          appKey: 'RNHomeFluency',
          wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
          jsBundleProvider: new TraceJSBundleProviderDecorator(
            new AnyJSBundleProvider([
              new MetroJSBundleProvider(),
              new FileJSBundleProvider('/data/storage/el2/base/files/bundle.harmony.js'),
              new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'hermes_bundle.hbc'),
              new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'bundle.harmony.js')
            ]),
            this.rnohCoreContext.logger),
        })
      }
    }
    .height('100%')
    .width('100%')
  }
}
/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  Tag,
  RNInstance,
  RNOHCoreContext,
  ResourceJSBundleProvider,
  RNSurface,
  RNComponentContext,
  RNOHContext,
  buildRNComponentForTag,
  RNPackageContext,
  RNPackage,
  ComponentBuilderContext
} from '@rnoh/react-native-openharmony';

@Builder
function buildCustomComponent(ctx: ComponentBuilderContext) {
  Stack() {
  }
  .position({ x: 0, y: 0 })
}

function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [];
}

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomComponent);

interface Params {
  rootTag: number | undefined;
}

@Entry
@Component
export struct Index {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined;
  private instance: RNInstance | undefined;
  private receivedInstance: RNInstance | undefined;
  private receivedRootTag: Tag | undefined;
  private bundlePath = 'bundle.harmony.js';
  @State isBundleReady: boolean = false;

  async aboutToAppear() {
    if (!this.rnohCoreContext) {
      return;
    }

    this.instance = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: true,
      arkTsComponentNames: []
    });
    await this.instance.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager, this.bundlePath))
      .then(() => {
        this.isBundleReady = true;
      })
  }

  build() {
    NavDestination() {
      if (this.isBundleReady) {
        Column() {
          Row() {
            RNSurface({
              surfaceConfig: {
                appKey: 'Hotel_Page',
              },
              ctx: new RNComponentContext(
                RNOHContext.fromCoreContext(this.rnohCoreContext!, this.instance),
                wrappedCustomRNComponentBuilder,
                wrapBuilder(buildRNComponentForTag),
                new Map()
              ),
              updateViewRootTag: (rnInstance: RNInstance, rootViewTag: Tag) => {
                // 将当前RNSurface的RNInstance和rootTag保存起来
                this.receivedRootTag = rootViewTag;
                this.receivedInstance = rnInstance;
              }
            })
          }
          .height("45%")

          Row() {
            Button("CLICK ME")
              .onClick(() => {
                // 向RN发送消息
                this.instance?.emitDeviceEvent("clickEvent", {
                  params: ({
                    rootTag: this.receivedRootTag
                  } as Params)
                })
              })
          }
          .height("10%")

          Row() {
            RNSurface({
              surfaceConfig: {
                appKey: 'Ticket_Page',
              },
              ctx: new RNComponentContext(
                RNOHContext.fromCoreContext(this.rnohCoreContext!, this.instance),
                wrappedCustomRNComponentBuilder,
                wrapBuilder(buildRNComponentForTag),
                new Map()
              ),
            })
          }
          .height("45%")
        }
        .width('100%')
        .height('100%')
      }
    }
  }
}
   
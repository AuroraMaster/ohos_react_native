/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { DescriptorWrapperFactoryByDescriptorType } from '../RNOH/ts';
import { RNPackage } from "../RNOH/RNPackage"
import type { AnyThreadTurboModule, UITurboModule } from '../RNOH/TurboModule';
import type { AnyThreadTurboModuleContext, UITurboModuleContext } from "../RNOH/RNOHContext"
import {
  AccessibilityInfoTurboModule,
  AlertManagerTurboModule,
  AppStateTurboModule,
  BlobTurboModule,
  DeviceEventManagerTurboModule,
  DeviceInfoTurboModule,
  DevLoadingViewTurboModule,
  DevMenuTurboModule,
  DevSettingsTurboModule,
  ExceptionsManagerTurboModule,
  FileReaderTurboModule,
  I18nManagerTurboModule,
  ImageLoaderTurboModule,
  KeyboardObserverTurboModule,
  LogBoxTurboModule,
  NativeAnimatedTurboModule,
  NetworkingTurboModule,
  PlatformColorTurboModule,
  PlatformConstantsTurboModule,
  SafeAreaTurboModule,
  ShareTurboModule,
  SourceCodeTurboModule,
  StatusBarTurboModule,
  TimingTurboModule,
  ToastAndroidTurboModule,
  VibrationTurboModule,
  WebSocketTurboModule,
  LinkingManagerTurboModule
} from './turboModules';
import { ViewDescriptorWrapper } from './components/ts';
import { AppearanceTurboModule } from './turboModules/AppearanceTurboModule';

export class RNOHCorePackage extends RNPackage {
  override createDescriptorWrapperFactoryByDescriptorType(): DescriptorWrapperFactoryByDescriptorType {
    return { View: ctx => new ViewDescriptorWrapper(ctx.descriptor) };
  }

  override getUITurboModuleFactoryByNameMap(): Map<string, (ctx: UITurboModuleContext) => UITurboModule | null> {
    return new Map<string, ((ctx: UITurboModuleContext) => UITurboModule)>()
      .set(AccessibilityInfoTurboModule.NAME, (ctx) => new AccessibilityInfoTurboModule(ctx))
      .set(AlertManagerTurboModule.NAME, (ctx) => new AlertManagerTurboModule(ctx))
      .set(AppStateTurboModule.NAME, (ctx) => new AppStateTurboModule(ctx))
      .set(DeviceEventManagerTurboModule.NAME, (ctx) => new DeviceEventManagerTurboModule(ctx))
      .set(DevSettingsTurboModule.NAME, (ctx) => new DevSettingsTurboModule(ctx))
      .set(DevMenuTurboModule.NAME, (ctx) => new DevMenuTurboModule(ctx))
      .set(ExceptionsManagerTurboModule.NAME, (ctx) => new ExceptionsManagerTurboModule(ctx))
      .set(ImageLoaderTurboModule.NAME, (ctx) => new ImageLoaderTurboModule(ctx))
      .set(KeyboardObserverTurboModule.NAME, (ctx) => new KeyboardObserverTurboModule(ctx))
      .set(NativeAnimatedTurboModule.NAME, (ctx) => new NativeAnimatedTurboModule(ctx))
      .set(LinkingManagerTurboModule.NAME, (ctx) => new LinkingManagerTurboModule(ctx))
      .set(PlatformColorTurboModule.NAME, (ctx) => new PlatformColorTurboModule(ctx))
      .set(PlatformConstantsTurboModule.NAME, (ctx) => new PlatformConstantsTurboModule(ctx))
      .set(ShareTurboModule.NAME, (ctx) => new ShareTurboModule(ctx))
      .set(SourceCodeTurboModule.NAME, (ctx) => new SourceCodeTurboModule(ctx))
      .set(TimingTurboModule.NAME, (ctx) => new TimingTurboModule(ctx))
      .set(I18nManagerTurboModule.NAME, (ctx) => new I18nManagerTurboModule(ctx))
      .set(LogBoxTurboModule.NAME, (ctx) => new LogBoxTurboModule(ctx))
      .set(DevLoadingViewTurboModule.NAME, (ctx) => new DevLoadingViewTurboModule(ctx))
      .set(VibrationTurboModule.NAME, (ctx) => new VibrationTurboModule(ctx))
      .set(ToastAndroidTurboModule.NAME, (ctx) => new ToastAndroidTurboModule(ctx))
  }

  override async createEagerUITurboModuleByNameMap(ctx: UITurboModuleContext): Promise<Map<string, UITurboModule>> {
    const deviceInfoTurboModulePromise = DeviceInfoTurboModule.create(ctx)
    const statusBarTurboModule = new StatusBarTurboModule(ctx);
    const safeAreaTurboModulePromise = SafeAreaTurboModule.create(
      ctx,
      statusBarTurboModule,
    )
    return new Map()
      .set(DeviceInfoTurboModule.NAME, await deviceInfoTurboModulePromise)
      .set(SafeAreaTurboModule.NAME, await safeAreaTurboModulePromise)
      .set(StatusBarTurboModule.NAME, statusBarTurboModule)
  }

  override getAnyThreadTurboModuleFactoryByNameMap(): Map<string, (ctx: AnyThreadTurboModuleContext) => AnyThreadTurboModule | null> {
    return new Map<string, ((ctx: AnyThreadTurboModuleContext) => AnyThreadTurboModule)>()
      .set(AppearanceTurboModule.NAME, (ctx) => new AppearanceTurboModule(ctx))
      .set(NetworkingTurboModule.NAME, (ctx) => new NetworkingTurboModule(ctx))
      .set(WebSocketTurboModule.NAME, (ctx) => new WebSocketTurboModule(ctx))
      .set(BlobTurboModule.NAME, (ctx) => new BlobTurboModule(ctx))
      .set(FileReaderTurboModule.NAME, (ctx) => new FileReaderTurboModule(ctx))
  }

  override getDebugName() {
    return 'rnoh';
  }
}

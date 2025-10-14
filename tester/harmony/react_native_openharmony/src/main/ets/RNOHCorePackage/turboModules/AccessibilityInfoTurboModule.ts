/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import Accessibility from '@ohos.accessibility';
import type { TurboModuleContext, Tag } from '../../RNOH/ts';
import { TurboModule } from '../../RNOH/TurboModule';
import bundleManager from '@ohos.bundle.bundleManager';

type AccessibilityFeature = "SCREEN_READER"
type AccessibilityDeviceEvent = "screenReaderChanged"

type RNAccessibilityEvent = {
  type: "click",
  targetId: string,
} | {
  type: "focus",
  targetId: string,
} | {
  type: "viewHoverEnter"
  targetId: string,
}

export class AccessibilityInfoTurboModule extends TurboModule {
  public static readonly NAME = 'AccessibilityInfo';

  private cleanUpCallbacks: (() => void)[] = [];
  private enabledAccessibilityFeatures = new Set<AccessibilityFeature>()
  private deviceEventByAccessibilityFeatureName =
    new Map<AccessibilityFeature, AccessibilityDeviceEvent>().set("SCREEN_READER", "screenReaderChanged")

  constructor(ctx: TurboModuleContext) {
    super(ctx);
    this.enabledAccessibilityFeatures = this.getEnabledAccessibilityFeatures()
    this.subscribeToAccessibilityEvents();
    this.cleanUpCallbacks.push(ctx.rnInstance.cppEventEmitter.subscribe("RNOH::schedulerDidSendAccessibilityEvent",
      (eventType: RNAccessibilityEvent) => {
        this.schedulerDidSendAccessibilityEvent(eventType)
      }))
  }

  private getEnabledAccessibilityFeatures(): Set<AccessibilityFeature> {
    const accessibilityExtensions = Accessibility.getAccessibilityExtensionListSync("all", "enable")
    const newEnabledAccessibilityFeatures = new Set<AccessibilityFeature>()
    for (const accessibilityExtension of accessibilityExtensions) {
      if (accessibilityExtension.bundleName === "com.huawei.hmos.screenreader") {
        newEnabledAccessibilityFeatures.add("SCREEN_READER")
      }
    }
    return newEnabledAccessibilityFeatures
  }

  __onDestroy__() {
    super.__onDestroy__();
    this.cleanUpCallbacks.forEach(cb => cb());
    this.cleanUpCallbacks = [];
  }

  private subscribeToAccessibilityEvents() {
    const onAccessibilityStateChange = (isOn: boolean) => this.onAccessibilityStateChange(isOn)
    Accessibility.on("accessibilityStateChange", onAccessibilityStateChange);
    this.cleanUpCallbacks.push(() => {
      Accessibility.off("accessibilityStateChange", onAccessibilityStateChange);
    });
  }

  public async setAccessibilityFocus(tag: Tag) {
    const bundleName = (await bundleManager.getBundleInfoForSelf(-1)).name;
    Accessibility.sendAccessibilityEvent({
      type: "requestFocusForAccessibility",
      bundleName,
      triggerAction: "common",
      customId: this.ctx.rnInstance.getNativeNodeIdByTag(tag),
    })
  }

  /**
   * This method is called indirectly by SchedulerDelegate
   */
  private async schedulerDidSendAccessibilityEvent(event: RNAccessibilityEvent) {
    const bundleName = (await bundleManager.getBundleInfoForSelf(-1)).name;
    switch (event.type) {
      case "click": {
        /**
         * On Android 'click' works with aria-selected or aria-checked.
         * At the moment of adding this case, aria-selected and aria-checked props aren't supported.
         * This case may need to be modified to match Android behavior.
         *
         * Android uses TYPE_VIEW_CLICKED:
         * https://developer.android.com/reference/android/view/accessibility/AccessibilityEvent#TYPE_VIEW_CLICKED
         */
        Accessibility.sendAccessibilityEvent({
          type: "click",
          bundleName,
          triggerAction: "click",
          customId: event.targetId
        })
        break;
      }
      case "focus": {
        Accessibility.sendAccessibilityEvent({
          type: "requestFocusForAccessibility",
          bundleName,
          triggerAction: "common",
          customId: event.targetId,
        })
        break;
      }
      case "viewHoverEnter": {
        /**
         * This case may need to be updated. At the moment of adding this case, OHOS doesn't
         * handle hovering in the same manner as Android. The behavior of this case differs from Android. It might be
         * a problem with hovering in general or this case is not handled properly.
         */
        Accessibility.sendAccessibilityEvent({
          type: "hoverEnter",
          bundleName,
          triggerAction: "common",
          customId: event.targetId,
        })
        break;
      }
      default: {
        this.ctx.logger.warn(`Unsupported eventType`)
      }
    }
  }

  private onAccessibilityStateChange(isOn: boolean) {
    const newEnabledAccessibilityFeatures = this.getEnabledAccessibilityFeatures()
    const oldEnabledAccessibilityFeatures = this.enabledAccessibilityFeatures;
    const { added, removed } = compareSets(oldEnabledAccessibilityFeatures, newEnabledAccessibilityFeatures)
    added.forEach(enabledFeature => {
      this.ctx.rnInstance.emitDeviceEvent(this.deviceEventByAccessibilityFeatureName.get(enabledFeature), true)
    })
    removed.forEach(disabledFeature => {
      this.ctx.rnInstance.emitDeviceEvent(this.deviceEventByAccessibilityFeatureName.get(disabledFeature), false)
    })
    this.ctx.rnInstance.emitDeviceEvent("accessibilityServiceChanged", isOn);
    this.enabledAccessibilityFeatures = newEnabledAccessibilityFeatures;
  }

  async announceForAccessibility(announcement: string) {
    const bundleInfo = await bundleManager.getBundleInfoForSelf(-1);
    Accessibility.sendAccessibilityEvent({
      type: "announceForAccessibility",
      bundleName: bundleInfo.name,
      triggerAction: "common",
      textAnnouncedForAccessibility: announcement
    });
  }

  public isScreenReaderEnabled(): Promise<boolean> {
    return Promise.resolve(this.enabledAccessibilityFeatures.has("SCREEN_READER"));
  }

  public isBoldTextEnabled(): Promise<boolean> {
    /**
     * Min value that can be set in the settings: 0.9
     * Max value that can be set in the settings: 1.25
     */
    const fontWeightScale = this.ctx.uiAbilityContext.config.fontWeightScale ?? 1;
    /**
     * OHOS classifies font weight as bold if the fontWeightScale is larger than 1.
     */
    return Promise.resolve(fontWeightScale > 1)
  }

  public async isAccessibilityServiceEnabled(): Promise<boolean> {
    const enabledAccessibilityExtensions = await Accessibility.getAccessibilityExtensionList("all", "enable")
    return enabledAccessibilityExtensions.length > 0
  }
}

function compareSets<T>(oldSet: Set<T>, newSet: Set<T>): {
  added: Set<T>;
  removed: Set<T>;
} {
  const added = new Set<T>([...newSet].filter(x =>!oldSet.has(x)));
  const removed = new Set<T>([...oldSet].filter(x =>!newSet.has(x)));

  return { added, removed };
}

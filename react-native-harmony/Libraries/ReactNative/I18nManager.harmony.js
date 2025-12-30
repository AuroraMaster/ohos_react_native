/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import NativeI18nManager from 'react-native/Libraries/ReactNative/NativeI18nManager';
import RCTDeviceEventEmitter from 'react-native/Libraries/EventEmitter/RCTDeviceEventEmitter';
import type {EventSubscription} from 'react-native/Libraries/vendor/emitter/EventEmitter';

const i18nConstants: {|
  doLeftAndRightSwapInRTL: boolean,
  isRTL: boolean,
  localeIdentifier?: ?string,
|} = getI18nManagerConstants();

let harmonyI18nConstants: {|
  doLeftAndRightSwapInRTL: boolean,
  isRTL: boolean,
  localeIdentifier?: ?string,
|} | null = null;

function getI18nManagerConstants() {
  if (NativeI18nManager) {
    const {isRTL, doLeftAndRightSwapInRTL, localeIdentifier} =
      NativeI18nManager.getConstants();
    return {isRTL, doLeftAndRightSwapInRTL, localeIdentifier};
  }

  return {
    isRTL: false,
    doLeftAndRightSwapInRTL: true,
  };
}

function getHarmonyI18nManagerConstants() {  
  if (harmonyI18nConstants == null) {
    harmonyI18nConstants = getI18nManagerConstants();
  }
  return harmonyI18nConstants;
}

function invalidateHarmonyI18nConstantsCache() {
  harmonyI18nConstants = null;
}

let languageChangeListener: ?EventSubscription = null;

function addLanguageListener() {
  languageChangeListener = RCTDeviceEventEmitter.addListener("i18nManagerLanguageChanged", (event) => {
    invalidateHarmonyI18nConstantsCache();
  });
}

function setupLanguageChangeListener() {
  if (!languageChangeListener) {    
    try {
      if (RCTDeviceEventEmitter && typeof RCTDeviceEventEmitter.addListener === 'function') {
        addLanguageListener();
      }
    } catch (error) {
      console.error('Failed to register listener:', error);
    }
  }
}

setupLanguageChangeListener();

module.exports = {
  getConstants: (): {|
    doLeftAndRightSwapInRTL: boolean,
    isRTL: boolean,
    localeIdentifier: ?string,
  |} => {
    return getHarmonyI18nManagerConstants();
  },

  allowRTL: (shouldAllow: boolean) => {
    if (!NativeI18nManager) {
      return;
    }

    NativeI18nManager.allowRTL(shouldAllow);
    invalidateHarmonyI18nConstantsCache();
  },

  forceRTL: (shouldForce: boolean) => {
    if (!NativeI18nManager) {
      return;
    }

    NativeI18nManager.forceRTL(shouldForce);
    invalidateHarmonyI18nConstantsCache();
  },

  swapLeftAndRightInRTL: (flipStyles: boolean) => {
    if (!NativeI18nManager) {
      return;
    }

    NativeI18nManager.swapLeftAndRightInRTL(flipStyles);
    invalidateHarmonyI18nConstantsCache();
  },

  get isRTL() {
    return getHarmonyI18nManagerConstants().isRTL;
  },

  get doLeftAndRightSwapInRTL() {
    return getHarmonyI18nManagerConstants().doLeftAndRightSwapInRTL;
  },
};

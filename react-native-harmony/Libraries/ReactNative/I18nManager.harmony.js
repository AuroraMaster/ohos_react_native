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

const i18nConstants: {|
  doLeftAndRightSwapInRTL: boolean,
  isRTL: boolean,
  localeIdentifier?: ?string,
|} = getI18nManagerConstants();

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

module.exports = {
  getConstants: (): {|
    doLeftAndRightSwapInRTL: boolean,
    isRTL: boolean,
    localeIdentifier: ?string,
  |} => {
    // RNOH patch
    // return i18nConstants;
    return Platform.OS === "harmony" ? getI18nManagerConstants() : i18nConstants;
  },

  allowRTL: (shouldAllow: boolean) => {
    if (!NativeI18nManager) {
      return;
    }

    NativeI18nManager.allowRTL(shouldAllow);
  },

  forceRTL: (shouldForce: boolean) => {
    if (!NativeI18nManager) {
      return;
    }

    NativeI18nManager.forceRTL(shouldForce);
  },

  swapLeftAndRightInRTL: (flipStyles: boolean) => {
    if (!NativeI18nManager) {
      return;
    }

    NativeI18nManager.swapLeftAndRightInRTL(flipStyles);
  },

  // RNOH patch
  get isRTL() {
    return Platform.OS === "harmony" ? getI18nManagerConstants().isRTL : i18nConstants.isRTL;
  },
  // isRTL: i18nConstants.isRTL,

  // RNOH patch
  get doLeftAndRightSwapInRTL() {
    return Platform.OS === "harmony" ? getI18nManagerConstants().doLeftAndRightSwapInRTL : i18nConstants.doLeftAndRightSwapInRTL;
  },
  // doLeftAndRightSwapInRTL: i18nConstants.doLeftAndRightSwapInRTL,
};

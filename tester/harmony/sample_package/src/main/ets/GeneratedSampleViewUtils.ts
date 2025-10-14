/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNC } from "@rnoh/react-native-openharmony/generated/ts"


export function getDirectProps(descriptorWrapper: RNC.GeneratedSampleViewArkTS.DescriptorWrapper ): Record<keyof RNC.GeneratedSampleViewArkTS.DirectRawProps, any> {
  const props = descriptorWrapper.props
  return {
    booleanTest: props.booleanTest,
    booleanWithDefaultTest: props.booleanWithDefaultTest,
    intTest: props.intTest,
    intWithDefault: props.intWithDefault,
    floatTest: props.floatTest,
    floatWithDefaultTest: props.floatWithDefaultTest,
    doubleTest: props.doubleTest,
    doubleWithDefaultTest: props.doubleWithDefaultTest,
    stringTest: props.stringTest,
    stringWithDefaultTest: props.stringWithDefaultTest,
    objectTest: props.objectTest,
    colorTest: props.colorTest.toRGBAString(),
    arrayTest: props.arrayTest,
    readOnlyArrayTest: props.readOnlyArrayTest,
    stringEnumTest: props.stringEnumTest,
    intEnumTest: props.intEnumTest
  }
}

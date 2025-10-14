/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { CodegenLibSampleModule } from "./generated/turboModules/CodegenLibSampleModule"
import { WorkerTurboModule } from "@rnoh/react-native-openharmony/ts"

export class SampleLibGenTurboModule extends WorkerTurboModule implements CodegenLibSampleModule.Spec {
  getNumber(): number {
    return 42
  }
}
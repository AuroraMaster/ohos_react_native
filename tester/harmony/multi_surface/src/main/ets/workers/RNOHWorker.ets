/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * Importing some ets files in the following way results in runtime errors e.g.
 * import { setupRNOHWorker } from "@rnoh/react-native-openharmony"
 *
 * It looks like this is ArkTS problem and it may be fixed in the future.
 * For the time being, use direct imports.
 */
import { setupRNOHWorker } from "@rnoh/react-native-openharmony/src/main/ets/setupRNOHWorker"
import { createRNPackages } from "../RNPackagesFactory"

setupRNOHWorker({
  createWorkerRNInstanceConfig: (_rnInstanceName) => {
    return { thirdPartyPackagesFactory: createRNPackages }
  }
})


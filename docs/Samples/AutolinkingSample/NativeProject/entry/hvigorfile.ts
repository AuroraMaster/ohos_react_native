/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { hapTasks } from '@ohos/hvigor-ohos-plugin';
import { createRNOHModulePlugin } from "@rnoh/hvigor-plugin"

export default {
  system: hapTasks, /* Built-in plugin of Hvigor. It cannot be modified. */
  plugins: [
    createRNOHModulePlugin({
      nodeModulesPath: "../ReactProject/node_modules",
      codegen: null,
      autolinking: {}
    }),
  ]
}

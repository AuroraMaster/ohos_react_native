/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// Script for compiling build behavior. It is built in the build plug-in and cannot be modified currently.
export { hapTasks } from '@ohos/hvigor-ohos-plugin';
import { hapTasks } from '@ohos/hvigor-ohos-plugin';
import { createRNOHHvigorPlugin } from "@rnoh/hvigor-plugin"

export default {
  system: hapTasks,
  plugins: [
    createRNOHHvigorPlugin({
      codegen: {
        rnohModulePath: "./react_native_openharmony",
      }
    }),
  ],
}

/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import { Command as CLICommand } from '@react-native-community/cli-types';

// RNOH patch: The parse function in the Command.options type is missing an extra argument in @react-native-community/cli-types.
// @react-native-community/cli parses options with Commander.js, which allows defining a custom parse function. This function
// receives two arguments: the value to parse and the previous values. The default parse function in the CLI is defined as (val) => val,
// which makes parsing multiple values impossible because this function will always return the last value.
export type Command = Omit<CLICommand, 'options'> & {
  options: Array<{
    name: string;
    description: string;
    default?: any;
    parse?: (val: string, prev: any) => any;
  }>;
};

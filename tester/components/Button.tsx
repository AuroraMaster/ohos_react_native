/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {Text, TouchableHighlight} from 'react-native';
import {PALETTE} from './palette';

export function Button({label, onPress}: {onPress: () => void; label: string}) {
  return (
    <TouchableHighlight
      underlayColor={PALETTE.REACT_CYAN_DARK}
      style={{
        paddingVertical: 6,
        paddingHorizontal: 12,
        backgroundColor: PALETTE.REACT_CYAN_LIGHT,
        borderWidth: 2,
        borderColor: PALETTE.REACT_CYAN_DARK,
      }}
      onPress={onPress}>
      <Text style={{color: 'black', fontWeight: 'bold', fontSize: 12}}>
        {label}
      </Text>
    </TouchableHighlight>
  );
}

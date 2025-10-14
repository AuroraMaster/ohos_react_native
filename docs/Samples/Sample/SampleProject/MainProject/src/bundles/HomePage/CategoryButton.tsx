/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Text, TouchableHighlight, StyleProp, TextStyle } from 'react-native';

export interface ButtonProps {
  label: string;
  textStyle?: StyleProp<TextStyle>;
  index?: number;
  onPress: () => void;
}

export default function CategoryButton({
  label,
  textStyle,
  index,
  onPress,
}: ButtonProps) {
  return (
    <TouchableHighlight
      underlayColor="hsl(210, 80%, 50%)"
      style={{
        flex: 1,
        paddingVertical: 6,
        paddingHorizontal: 12,
        backgroundColor: 'hsl(210, 80%, 50%)',
        alignItems: 'center',
        justifyContent: 'center',
      }}
      onPress={onPress}>
      <Text style={[{ color: 'white', marginTop: 40 }, textStyle]}>{label}</Text>
    </TouchableHighlight>
  );
}

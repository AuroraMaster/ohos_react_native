/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import { Text, Image, View, StyleSheet } from 'react-native';

const styles = StyleSheet.create({
  //创建样式
  text: {
    margin: 5,
  },
});

const Sandbox = () => {
  return (
    <View
      style={{
        marginTop: 20,
        backgroundColor: '#def5ee',
        width: '90%',
        borderRadius: 10,
        marginLeft: 'auto',
        marginRight: 'auto',
      }}>
      <Text style={{ fontWeight: 600, marginTop: 20 }}>沙箱bundle渲染</Text>
      <View
        style={{
          display: 'flex',
          alignItems: 'center',
          flexDirection: 'row',
          width: '100%',
        }}>
        <Image
          source={require('../../../assets/expo.png')}
          style={{
            width: 100,
            height: 100,
            margin: 10,
          }}
        />
        <Image
          source={{ uri: 'file:///data/storage/el2/base/files/call.png' }}
          style={{
            width: 100,
            height: 100,
            margin: 10,
          }}
        />
        <Image
          source={require('../../../assets/call.png')}
          style={{
            width: 100,
            height: 100,
            margin: 10,
          }}
        />
      </View>

      <Text style={styles.text}>
        如果图片未显示，需发送图片文件到沙箱。步骤如下bundletest1v{' '}
      </Text>
      <Text style={styles.text}>
        1.通过hdc file send命令或IDE中右下的device file
        browser都可发送文件到应用沙箱
      </Text>
      <Text style={styles.text}>
        2.再将assets文件夹和其中的图片都增加读写权限，可进入hdc
        shell中通过chmod命令完成
      </Text>
    </View>
  );
};

export default Sandbox;

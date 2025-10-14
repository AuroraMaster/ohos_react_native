/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import React, { useState } from 'react';
import { View, Image, Text, StyleSheet, ActivityIndicator } from 'react-native';

const BlankView: React.FC = React.memo(() => {
  const [isShow, setIsShow] = useState(false);
  let items = [];
  for (let i = 0; i < 5; i += 1) {
    items.push(
      <View style={styles.itemContainer}>
        <Image
          source={require('../../../assets/rect_middle.png')}
          style={styles.itemImage}
        />
        <Image
          source={require('../../../assets/rect_small.png')}
          style={styles.itemText}
        />
      </View>
    )
  }

  setTimeout(() => {
    setIsShow(true)
  }, 2000);
  return (
    <View>
      <View
        style={{
          marginTop: 270,
          display: isShow ? '' : 'none',
          alignItems: 'center',
        }}>
        <Image
          source={require('../../../assets/blank.png')}
          style={styles.blankImage}
        />
        <Text style={styles.blankText}>{'无法连接网络，请检查网络设置'}</Text>
      </View>
      <View style={{ height: 160, marginTop: 20, display: !isShow ? '' : 'none' }}>
        <View style={styles.rowContainer}>{items}</View>
        <View style={styles.rowContainer}>{items}</View>
      </View>
      <View style={{ position: 'relative', display: !isShow ? '' : 'none' }}>
        <View style={{ height: 500 }}>
          <View style={styles.rowContainer}>
            <Image
              source={require('../../../assets/rect_big.png')}
              style={styles.itemCard}
            />
            <Image
              source={require('../../../assets/rect_big.png')}
              style={styles.itemCard}
            />
          </View>
          <View style={styles.rowContainer}>
            <Image
              source={require('../../../assets/rect_big.png')}
              style={styles.itemCard}
            />
            <Image
              source={require('../../../assets/rect_big.png')}
              style={styles.itemCard}
            />
          </View>
        </View>
        <ActivityIndicator
          color="gray"
          size={100}
          style={{ position: 'absolute', top: 60, left: '36%' }}
        />
      </View>
    </View>
  );
});

const styles = StyleSheet.create({
  blankImage: {
    width: 80,
    height: 80,
    borderRadius: 5,
  },
  itemImage: {
    width: 40,
    height: 40,
    borderRadius: 5,
  },
  itemCard: {
    width: 170,
    height: 240,
    borderRadius: 5,
  },
  itemText: {
    marginTop: 10,
    width: 40,
    height: 8,
  },
  blankText: {
    marginTop: 10,
    color: '#9b9c9d',
  },
  itemContainer: {
    alignItems: 'center',
    width: '20%'
  },
  rowContainer: {
    flex: 1,
    flexDirection: 'row',
    justifyContent: 'space-around',
    alignItems: 'flex-start',
  },
});

export default BlankView;

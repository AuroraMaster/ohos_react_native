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

import React from 'react';
import {
  View,
  TextInput,
  Image,
  StyleSheet,
  Pressable,
} from 'react-native';
const SearchBar = () => {
  return (
    <View style={styles.searchContainer}>
      <View style={styles.backgroundSearch}>
        <Pressable
          onPress={() => {
            console.log('搜索按钮点击');
          }}>
          <Image
            source={require('../../../assets/search.png')}
            style={styles.icon}
          />
        </Pressable>
        <TextInput
          style={styles.input}
          placeholder="搜索..."
          placeholderTextColor="rgba(0, 0, 0, 0.6)"
          underlineColorAndroid="#e5e7e9"
        />
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    backgroundColor: '#ADD8E6',
  },
  searchContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
    borderRadius: 24,
    backgroundColor: 'rgba(0, 0, 0, 0.05)',
    margin: 8,
    marginLeft: 16,
    marginRight: 16,
  },
  input: {
    flex: 0.9,
    height: 40,
    width: 120,
    fontSize: 16,
  },
  icon: {
    width: 16,
    height: 16,
    marginRight: 8,
  },
  backgroundSearch: {
    flex: 1,
    flexDirection: 'row',
    justifyContent: 'center',
    alignItems: 'center',
  },
});

export default SearchBar;

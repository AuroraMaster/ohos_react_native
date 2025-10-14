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

import React, { Component } from 'react';
import {
  View,
  StyleSheet,
  Image,
} from 'react-native';

interface ImageCellProps {
  thumbnails: string;
  height: number;
  width: number;
}

class ImageCell extends Component<ImageCellProps> {
  render() {
    const {
      thumbnails,
      height,
      width
    } = this.props;
    return (
      <View style={styles.itemContainer}>
        <Image
          source={{ uri: thumbnails }}
          style={{ width: width, height: height }}
          resizeMode='cover'
        />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  itemContainer: {
    flex: 1,
    borderTopLeftRadius: 16,
    borderTopRightRadius: 16,
    flexDirection: 'column',
    overflow: 'hidden',
  },
});

export default ImageCell;

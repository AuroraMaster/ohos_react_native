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
  Text,
  StyleSheet,
  Dimensions,
  TouchableOpacity,
  Image,
} from 'react-native';
import Video from 'react-native-video';

interface VideoCellProps {
  thumbURL: string;
  height: number;
  width: number;
  videoURL: string;
}

class VideoCell extends Component<VideoCellProps> {
  state = {
    videoReady: false,
    paused: true,
    videoTotal: 0,
    currentTime: 0,
    videoProgress: 0,
  };
  onProgress = data => {
    let duration = data.playableDuration;
    let currentTime = data.currentTime;
    let percent = 0;

    //初始加载时duration=0
    if (duration != 0) {
      percent = Number((currentTime / duration).toFixed(2));
    } else {
      percent = 0;
    }

    this.setState({
      videoTotal: duration,
      currentTime: currentTime,
      videoProgress: percent,
      replay: false,
    });
  };

  componentDidMount() { }

  onLoad = () => {
    this.setState({
      videoReady: true,
    });
  };

  onEnd = () => {
    this.setState({
      videoProgress: 1,
      paused: true,
    });
  };

  render() {
    const {
      thumbURL,
      width,
      height
    } = this.props;
    return (
      <View style={styles.itemContainer}>
        <View style={{ width: width, height: height }}>
          <Image
            source={{ uri: thumbURL }}
            resizeMode="cover"
            style={{ width: width, height: height }}
          />
          {true && (
            <Image
              source={require('../../../assets/play.png')}
              style={styles.playIcon}
              resizeMode="cover"
            />
          )}
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  wrap: {
    position: 'absolute',
    top: 10,
    right: 10,
  },
  playIcon: {
    height: 28,
    width: 28,
    position: 'absolute',
    top: 10,
    right: 10,
  },
  itemContainer: {
    flex: 1,
    flexDirection: 'column',
    overflow: 'hidden',
    backgroundColor: 'white',
  },
  itemImage: {
    width: '100%',
  },
});

export default VideoCell;

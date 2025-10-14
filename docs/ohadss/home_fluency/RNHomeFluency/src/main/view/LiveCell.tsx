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
import { View, Text, StyleSheet, Dimensions, Image, DeviceEventEmitter, TouchableOpacity } from 'react-native';
import { CardData } from '../utils/NetWorkUtil';
import Video from 'react-native-video';

interface LiveCellProps {
  liveURL: string;
  thumbURL: string;
  _ref?: any;
  width: number;
  height: number;
  index: number;
}

class LiveCell extends Component<LiveCellProps> {
  private onViewableItemsChangedListen: any;
  onLoad = () => {
  };
  state = {
    paused: true
  }
  _ref = null
  componentDidMount() {
    this.onViewableItemsChangedListen = DeviceEventEmitter.addListener('onViewableItemsChanged', (cardList: CardData[]) => {
      if (!this._ref) {
        return;
      }
      const self = this
      let isPaused = true;
      for (let i = 0; i < cardList.length; i++) {
        if (cardList[i].index === self.props.index) {
          isPaused = false;
        }
      }
      self.setState({
        paused: isPaused
      })
    })
  }

  componentWillUnmount() {
    if (this.onViewableItemsChangedListen) {
      this.onViewableItemsChangedListen = null;
    }
  }

  onEnd = () => { };
  render() {
    const { thumbURL, liveURL, width, height } = this.props;
    const { paused } = this.state
    return (
      <View style={styles.itemContainer} ref={(ref) => (this._ref = ref)}>
        <TouchableOpacity style={{
          position: 'relative'
        }}>
          <Video
            source={{ uri: liveURL }}
            paused={paused}
            resizeMode="cover"
            repeat={true}
            onLoad={this.onLoad}
            onEnd={this.onEnd}
            style={{ width: width, height: height }}
            poster={thumbURL}
            posterResizeMode='cover'
            muted={true}
          />
          <Image
            source={require('../../../assets/live.png')}
            style={styles.playIcon}
            resizeMode="cover"
          />
        </TouchableOpacity>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  playIcon: {
    height: 28,
    width: 28,
    position: 'absolute',
    top: 10,
    right: 10
  },
  itemContainer: {
    flex: 1,
    flexDirection: 'column',
    overflow: 'hidden',
    backgroundColor: 'white',
  },
});

export default LiveCell;

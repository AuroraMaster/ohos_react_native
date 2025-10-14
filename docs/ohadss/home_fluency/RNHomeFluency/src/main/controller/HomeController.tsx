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

import { PureComponent, Component } from 'react';
import {
  View,
  Dimensions,
  ActivityIndicator,
  Text,
  DeviceEventEmitter,
  ScaledSize,
  Image,
  StyleSheet,
  RefreshControl,
  LogBox
} from 'react-native';
import React from 'react';
import VideoCell from '../view/VideoCell';
import LiveCell from '../view/LiveCell';
import ImageCell from '../view/ImageCell';
import SearchBar from '../view/SearchBar';
import SwiperView from '../view/SwiperView';
import BlankView from '../view/BlankView';
import { MasonryFlashList } from '@shopify/flash-list';
import { CardData, NetworkUtil } from '../utils/NetWorkUtil';
import CustomToast from '../view/Toast';
import getNumberOfLines from '../utils/GetNumberOfLine';
import { DeviceState, getDeviceRange, } from '../utils/DeviceRangeUtil';

const window = Dimensions.get('window');
const PAGE_SIZE = 20;

interface WaterfallFlowScreenState {
  data: CardData[];
  refreshing: boolean;
  loadingMore: boolean;
  hasMore: boolean;
  showNetErrorToast: boolean
}

interface CardProps {
  item: CardData;
  columnIndex: number;
}

class WaterfallFlowScreen extends Component<
  {},
  WaterfallFlowScreenState,
  DeviceState
> {
  private pageNum: number;
  private listRef: any;
  private tabPressListen: any;
  private changeListen: any;
  private cardList: CardData[] = [];

  constructor(props: {}) {
    super(props);

    this.state = {
      data: [],
      refreshing: false,
      loadingMore: false,
      deviceRange: getDeviceRange(window.width),
      showNetErrorToast: false
    };
    this.pageNum = 0;
    this.listRef = null;
  }

  componentDidMount() {
    // 忽略所有日志信息
    LogBox.ignoreAllLogs();
    this.tabPressListen = this.props.navigation.addListener('tabPress', () => {
      if (!this.listRef) {
        return;
      }
      this.listRef.scrollToOffset({ offset: 0, Animated: true })
    })
    this.fetchData(1);
    const handleResize = ({ window }: { window: ScaledSize }) => {
      const data = [...this.state.data];
      let windowWidth = window.width;
      this.setState({
        deviceRange: getDeviceRange(windowWidth),
      })
      let isSM = windowWidth >= 320 && windowWidth <= 600;
      let itemWidth = (Dimensions.get('window').width - (isSM ? 40 : 48)) / (isSM ? 2 : 3);
      for (let i = 0; i < data.length; i++) {
        let ratio = data[i].height / data[i].width;
        data[i].width = itemWidth;
        data[i].height = itemWidth * ratio;
      }
      this.setState({
        data: data,
      });
      if (this.listRef) {
        setTimeout(() => {
          this.listRef.scrollToOffset({ offset: 0, Animated: true })
        }, 400);
      }
    };
    this.changeListen = Dimensions.addEventListener('change', handleResize);
  }

  componentWillUnmount() {
    if (this.tabPressListen) {
      this.tabPressListen = null;
    }
    if (this.changeListen) {
      this.changeListen = null;
    }
  }
  checkNetError(isShow: boolean) {
    NetworkUtil.testNetwork(() => {
      if (isShow) {
        this.setState({
          showNetErrorToast: true
        })
        setTimeout(() => {
          this.setState({
            showNetErrorToast: false
          })
        }, 3000)
      }
    })
  }

  fetchData = async (pageNum: number, isRefresh: boolean = false) => {
    try {
      const newData: CardData[] = await NetworkUtil.getCarData("smoothSlide", "waterFlow.json", pageNum, PAGE_SIZE, isRefresh);
      let windowWidth = window.width;
      let isSM = windowWidth >= 320 && windowWidth <= 600;
      let itemWidth = (Dimensions.get('window').width - (isSM ? 40 : 48)) / (isSM ? 2 : 3);
      for (let i = 0; i < newData.length; i++) {
        let ratio = newData[i].height / newData[i].width;
        newData[i].width = itemWidth;
        newData[i].height = itemWidth * ratio;

        let tempCollectionsCount = parseInt(newData[i].collections_count);
        if (tempCollectionsCount > 1000) {
          newData[i].collections_count = `${Math.floor(tempCollectionsCount / 1000)}k+`
        }
      }
      this.pageNum = pageNum;
      this.setState(prevState => ({
        data: pageNum === 1 ? newData : [...prevState.data, ...newData],
        hasMore: newData.length > 0 && pageNum < 26,   // 临时处理没有更多数据
        refreshing: false,
        loadingMore: false,
      }));
    } catch (error) {
      console.error(error);
      this.setState({ refreshing: false, loadingMore: false });
      const newData = []
      if (this.state.data.length === 0) {
        this.setState(prevState => ({
          data: newData,
          hasMore: false,
        }));
      }
    }
  };

  onRefresh = () => {
    this.setState({ refreshing: true });
    this.checkNetError(true)
    this.fetchData(1, true);
  };

  onLoadMore = () => {
    if (this.state.loadingMore || !this.state.hasMore) {
      return;
    }
    this.setState({ loadingMore: true });
    this.checkNetError(true)
    this.fetchData(this.pageNum + 1);
  };

  overrideItemLayout = (layout: { span?: number; size?: number }, item: CardData,) => {
    if (!item) {
      return;
    }
    let titleHeight = 19 * (getNumberOfLines(item.title, 14, item.width - 24) > 1 ? 2 : 1)
    layout.size = item.height + titleHeight + 46;
  }


  onViewableItemsChanged = ({ viewableItems }: { viewableItems: Array<{ item: CardData; isViewable: boolean }> }) => {
    viewableItems.forEach(({ item, isViewable }) => {
      if (isViewable) {
        if (this.cardList.length >= 3) {
          this.cardList.shift();
        }
        this.cardList.push(item);
        if (this.cardList.length >= 3) {
          DeviceEventEmitter.emit('onViewableItemsChanged', this.cardList)
        }
      }
    });
  }

  render() {
    const { data, refreshing, deviceRange, showNetErrorToast } = this.state;
    return (
      <View style={{ flex: 1 }}>
        <View style={{ flexDirection: 'column', flex: 1 }}>
          <SearchBar />
          <MasonryFlashList
            ref={(ref) => (this.listRef = ref)}
            keyExtractor={(item, index) => index.toString()}
            estimatedItemSize={200}
            disableAutoLayout={false}
            ListHeaderComponent={
              data.length === 0 ? <BlankView /> : <SwiperView />
            }
            onEndReachedThreshold={0.5}
            ListFooterComponent={this.ListFooterComponent(data)}
            data={data}
            numColumns={deviceRange === 'SM' ? 2 : 3}
            refreshControl={<RefreshControl refreshing={refreshing} onRefresh={this.onRefresh} />}
            onEndReached={this.onLoadMore}
            optimizeItemArrangement={true}
            overrideItemLayout={this.overrideItemLayout}
            getItemType={(item: CardData) => {
              return item ? item.type : 'image';
            }}
            renderItem={({ item, index, columnIndex }) => {
              return (
                <Card item={item} columnIndex={columnIndex} />
              );
            }}
            onViewableItemsChanged={this.onViewableItemsChanged}
            viewabilityConfig={{
              minimumViewTime: 800,
              itemVisiblePercentThreshold: 100,
            }}
            showsVerticalScrollIndicator={false}
            contentContainerStyle={{ paddingLeft: 16, paddingRight: 16 }}
          />
        </View>
        <CustomToast message={'无法连接网络，请检查网络设置'} visible={showNetErrorToast} />
      </View>
    );
  }

  ListFooterComponent = (data) => {
    const { hasMore, loadingMore } = this.state
    return (
      <View style={{ flexDirection: 'row', alignItems: 'center', justifyContent: 'center', height: 60 }}>
        {loadingMore && <ActivityIndicator color='gray' />}
        {data.length !== 0 && (
          <Text style={{ color: 'black', marginLeft: 8, opacity: 0.4 }}>
            {hasMore
              ? loadingMore
                ? '——加载中，请稍后——'
                : ''
              : '——已到达底部——'}
          </Text>
        )}
      </View>
    );
  }
}

class Card extends PureComponent<CardProps, DeviceState> {
  private changeListen: any;
  constructor(props: {}) {
    super(props);
    this.state = {
      deviceRange: getDeviceRange(window.width),
    };
  }

  componentDidMount() {
    const handleResize = ({ window }: { window: ScaledSize }) => {
      this.setState({ deviceRange: getDeviceRange(window.width) });
    };
    this.changeListen = Dimensions.addEventListener('change', handleResize);
  }

  componentWillUnmount() {
    if (this.changeListen) {
      this.changeListen = null;
    }
  }

  marginLeft(columnIndex: number): number {
    if (columnIndex == 0) {
      return 0;
    }
    let windowWidth = window.width;
    let isSM = windowWidth >= 320 && windowWidth <= 600;
    if (!isSM && columnIndex == 1) {
      return 2;
    }
    return 4;
  }

  render() {
    const { item, columnIndex } = this.props;
    return (
      <View
        style={{
          flex: 1,
          overflow: 'hidden',
          marginLeft: this.marginLeft(columnIndex),
          marginBottom: 8,
          borderRadius: 16,
          backgroundColor: 'white',
          width: item.width,
        }}>
        {item && item.type === 'image' && (
          <ImageCell
            thumbnails={item.thumbnails}
            width={item.width}
            height={item.height}
          />
        )}
        {item && item.type === 'video' && (
          <VideoCell
            videoURL={item.source}
            thumbURL={item.thumbnails}
            width={item.width}
            height={item.height}
          />
        )}
        {item && item.type === 'living' && (
          <LiveCell
            liveURL={item.source}
            thumbURL={item.thumbnails}
            width={item.width}
            height={item.height}
            index={item.index}
          />
        )}
        {item && (
          <View style={{ flex: 1, flexDirection: 'column', width: item.width }}>
            <Text style={[{ width: item.width }, styles.titleText]} numberOfLines={2} ellipsizeMode='clip' >{item.title}</Text>
            <View style={styles.userContainer}>
              <View
                style={{
                  position: 'relative',
                }}>
                <Image
                  source={{ uri: item.user_image }}
                  style={styles.userImage}
                  resizeMode="cover"
                />
                {item.vip_sign === '1' && <Image
                  source={require('../../../assets/vip.png')}
                  style={styles.vipIcon}
                  resizeMode="cover"
                />}
              </View>
              <Text style={styles.nickName}>{item.nick_name}</Text>
              <View style={{ flex: 1 }} />
              <Image
                source={require('../../../assets/icon_love.png')}
                style={styles.loveIcon}
                resizeMode='cover'
              />
              <Text style={styles.loveText}>{item.collections_count}</Text>
            </View>
          </View>
        )}
        {item === null && (
          <ImageCell
            source={{
              uri: '',
              width: 246,
              height: 258,
            }}
            resizeMode='cover' title={''} nick_name={''} collections_count={0} user_image={''} />
        )}
      </View>
    );
  }
}

const styles = StyleSheet.create({
  titleText: {
    fontSize: 14,
    lineHeight: 19,
    paddingLeft: 12,
    paddingRight: 12,
    marginTop: 12,
    marginBottom: 9,
  },
  nickName: {
    fontSize: 10,
    lineHeight: 13,
    marginLeft: 4,
    opacity: 0.6
  },
  loveText: {
    fontSize: 12,
    lineHeight: 16,
    marginLeft: 4,
    opacity: 0.6,
  },
  userContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    flex: 1,
    width: '100%',
    paddingHorizontal: 12,
    marginBottom: 11,
  },
  userImage: {
    width: 16,
    height: 16,
    borderRadius: 8,
  },
  loveIcon: {
    width: 10.8,
    height: 9,
  },
  vipIcon: {
    height: 8,
    width: 8,
    position: 'absolute',
    bottom: 0,
    right: 0
  },
});

export default WaterfallFlowScreen;

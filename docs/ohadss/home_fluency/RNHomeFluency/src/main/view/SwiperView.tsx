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

import React, { useEffect, useState, useMemo, useRef } from 'react';
import { View, Image, Text, StyleSheet, Animated } from 'react-native';
import Swiper from 'react-native-swiper';
import { Item, NetworkUtil } from '../utils/NetWorkUtil';

interface BookmarkBarProps {
  barKey: number;
  data: Item[];
}

const BookmarkBar: React.FC<BookmarkBarProps> = ({ barKey, data }) => {
  const chunkedData = useMemo(() => {
    const chunks: Item[][] = [];
    for (let i = 0; i < data.length; i += 5) {
      chunks.push(data.slice(i, i + 5));
    }
    return chunks;
  }, [data]);

  return (
    <View
      style={{
        paddingVertical: 5,
        height: barKey === 0 ? 170 : 240,
      }}>
      {chunkedData.map((row, rowIndex) => (
        <View key={rowIndex} style={styles.rowContainer}>
          {row.map(item => (
            <View key={item.index} style={styles.itemContainer}>
              <Image source={{ uri: item.icon }} style={styles.itemImage} />
              <Text style={styles.itemText}>{item.name}</Text>
            </View>
          ))}
        </View>
      ))}
    </View>
  );
};

const SwiperView: React.FC = React.memo(() => {
  const [showData, setShowData] = useState<Item[][]>([[], [], []]);
  const scrollX = useRef(new Animated.Value(0)).current;

  const fetchData = async () => {
    const data: Item[] = await NetworkUtil.getFunctionData("smoothSlide", "entry.json", 1, 25);
    let tmpData: Item[][] = [];
    tmpData.push(data.slice(0, 10));
    tmpData.push(data.slice(10, 25));
    setShowData(tmpData);
  };

  useEffect(() => {
    fetchData();
  }, [fetchData]);

  return (
    <View style={styles.viewContainer}>
      <Animated.View
        style={{
          height: scrollX.interpolate({
            inputRange: [
              0,
              300
            ],
            outputRange: [200, 280],
            extrapolate: "clamp"
          })
        }}
      >
        <Swiper
          showsPagination={true}
          paginationStyle={styles.paginationStyle}
          loop={false}
          autoplay={false}
          dotStyle={styles.dot}
          onScroll={Animated.event([
            {
              nativeEvent: {
                contentOffset: {
                  x: scrollX
                }
              }
            }
          ])}
          activeDotStyle={styles.activeDot}
          pagingEnabled={true}>
          {showData.map((item, index) => (
            <BookmarkBar barKey={index} data={item} />
          ))}
        </Swiper>
      </Animated.View>
    </View>
  );
});

const styles = StyleSheet.create({
  viewContainer: {
    justifyContent: 'center',
    alignItems: 'center',
  },
  loadingIndicator: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  pagination: {
    flexDirection: 'row',
    justifyContent: 'space-around',
    alignItems: 'center',
    position: 'absolute',
    bottom: 10,
    left: 0,
    right: 0,
  },
  container: {
    paddingVertical: 5,
    height: 140,
  },
  paginationStyle: {
    bottom: 6,
    height: 32,
  },
  dot: {
    backgroundColor: 'rgba(0,0,0,.2)',
    width: 8,
    height: 8,
    borderRadius: 4,
    margin: 3,
  },
  activeDot: {
    backgroundColor: 'rgb(10, 89, 247)',
    width: 12,
    height: 8,
    borderRadius: 4,
    margin: 3,
  },
  rowContainer: {
    flex: 1,
    flexDirection: 'row',
    justifyContent: 'space-around',
    alignItems: 'flex-start',
  },
  itemContainer: {
    alignItems: 'center',
    width: '20%',

  },
  fold: {
    width: '16.6%',
  },
  pad: {
    width: '10%',
  },
  itemImage: {
    width: 40,
    height: 40,
    borderRadius: 5,
  },
  itemText: {
    marginTop: 10,
    fontSize: 12,
  },
});

export default SwiperView;

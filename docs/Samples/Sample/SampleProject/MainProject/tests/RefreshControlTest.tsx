/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  FlatList,
  Platform,
  RefreshControl,
  ScrollView,
  StyleSheet,
  Text,
  View,
} from 'react-native';
import { TestCase, TestSuite } from '@rnoh/testerino';
import { useEffect, useState } from 'react';

export const RefreshControlTest = () => {
  const [refreshKey, setRefreshKey] = useState(0);

  useEffect(() => {
    setInterval(() => setRefreshKey(prev => prev + 1), 1000);
  }, []);

  return (
    <TestSuite name="RefreshControl">
      <TestCase itShould="display refresh control every second">
        <ScrollView
          style={{ height: 128, backgroundColor: 'white' }}
          refreshControl={
            <RefreshControl
              refreshing={refreshKey % 2 === 0}
              onRefresh={() => { }}
            />
          }
        />
      </TestCase>
      <TestCase itShould="display refresh control with tintColor">
        <ScrollView
          style={{ height: 128, backgroundColor: 'white' }}
          refreshControl={
            <RefreshControl
              refreshing={refreshKey % 2 === 0}
              tintColor={'#FFC0CB'}
              onRefresh={() => { }}
            />
          }
        />
      </TestCase>
      <TestCase
        modal
        itShould="be refreshing for one second after pull to refresh">
        <PullToRefreshExample />
      </TestCase>
      <TestCase
        modal
        itShould="immediately stop refreshing after pulling to refresh">
        <PullToRefreshExample doNothingOnRefresh />
      </TestCase>
      <TestCase
        modal
        itShould="refresh with progressViewOffset = undefined"
        skip // there is a restriction on how tall the progress view can be - should be removed in API 11 https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/624
      >
        <PullToRefreshProgressViewOffset />
      </TestCase>
      <TestCase
        modal
        itShould="refresh with progressViewOffset = 50"
        skip // there is a restriction on how tall the progress view can be - should be removed in API 11 https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/624
      >
        <PullToRefreshProgressViewOffset progressViewOffset={50} />
      </TestCase>
      <TestCase
        modal
        itShould="refresh with progressViewOffset = 100"
        skip // there is a restriction on how tall the progress view can be - should be removed in API 11 https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/624
      >
        <PullToRefreshProgressViewOffset progressViewOffset={100} />
      </TestCase>
      <TestCase
        itShould="display double refresh control for 2 seconds in nested scroll view"
        modal
        skip={Platform.select({ harmony: true, android: false })} // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/770
      >
        <PullToRefreshInNestedScrollViews />
      </TestCase>
      <TestCase
        itShould="display one refresh control - for 3 seconds in nested scroll view"
        modal>
        <PullToRefreshInNestedScrollViewsDifferentSource />
      </TestCase>
    </TestSuite>
  );
};

function PullToRefreshExample({
  doNothingOnRefresh,
}: {
  doNothingOnRefresh?: boolean;
}) {
  const [isRefreshing, setIsRefreshing] = useState(false);

  return (
    <FlatList
      style={{ height: 256 }}
      refreshing={isRefreshing}
      onRefresh={() => {
        if (!doNothingOnRefresh) {
          setIsRefreshing(true);
          setTimeout(() => setIsRefreshing(false), 1000);
        }
      }}
      data={[1, 2, 3, 4, 5]}
      renderItem={({ item }) => (
        <Text style={{ height: 96, borderBottomWidth: 1 }}>{item}</Text>
      )}
    />
  );
}

function PullToRefreshProgressViewOffset({
  progressViewOffset,
}: {
  progressViewOffset?: number;
}) {
  const [refreshing, setIsRefreshing] = useState(false);

  return (
    <ScrollView
      style={{ height: '90%', backgroundColor: 'lightgray' }}
      refreshControl={
        <RefreshControl
          refreshing={refreshing}
          onRefresh={() => {
            setIsRefreshing(true);
            setTimeout(() => setIsRefreshing(false), 3000);
          }}
          progressViewOffset={progressViewOffset}
        />
      }>
      <View style={{ height: 50, backgroundColor: 'lightblue' }}>
        <Text>First Content Component</Text>
      </View>
      <View style={{ height: 50, backgroundColor: 'lightgreen' }}>
        <Text>Second Content Component</Text>
      </View>
      <View style={{ height: 50, backgroundColor: 'lightblue' }}>
        <Text>Third Content Component</Text>
      </View>
    </ScrollView>
  );
}

const wait = (timeout: number) => {
  return new Promise(resolve => {
    setTimeout(resolve, timeout);
  });
};

const PullToRefreshInNestedScrollViews = () => {
  const [refreshing, setRefrehing] = useState(false);

  const onRefresh = () => {
    setRefrehing(true);
    wait(2000).then(() => {
      setRefrehing(false);
    });
  };

  return (
    <ScrollView
      contentContainerStyle={styles.scrollView}
      refreshControl={
        <RefreshControl refreshing={refreshing} onRefresh={onRefresh} />
      }>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <ScrollView
        contentContainerStyle={{
          ...styles.scrollView,
          backgroundColor: 'white',
        }}
        refreshControl={
          <RefreshControl refreshing={refreshing} onRefresh={onRefresh} />
        }>
        <Text style={styles.text}>
          inner-scrollView Press and hold down the area to refresh. The refresh
          label does not bounce back
        </Text>
        <Text style={styles.text}>
          inner-scrollView Press and hold down the area to refresh. The refresh
          label does not bounce back
        </Text>
        <Text style={styles.text}>
          inner-scrollView Press and hold down the area to refresh. The refresh
          label does not bounce back
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView Press and hold down the area to refresh. The refresh
          label does not bounce back
        </Text>
      </ScrollView>
    </ScrollView>
  );
};

const PullToRefreshInNestedScrollViewsDifferentSource = () => {
  const [firstRefreshing, setFirstRefrehing] = useState(false);
  const [secondRefreshing, setSecondRefrehing] = useState(false);

  const onFirstsRefresh = () => {
    setFirstRefrehing(true);
    wait(3000).then(() => {
      setFirstRefrehing(false);
    });
  };

  const onSecondRefresh = () => {
    setSecondRefrehing(true);
    wait(4000).then(() => {
      setSecondRefrehing(false);
    });
  };

  return (
    <ScrollView
      contentContainerStyle={styles.scrollView}
      refreshControl={
        <RefreshControl
          refreshing={firstRefreshing}
          onRefresh={onFirstsRefresh}
        />
      }>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <Text>outer-scrollView</Text>
      <ScrollView
        contentContainerStyle={{
          ...styles.scrollView,
          backgroundColor: 'white',
        }}
        refreshControl={
          <RefreshControl
            refreshing={secondRefreshing}
            onRefresh={onSecondRefresh}
          />
        }>
        <Text style={styles.text}>
          inner-scrollView Press and hold down the area to refresh. The refresh
          label does not bounce back
        </Text>
        <Text style={styles.text}>
          inner-scrollView Press and hold down the area to refresh. The refresh
          label does not bounce back
        </Text>
        <Text style={styles.text}>
          inner-scrollView Press and hold down the area to refresh. The refresh
          label does not bounce back
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView 按住该区域下拉刷新，刷新标识不回弹回
        </Text>
        <Text style={styles.text}>
          inner-scrollView Press and hold down the area to refresh. The refresh
          label does not bounce back
        </Text>
      </ScrollView>
    </ScrollView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  scrollView: {
    flex: 1,
    backgroundColor: 'pink',
    alignItems: 'center',
    justifyContent: 'center',
  },
  text: {
    backgroundColor: 'green',
  },
});

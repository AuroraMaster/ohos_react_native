/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, { useEffect, useMemo, useRef, useState } from 'react';
import {
  View,
  SectionList,
  StyleSheet,
  Text,
  SectionListProps,
  RefreshControl,
  Platform,
  ViewabilityConfig,
} from 'react-native';
import { TestCase, TestSuite } from '@rnoh/testerino';
import { Button, Modal, ObjectDisplayer } from '../components';

interface SectionData {
  id: string;
  title: string;
  data: string[];
}

const DATA: SectionData[] = [
  {
    id: '0',
    title: 'Main dishes',
    data: ['Pizza', 'Burger', 'Risotto'],
  },
  {
    id: '1',
    title: 'Sides',
    data: ['French Fries', 'Onion Rings', 'Fried Shrimps'],
  },
  {
    id: '2',
    title: 'Drinks',
    data: ['Water', 'Coke', 'Beer'],
  },
  {
    id: '3',
    title: 'Desserts',
    data: ['Cheese Cake', 'Ice Cream'],
  },
];

const commonProps = {
  style: { width: 256 },
  sections: DATA,
  keyExtractor: (item, _index) => item.id,
  renderSectionHeader: ({ section }) => (
    <Text style={styles.title}>{section.title}</Text>
  ),
  renderItem: ({ item }) => (
    <View style={styles.item}>
      <Text style={styles.title}>{item}</Text>
    </View>
  ),
} satisfies SectionListProps<any>;

export const SectionListTest = () => {
  return (
    <TestSuite name="SectionList">
      <TestCase itShould="display items in the SectionList">
        <Modal>
          <View style={styles.container}>
            <SectionList
              sections={DATA}
              keyExtractor={(item, index) => item + index}
              renderItem={({ item }) => (
                <View style={styles.item}>
                  <Text style={styles.title}>{item}</Text>
                </View>
              )}
              renderSectionHeader={({ section: { title } }) => (
                <Text style={styles.title}>{title}</Text>
              )}
            />
          </View>
        </Modal>
      </TestCase>
      <TestCase itShould="display an array of visible items">
        <Modal>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  viewabilityConfig={{ viewAreaCoveragePercentThreshold: 100 }}
                  onViewableItemsChanged={item => {
                    setObject(item.viewableItems.map(i => i.item));
                  }}
                />
              );
            }}
          />
        </Modal>
      </TestCase>
      <TestCase itShould="render no more than 2 new items per batch when scrolling down">
        <Modal>
          <SectionList
            {...commonProps}
            windowSize={2}
            renderItem={({ item }) => {
              return (
                <DelayedDisplayer
                  delayInMs={1000}
                  renderContent={() => {
                    return (
                      <View style={styles.item}>
                        <Text style={styles.title}>{item}</Text>
                      </View>
                    );
                  }}
                />
              );
            }}
          />
        </Modal>
      </TestCase>
      <TestCase itShould="display nativeEvent when onMomentumScrollBegin">
        <Modal>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  onMomentumScrollBegin={e => {
                    setObject({ nativeEvent: e.nativeEvent });
                  }}
                />
              );
            }}
          />
        </Modal>
      </TestCase>
      <TestCase itShould="display nativeEvent when onMomentumScrollEnd">
        <Modal>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  onMomentumScrollEnd={e => {
                    setObject({ nativeEvent: e.nativeEvent });
                  }}
                />
              );
            }}
          />
        </Modal>
      </TestCase>
      <TestCase itShould="display event sent to by onScrollToIndexFailed when pressing the button before scrolling">
        <Modal>
          <ScrollToIndexFailureTestCase />
        </Modal>
      </TestCase>
      {/* sticky headers seems to work on Android when App.tsx was replaced with content of this test */}
      <TestCase
        itShould="stick section headers (fails on Android when fabric is enabled)"
        skip={Platform.OS === 'android'}>
        <Modal>
          <SectionList {...commonProps} stickySectionHeadersEnabled />
        </Modal>
      </TestCase>
      <TestCase itShould="support viewOffset when scrolling to location">
        <Modal>
          <ScrollToLocationOffset />
        </Modal>
      </TestCase>
      <TestCase itShould="show vertical scroll indicator">
        <Modal>
          <SectionList {...commonProps} showsVerticalScrollIndicator={true} />
        </Modal>
      </TestCase>
      <TestCase itShould="hide vertical scroll indicator">
        <Modal>
          <SectionList {...commonProps} showsVerticalScrollIndicator={false} />
        </Modal>
      </TestCase>
      <TestCase itShould="show horizontal scroll indicator">
        <Modal>
          <View style={{ width: 200, height: '100%' }}>
            <SectionList
              {...commonProps}
              showsHorizontalScrollIndicator={true}
              horizontal
            />
          </View>
        </Modal>
      </TestCase>
      <TestCase itShould="hide horizontal scroll indicator">
        <Modal>
          <View style={{ width: 200, height: '100%' }}>
            <SectionList
              {...commonProps}
              showsHorizontalScrollIndicator={false}
              horizontal
            />
          </View>
        </Modal>
      </TestCase>
      <TestCase itShould="[SKIP] display overscroll effect">
        <Modal>
          {/* On Android this settings enables stretching the ScrollView content. On OpenHarmony `bounces` prop can be used instead. */}
          <SectionList {...commonProps} overScrollMode="always" />
        </Modal>
      </TestCase>
      <TestCase itShould="render custom RefreshControl on pull to refresh">
        <Modal>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  refreshControl={
                    // only RefreshControl can be provided
                    <RefreshControl
                      onRefresh={() => {
                        setObject({ onRefreshCalled: true });
                      }}
                      refreshing={false}
                      tintColor={'red'} // iOS. It's unknown how to set the color of the refreshing widget in ArkUI.
                      colors={['red', 'green']} // Android
                    />
                  }
                />
              );
            }}
          />
        </Modal>
      </TestCase>
      <TestCase itShould="render standard RefreshControl on pull to refresh">
        <Modal>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  onRefresh={() => {
                    setObject({ onRefreshCalled: true });
                  }}
                  refreshing={false}
                />
              );
            }}
          />
        </Modal>
      </TestCase>
      <TestCase
        itShould="display onScroll native event throttled every second"
        skip={Platform.select({
          android: 'RN bug',
          harmony: "doesn't work on Android",
        })}>
        {/* https://github.com/facebook/react-native/issues/18441 */}
        <Modal>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  scrollEventThrottle={1000}
                  onScroll={e => {
                    setObject(e.nativeEvent);
                  }}
                />
              );
            }}
          />
        </Modal>
      </TestCase>
      <TestCase itShould="allow scrolling beneath the content due to large lengths returned in getItemLayout">
        <Modal>
          <SectionList
            {...commonProps}
            getItemLayout={(data, index) => {
              const ITEM_HEIGHT = 1000;
              return {
                length: ITEM_HEIGHT,
                offset: ITEM_HEIGHT * index,
                index,
              };
            }}
          />
        </Modal>
      </TestCase>
      <TestCase itShould="display onEndReached event when scroll reached bottom">
        <Modal>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  onEndReached={e => {
                    setObject(e);
                  }}
                />
              );
            }}
          />
        </Modal>
      </TestCase>
      <TestCase itShould="click on 'Record interaction' button changes the first three items background color to blue">
        <Modal contentContainerStyle={{ width: '85%' }}>
          <SectionListRecordInteractionTest />
        </Modal>
      </TestCase>
    </TestSuite>
  );
};

function ScrollToIndexFailureTestCase() {
  const ref = useRef<SectionList>(null);

  return (
    <>
      <Button
        label="Scroll to index"
        onPress={() => {
          if (ref.current) {
            ref.current.scrollToLocation({
              sectionIndex: 1,
              itemIndex: 10,
              animated: true,
            });
          }
        }}
      />
      <ObjectDisplayer
        renderContent={setObject => {
          return (
            <SectionList
              ref={ref}
              {...commonProps}
              windowSize={1}
              onScrollToIndexFailed={info => {
                setObject(info);
              }}
            />
          );
        }}
      />
    </>
  );
}

function ScrollToLocationOffset() {
  const ref = useRef<SectionList>(null);

  return (
    <>
      <Button
        label="Scroll to onion rings"
        onPress={() => {
          ref.current?.scrollToLocation({
            itemIndex: 1,
            sectionIndex: 1,
            viewOffset: -100,
          });
        }}
      />
      <SectionList ref={ref} {...commonProps} />
    </>
  );
}

function DelayedDisplayer(props: {
  renderContent: () => any;
  delayInMs: number;
}) {
  const [isVisible, setIsVisible] = useState(false);

  useEffect(() => {
    const timeout = setTimeout(() => {
      setIsVisible(true);
    }, props.delayInMs);
    return () => {
      clearTimeout(timeout);
    };
  }, []);

  return <>{isVisible ? props.renderContent() : null}</>;
}

export interface ViewTokenItem<TItem> {
  item: TItem;
  key: string;
  index: number | null;
  isViewable: boolean;
  section?: any | undefined;
}

type OnViewableItemsChangedType<TItem> = {
  viewableItems: Array<ViewTokenItem<TItem>>;
  changed: Array<ViewTokenItem<TItem>>;
};

function SectionListRecordInteractionTest() {
  const [visibleItems, setVisibleItems] = useState<string[]>([]);

  const ref = useRef<SectionList>(null);

  const defaultViewabilityConfig: ViewabilityConfig = useMemo(() => {
    return {
      waitForInteraction: true,
      minimumViewTime: 100,
      itemVisiblePercentThreshold: 70,
    };
  }, []);

  const handleOnPress = () => {
    if (ref.current) {
      ref.current.recordInteraction();
    }
  };

  const onViewableItemsChanged = ({
    viewableItems,
  }: OnViewableItemsChangedType<string>) => {
    const newVisibleItems = viewableItems
      .filter(viewableItem => viewableItem.index != null)
      .map(viewableItem => viewableItem.item);

    setVisibleItems(newVisibleItems);
  };

  return (
    <>
      <View style={{ marginBottom: 10 }}>
        <Button label="Record interaction" onPress={handleOnPress} />
        <Text style={{ padding: 10 }}>
          Visible Items are: {JSON.stringify(visibleItems)}
        </Text>
      </View>
      <SectionList
        ref={ref}
        sections={DATA}
        scrollEnabled={false}
        keyExtractor={(_, index) => String(index)}
        renderSectionHeader={({ section }) => (
          <Text style={styles.title}>{section.title}</Text>
        )}
        renderItem={({ item }) => (
          <View
            style={[
              {
                height: 200,
                backgroundColor: 'lightblue',
                marginBottom: 10,
              },
              visibleItems.includes(item) && { backgroundColor: 'blue' },
            ]}>
            <Text
              style={{
                fontSize: 32,
                height: 200,
                textAlign: 'center',
                textAlignVertical: 'center',
              }}>
              {item}
            </Text>
          </View>
        )}
        viewabilityConfig={defaultViewabilityConfig}
        onViewableItemsChanged={onViewableItemsChanged}
      />
    </>
  );
}

const styles = StyleSheet.create({
  container: {
    height: 120,
  },
  item: {
    backgroundColor: '#f9c2ff',
    padding: 20,
    marginVertical: 8,
    marginHorizontal: 16,
  },
  title: {
    fontSize: 32,
    height: 40,
  },
});

export default SectionListTest;

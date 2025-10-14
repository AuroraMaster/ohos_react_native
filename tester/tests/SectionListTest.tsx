/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useEffect, useMemo, useRef, useState} from 'react';
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
import {TestSuite} from '@rnoh/testerino';
import {Button, ObjectDisplayer, TestCase} from '../components';

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
  style: {width: 256},
  sections: DATA,
  keyExtractor: (item, index) => `${item.id}-${index}`,
  renderSectionHeader: ({section}) => (
    <Text style={styles.title}>{section.title}</Text>
  ),
  renderItem: ({item}) => (
    <View style={styles.item}>
      <Text style={styles.title}>{item}</Text>
    </View>
  ),
} satisfies SectionListProps<any>;

export const SectionListTest = () => {
  return (
    <TestSuite name="SectionList">
      <TestCase.Example modal itShould="display items in the SectionList">
        <View style={styles.container}>
          <SectionList
            sections={DATA}
            keyExtractor={(item, index) => `${item}-${index}`}
            renderItem={({item}) => (
              <View style={styles.item}>
                <Text style={styles.title}>{item}</Text>
              </View>
            )}
            renderSectionHeader={({section: {title}}) => (
              <Text style={styles.title}>{title}</Text>
            )}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example modal itShould="display an array of visible items">
        <View style={styles.container}>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  viewabilityConfig={{viewAreaCoveragePercentThreshold: 100}}
                  onViewableItemsChanged={item => {
                    setObject(item.viewableItems.map(i => i.item));
                  }}
                />
              );
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="render no more than 2 new items per batch when scrolling down">
        <View style={{height: 200}}>
          <SectionList
            {...commonProps}
            windowSize={2}
            renderItem={({item}) => {
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
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display nativeEvent when onMomentumScrollBegin">
        <View style={styles.container}>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  onMomentumScrollBegin={e => {
                    setObject({nativeEvent: e.nativeEvent});
                  }}
                />
              );
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display nativeEvent when onMomentumScrollEnd">
        <View style={styles.container}>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  onMomentumScrollEnd={e => {
                    setObject({nativeEvent: e.nativeEvent});
                  }}
                />
              );
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display event sent to by onScrollToIndexFailed when pressing the button before scrolling">
        <ScrollToIndexFailureTestCase />
      </TestCase.Example>
      {/* sticky headers seems to work on Android when App.tsx was replaced with content of this test */}
      <TestCase.Example
        modal
        itShould="stick section headers (fails on Android when fabric is enabled)"
        skip={{android: true, harmony: false}}>
        <View style={styles.container}>
          <SectionList {...commonProps} stickySectionHeadersEnabled />
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="support viewOffset when scrolling to location">
        <View style={styles.container}>
          <ScrollToLocationOffset />
        </View>
      </TestCase.Example>
      <TestCase.Example modal itShould="show vertical scroll indicator">
        <View style={styles.container}>
          <SectionList {...commonProps} showsVerticalScrollIndicator={true} />
        </View>
      </TestCase.Example>
      <TestCase.Example modal itShould="hide vertical scroll indicator">
        <View style={styles.container}>
          <SectionList {...commonProps} showsVerticalScrollIndicator={false} />
        </View>
      </TestCase.Example>
      <TestCase.Example modal itShould="show horizontal scroll indicator">
        <View style={styles.container}>
          <View style={{width: 200, height: '100%'}}>
            <SectionList
              {...commonProps}
              showsHorizontalScrollIndicator={true}
              horizontal
            />
          </View>
        </View>
      </TestCase.Example>
      <TestCase.Example modal itShould="hide horizontal scroll indicator">
        <View style={styles.container}>
          <View style={{width: 200, height: '100%'}}>
            <SectionList
              {...commonProps}
              showsHorizontalScrollIndicator={false}
              horizontal
            />
          </View>
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display overscroll effect"
        skip={{android: false, harmony: true}}>
        <View style={styles.container}>
          {/* On Android this settings enables stretching the ScrollView content. On Harmony `bounces` prop can be used instead. */}
          <SectionList {...commonProps} overScrollMode="always" />
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="render custom RefreshControl on pull to refresh">
        <View style={styles.container}>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  refreshControl={
                    // only RefreshControl can be provided
                    <RefreshControl
                      onRefresh={() => {
                        setObject({onRefreshCalled: true});
                      }}
                      refreshing={false}
                      tintColor={'red'} // iOS.
                      colors={['red', 'green']} // Android
                    />
                  }
                />
              );
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="render standard RefreshControl on pull to refresh">
        <View style={styles.container}>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  onRefresh={() => {
                    setObject({onRefreshCalled: true});
                  }}
                  refreshing={false}
                />
              );
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display onScroll native event throttled every second"
        skip={Platform.select({
          android: 'RN bug', // https://github.com/facebook/react-native/issues/18441
        })}>
        <View style={styles.container}>
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
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="allow scrolling beneath the content due to large lengths returned in getItemLayout">
        <View style={styles.container}>
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
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display onEndReached event when scroll reached bottom and onStartReached event when scroll reached top">
        <View style={styles.container}>
          <ObjectDisplayer
            renderContent={setObject => {
              return (
                <SectionList
                  {...commonProps}
                  onEndReached={e => {
                    setObject(e);
                  }}
                  onStartReached={e => {
                    setObject(e);
                  }}
                />
              );
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="click on 'Record interaction' button changes the first three items background color to blue">
        <View style={{height: 500}}>
          <SectionListRecordInteractionTest />
        </View>
      </TestCase.Example>
    </TestSuite>
  );
};

function ScrollToIndexFailureTestCase() {
  const ref = useRef<SectionList>(null);

  return (
    <View style={{height: 500}}>
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
      <View style={{flex: 1}}>
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
      </View>
    </View>
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
      <View style={{marginBottom: 10}}>
        <Button label="Record interaction" onPress={handleOnPress} />
        <Text style={{padding: 10}}>
          Visible Items are: {JSON.stringify(visibleItems)}
        </Text>
      </View>
      <SectionList
        ref={ref}
        sections={DATA}
        scrollEnabled={false}
        keyExtractor={(_, index) => String(index)}
        renderSectionHeader={({section}) => (
          <Text style={styles.title}>{section.title}</Text>
        )}
        renderItem={({item}) => (
          <View
            style={[
              {
                height: 200,
                backgroundColor: 'lightblue',
                marginBottom: 10,
              },
              visibleItems.includes(item) && {backgroundColor: 'blue'},
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
    height: 300,
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

/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useRef, useState} from 'react';
import {
  RefreshControl,
  Text,
  TouchableOpacity,
  View,
  ViewabilityConfig,
  VirtualizedList,
} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../components';

type OnScrollToIndexFailed = {
  index: number;
  highestMeasuredFrameIndex: number;
  averageItemLength: number;
};

type ItemData = {
  id: string;
  title: string;
};

const getItem = (_data: unknown, index: number): ItemData => ({
  id: index.toString(),
  title: `Item ${index + 1}`,
});

const getItemCountVirtualized = (_data: unknown): number => 50;

const Item = ({title}: {title: string}) => (
  <View style={{height: 48, padding: 16, borderWidth: 1}}>
    <Text style={{width: '100%', height: 24}}>{title}</Text>
  </View>
);

const HorizontalItem = ({title}: {title: string}) => (
  <View style={{borderWidth: 1, width: 48, height: '100%'}}>
    <Text style={{width: '100%', height: '100%'}}>{title}</Text>
  </View>
);

export function VirtualizedListTest() {
  return (
    <TestSuite name="VirtualizedList">
      <TestCase.Example itShould="display list of 3 items">
        <VirtualizedList<number[]>
          style={{height: 64}}
          data={[1, 2, 3]}
          getItem={(data, idx) => data[idx]}
          getItemCount={() => 3}
          renderItem={({item}) => (
            <View style={{height: 48, padding: 16}}>
              <Text style={{width: '100%', height: 24}}>{item}</Text>
            </View>
          )}
          keyExtractor={(_, index) => String(index)}
        />
      </TestCase.Example>
      <TestCase.Manual
        modal
        itShould="trigger onStartReached event when start of the content is within half the visible length of the list"
        initialState={-1}
        arrange={({setState}) => {
          const data = [1, 2, 3, 4, 5];
          return (
            <VirtualizedList
              data={data}
              getItem={(items: number[], idx: number) => items[idx]}
              getItemCount={() => data.length}
              renderItem={({item}: {item: number}) => (
                <View style={{height: 100, padding: 16, borderWidth: 1}}>
                  <Text style={{width: '100%', height: 24}}>{item}</Text>
                </View>
              )}
              style={{height: 200}}
              onStartReachedThreshold={0.5}
              onStartReached={({
                distanceFromStart,
              }: {
                distanceFromStart: number;
              }) => {
                setState(distanceFromStart);
              }}
            />
          );
        }}
        assert={({state, expect}) => {
          expect(state).to.be.lessThanOrEqual(100);
        }}
      />
      <TestCase.Manual
        modal
        itShould="display event sent to by onScrollToIndexFailed when pressing the button before scrolling"
        initialState={undefined}
        arrange={({state, setState}) => {
          return (
            <VirtualizedListOnScrollToIndexFailed
              state={state}
              setState={setState}
            />
          );
        }}
        assert={({state, expect}) => {
          expect(state).to.be.not.undefined;
          expect(state).to.have.all.keys([
            'index',
            'highestMeasuredFrameIndex',
            'averageItemLength',
          ]);
        }}
      />
      <TestCase.Example modal itShould="invert the list">
        <InvertedVirtualizedListTest />
      </TestCase.Example>
      <TestCase.Example modal itShould="start at the 81st item">
        <InitialScrollIndexTest />
      </TestCase.Example>
      <TestSuite name="ref">
        <TestCase.Example
          modal
          itShould="scroll to the element with the index 10 (Item 11) - scrollToIndex()">
          <VirtualizedListScrollToIndexTest />
        </TestCase.Example>
        <TestCase.Example
          modal
          itShould="scroll to the specific element (Item 3) - scrollToItem()">
          <VirtualizedListScrollToItemTest />
        </TestCase.Example>
        <TestCase.Manual
          modal
          itShould="scroll to the end of the list - scrollToEnd()"
          initialState={false}
          arrange={({state, setState}) => {
            return (
              <VirtualizedListGetScrollToEnd
                state={state}
                setState={setState}
              />
            );
          }}
          assert={({state, expect}) => {
            expect(state).to.be.true;
          }}
        />
        <TestCase.Manual
          modal
          itShould="get the node number - getScrollableNode()"
          initialState={undefined}
          arrange={({state, setState}) => {
            return (
              <VirtualizedListGetScrollableNode
                state={state}
                setState={setState}
              />
            );
          }}
          assert={({state, expect}) => {
            expect(state).to.be.not.undefined;
            expect(state).to.be.an('number');
          }}
        />

        <TestCase.Manual
          modal
          itShould="get the scroll ref - getScrollRef()"
          initialState={undefined}
          arrange={({state, setState}) => {
            return (
              <VirtualizedListGetScrollRef state={state} setState={setState} />
            );
          }}
          assert={({state, expect}) => {
            expect(state).to.be.not.undefined;
          }}
        />
      </TestSuite>
      <TestCase.Manual
        modal
        itShould="click (call on ref.recordInteraction()) on button before first scroll should trigger onViewableItemsChanged and change the first two items background color to blue"
        initialState={[]}
        arrange={({state, setState}) => {
          return (
            <VirtualizedListRecordInteractionTest
              state={state}
              setState={setState}
            />
          );
        }}
        assert={({state, expect}) => {
          expect(state).to.be.not.empty;
          expect(state).to.be.an('array');
          expect(state).to.have.lengthOf(2);
        }}
      />
      <TestCase.Example
        modal
        itShould="change background color of visible items after scrolling slightly (to blue when fully visible and lightblue when at least 20% is visible)">
        <VirtualizedListTestViewabiliyConfigCallbackPairs />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="change background color of fully visible items after 2 seconds">
        <VirtualizedListViewabilityConfigViewTime />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display even items with a lightgray background and odd items should have lightblue background (CellRenderedComponent)">
        <VirtualizedListCellRendererComponent />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould='display "Separator" text on lightgray background between items'>
        <VirtualizedListItemSeparatorTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould='display "Empty Component" text on lightgray background when data is empty'>
        <VirtualizedListListEmptyComponentTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould='display "Item Component" text on lightgray background'>
        <VirtualizedListListItemComponent />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould='display "List Footer Component" text on lightgray background at the end of the list'>
        <VirtualizedListListFooterComponent />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould='display "List Footer Component" text on red background at the end of the list'>
        <VirtualizedListListFooterComponentStyles />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould='display "List Header Component" text on small lightgray background on red background at the start of the list'>
        <VirtualizedListListHeaderComponent />
      </TestCase.Example>
      <TestCase.Example itShould="display debugging scroll bars (debug = true)">
        <VirtualizedListDebugTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="change background color of selected items to red">
        <VirtualizedListExtraDataTest />
      </TestCase.Example>
      <TestCase.Example modal itShould="display items horizontally">
        <VirtualizedListHorizontalTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display items horizontally and invert the list">
        <VirtualizedListHorizontalInvertedTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display 'refreshing' on red background when pulling down (onRefresh method)">
        <VirtualizedListOnRefreshTest />
      </TestCase.Example>
      <TestCase.Example modal itShould="display persistent scrollbar">
        <VirtualizedListPersistentScrollbarTest />
      </TestCase.Example>
      <TestCase.Example modal itShould="render cell in 2 second batches">
        <VirtualizedListUpdateCellsBatchingPeriodTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display refreshing indicator when pulling down (refreshControl)">
        <VirtualizedListRefreshControlTest />
      </TestCase.Example>
    </TestSuite>
  );
}

function VirtualizedListOnScrollToIndexFailed({
  state,
  setState,
}: {
  state: OnScrollToIndexFailed | undefined;
  setState: (state: any) => void;
}) {
  const ref = useRef<VirtualizedList<ItemData>>(null);

  const handleOnPress = () => {
    if (ref.current) {
      ref.current.scrollToIndex({index: 20, animated: true});
    }
  };

  return (
    <>
      <Button label="Scroll to NOT_EXISTING index" onPress={handleOnPress} />
      <View style={{height: 50, backgroundColor: 'lightblue'}}>
        <Text>{state ? JSON.stringify(state) : ''}</Text>
      </View>
      <VirtualizedList
        initialNumToRender={5}
        windowSize={5}
        ref={ref}
        style={{height: 128}}
        getItem={getItem}
        getItemCount={getItemCountVirtualized}
        renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
        keyExtractor={(item: ItemData) => item.id}
        onScrollToIndexFailed={(failInfo: OnScrollToIndexFailed) => {
          // @ts-ignore
          setState(failInfo);
        }}
      />
    </>
  );
}

const GENERATED_DATA = Array.from({length: 100}, (_, index) => ({
  id: String(index),
  title: `Item ${index + 1}`,
}));

function VirtualizedListScrollToIndexTest() {
  const ref = useRef<VirtualizedList<ItemData>>(null);

  const handleOnPress = () => {
    if (ref.current) {
      ref.current?.scrollToIndex({index: 10, animated: true});
    }
  };

  return (
    <>
      <Button label="Scroll to index = 10" onPress={handleOnPress} />
      <VirtualizedList
        ref={ref}
        style={{height: 128}}
        getItem={(_, index: number) => GENERATED_DATA[index]}
        getItemCount={() => GENERATED_DATA.length}
        renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
        keyExtractor={(item: ItemData) => item.id}
      />
    </>
  );
}

function VirtualizedListScrollToItemTest() {
  const ref = useRef<VirtualizedList<ItemData>>(null);

  const handleOnPress = () => {
    if (ref.current) {
      ref.current.scrollToItem({item: GENERATED_DATA[2], animated: true});
    }
  };

  return (
    <>
      <Button label="Scroll to item = 3" onPress={handleOnPress} />
      <VirtualizedList
        ref={ref}
        style={{height: 256}}
        data={GENERATED_DATA}
        getItem={(_, index: number) => GENERATED_DATA[index]}
        getItemCount={() => GENERATED_DATA.length}
        getItemLayout={(_, index: number) => ({
          length: 48,
          offset: 48 * index,
          index,
        })}
        renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
        keyExtractor={(item: ItemData) => item.id}
      />
    </>
  );
}

function InvertedVirtualizedListTest() {
  return (
    <VirtualizedList
      style={{height: 256}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      inverted
      renderItem={({item}: {item: ItemData}) => (
        <TouchableOpacity onPress={() => console.log(item.title)}>
          <Item title={item.title} />
        </TouchableOpacity>
      )}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

function InitialScrollIndexTest() {
  return (
    <VirtualizedList
      style={{height: 256}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      initialScrollIndex={80}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

function VirtualizedListGetScrollableNode({
  state,
  setState,
}: {
  state: number | undefined;
  setState: (state: any) => void;
}) {
  const ref = useRef<VirtualizedList<ItemData>>(null);

  const handleOnPress = () => {
    if (ref.current) {
      // @ts-ignore - getScrollableNode() is not in the type definition
      // in react-native repository but it is in the documentation
      const node = ref.current.getScrollableNode();
      setState(node);
    }
  };

  return (
    <>
      <Button label="Get ScrollableNode" onPress={handleOnPress} />
      <View style={{height: 50, backgroundColor: 'lightblue'}}>
        <Text>{`ScrollableNode = ${state}`}</Text>
      </View>
      <VirtualizedList
        ref={ref}
        style={{height: 128}}
        getItem={getItem}
        getItemCount={getItemCountVirtualized}
        renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
        keyExtractor={(item: ItemData) => item.id}
      />
    </>
  );
}

function VirtualizedListGetScrollRef({
  state,
  setState,
}: {
  state: boolean | undefined;
  setState: (state: any) => void;
}) {
  const ref = useRef<VirtualizedList<ItemData>>(null);

  const handleOnPress = () => {
    if (ref.current) {
      const scrollRef = ref.current.getScrollRef();
      setState(Boolean(scrollRef));
    }
  };
  return (
    <>
      <Button label="Get ScrollRef" onPress={handleOnPress} />
      <View style={{height: 50, backgroundColor: 'lightblue'}}>
        <Text>{`ScrollRef is defined: ${state}`}</Text>
      </View>
      <VirtualizedList
        ref={ref}
        style={{height: 128}}
        getItem={getItem}
        getItemCount={getItemCountVirtualized}
        renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
        keyExtractor={(item: ItemData) => item.id}
      />
    </>
  );
}

function VirtualizedListGetScrollToEnd({
  state,
  setState,
}: {
  state: boolean | undefined;
  setState: (state: any) => void;
}) {
  const ref = useRef<VirtualizedList<ItemData>>(null);

  const handleOnPress = () => {
    if (ref.current) {
      ref.current.scrollToEnd({animated: true});
    }
  };
  return (
    <>
      <Button label="Scroll to the end" onPress={handleOnPress} />
      <View style={{height: 50, backgroundColor: 'lightblue'}}>
        <Text>{`End is reached: ${state}`}</Text>
      </View>
      <VirtualizedList
        ref={ref}
        style={{height: 128}}
        getItem={getItem}
        getItemCount={getItemCountVirtualized}
        renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
        keyExtractor={(item: ItemData) => item.id}
        onEndReached={() => setState(true)}
      />
    </>
  );
}

interface ViewToken<TItem> {
  item: TItem;
  key: string;
  index: number | null;
  isViewable: boolean;
  section?: any | undefined;
}

type OnViewableItemsChangedType<TItem> = {
  viewableItems: Array<ViewToken<TItem>>;
  changed: Array<ViewToken<TItem>>;
};

const deafultViewabilityConfig: ViewabilityConfig = {
  // Nothing is considered viewable until the user scrolls or `recordInteraction`
  // is called after render.
  waitForInteraction: true,

  // minimum amount of time (in milliseconds) that an item must be physically viewable
  // before the viewability callback will be fired
  minimumViewTime: 100,

  // viewAreaCoveragePercentThreshold: 100,
  itemVisiblePercentThreshold: 70,
};

export const MockedVideoPlayer = ({
  itemId,
  height,
  playMockVideo,
  prefetchMockVideo,
}: {
  height?: number;
  itemId: string | number;
  playMockVideo: boolean;
  prefetchMockVideo?: boolean;
}) => {
  const backgroundPlayingColor = playMockVideo ? 'blue' : 'lightgray';
  const backgroundPrefetchingColor = prefetchMockVideo
    ? 'lightblue'
    : 'lightgray';

  return (
    <View
      style={{
        height: height ?? 200,
        backgroundColor: playMockVideo
          ? backgroundPlayingColor
          : backgroundPrefetchingColor,
        borderWidth: 2,
        marginBottom: 5,
        justifyContent: 'center',
      }}>
      <Text style={{textAlign: 'center'}}>Item with id: {itemId}</Text>
      <Text style={{textAlign: 'center'}}>Mocked Video Player</Text>
      <Text style={{textAlign: 'center'}}>
        {playMockVideo
          ? 'Playing'
          : prefetchMockVideo
            ? 'Prefetching...'
            : 'Waiting to be visible'}
      </Text>
    </View>
  );
};

function VirtualizedListRecordInteractionTest({
  state,
  setState,
}: {
  state: string[];
  setState: (state: any) => void;
}) {
  const [visibleItems, setVisibleItems] = useState<string[]>(state);

  const ref = useRef<VirtualizedList<ItemData>>(null);

  const handleOnPress = () => {
    if (ref.current) {
      ref.current.recordInteraction();
    }
  };

  const onViewableItemsChanged = ({
    viewableItems,
  }: OnViewableItemsChangedType<ItemData>) => {
    const newVisibleItems = viewableItems.map(
      viewableItem => viewableItem.item.id,
    );
    setVisibleItems(newVisibleItems);
    setState(newVisibleItems);
  };

  return (
    <View style={{height: 600}}>
      <View style={{marginBottom: 10}}>
        <Button label="Record interaction" onPress={handleOnPress} />
        <Text style={{padding: 10}}>
          Visible Items are: {JSON.stringify(visibleItems)}
        </Text>
      </View>
      <VirtualizedList
        ref={ref}
        style={{height: 128}}
        getItem={getItem}
        getItemCount={getItemCountVirtualized}
        renderItem={({item}: {item: ItemData}) => (
          <MockedVideoPlayer
            itemId={item.id}
            playMockVideo={visibleItems.includes(item.id)}
          />
        )}
        keyExtractor={(item: ItemData) => item.id}
        viewabilityConfig={deafultViewabilityConfig}
        onViewableItemsChanged={onViewableItemsChanged}
      />
    </View>
  );
}

const firstViewabilityConfig: ViewabilityConfig = {
  waitForInteraction: true,
  minimumViewTime: 100,
  itemVisiblePercentThreshold: 100,
};

const secondViewabilityConfig: ViewabilityConfig = {
  waitForInteraction: true,
  minimumViewTime: 100,
  itemVisiblePercentThreshold: 20,
};

function VirtualizedListTestViewabiliyConfigCallbackPairs() {
  const [firstVisibleItems, setFirstVisibleItems] = useState<string[]>([]);
  const [secondVisibleItems, setSecondVisibleItems] = useState<string[]>([]);

  const viewabilityConfigCallbackPairs = [
    {
      viewabilityConfig: firstViewabilityConfig,
      onViewableItemsChanged: ({
        viewableItems,
      }: OnViewableItemsChangedType<ItemData>) => {
        const newFirstVisibleItems = viewableItems.map(
          viewableItem => viewableItem.item.id,
        );
        setFirstVisibleItems(newFirstVisibleItems);
      },
    },
    {
      viewabilityConfig: secondViewabilityConfig,
      onViewableItemsChanged: ({
        viewableItems,
      }: OnViewableItemsChangedType<ItemData>) => {
        const newSecondVisibleItems = viewableItems.map(
          viewableItem => viewableItem.item.id,
        );
        setSecondVisibleItems(newSecondVisibleItems);
      },
    },
  ];

  return (
    <View style={{height: 600}}>
      <View style={{marginBottom: 10}}>
        <Text>
          First threshold {firstViewabilityConfig.itemVisiblePercentThreshold}%{' '}
          visible items are: {JSON.stringify(firstVisibleItems)}
        </Text>
        <Text>
          second threshold {secondViewabilityConfig.itemVisiblePercentThreshold}
          % visible items are: {JSON.stringify(secondVisibleItems)}
        </Text>
      </View>
      <VirtualizedList
        style={{height: 128}}
        getItem={getItem}
        getItemCount={getItemCountVirtualized}
        renderItem={({item}: {item: ItemData}) => (
          <MockedVideoPlayer
            height={300}
            itemId={item.id}
            playMockVideo={firstVisibleItems.includes(item.id)}
            prefetchMockVideo={secondVisibleItems.includes(item.id)}
          />
        )}
        keyExtractor={(item: ItemData) => item.id}
        viewabilityConfigCallbackPairs={viewabilityConfigCallbackPairs}
      />
    </View>
  );
}

const thirdViewabilityConfig: ViewabilityConfig = {
  waitForInteraction: true,
  minimumViewTime: 2000,
  itemVisiblePercentThreshold: 100,
};

function VirtualizedListViewabilityConfigViewTime() {
  const [visibleItems, setVisibleItems] = useState<string[]>([]);

  const onViewableItemsChanged = ({
    viewableItems,
  }: OnViewableItemsChangedType<ItemData>) => {
    const newVisibleItems = viewableItems.map(
      viewableItem => viewableItem.item.id,
    );

    setVisibleItems(newVisibleItems);
  };

  return (
    <View style={{height: 600}}>
      <View style={{marginBottom: 10}}>
        <Text style={{padding: 10}}>
          Visible Items after {thirdViewabilityConfig.minimumViewTime}ms are:{' '}
          {JSON.stringify(visibleItems)}
        </Text>
      </View>
      <VirtualizedList
        style={{height: 128}}
        getItem={getItem}
        getItemCount={getItemCountVirtualized}
        renderItem={({item}: {item: ItemData}) => (
          <MockedVideoPlayer
            itemId={item.id}
            playMockVideo={visibleItems.includes(item.id)}
          />
        )}
        keyExtractor={(item: ItemData) => item.id}
        viewabilityConfig={thirdViewabilityConfig}
        onViewableItemsChanged={onViewableItemsChanged}
      />
    </View>
  );
}

function VirtualizedListCellRendererComponent() {
  return (
    <VirtualizedList
      style={{height: 128}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
      CellRendererComponent={({
        index,
        children,
        style,
      }: {
        index: number;
        children: React.ReactNode;
        style: any;
      }) => {
        return (
          <View
            style={{
              ...style,
              backgroundColor: index % 2 === 0 ? 'lightblue' : 'lightgray',
            }}>
            {children}
          </View>
        );
      }}
    />
  );
}

function VirtualizedListItemSeparatorTest() {
  return (
    <VirtualizedList
      style={{height: 128}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
      ItemSeparatorComponent={() => (
        <View
          style={{
            height: 20,
            backgroundColor: 'lightgray',
            alignSelf: 'center',
            width: '90%',
          }}>
          <Text>Separator</Text>
        </View>
      )}
    />
  );
}

function VirtualizedListListEmptyComponentTest() {
  return (
    <VirtualizedList
      style={{height: 128}}
      data={[]}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => 0}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
      ListEmptyComponent={() => (
        <View
          style={{
            backgroundColor: 'lightgray',
            alignSelf: 'center',
            width: '90%',
            height: 100,
          }}>
          <Text>Empty Component</Text>
        </View>
      )}
    />
  );
}

function VirtualizedListListItemComponent() {
  return (
    <VirtualizedList
      style={{height: 128}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      // @ts-ignore
      ListItemComponent={props => (
        <View
          style={{
            backgroundColor: 'lightgray',
            alignSelf: 'center',
            width: '90%',
            height: 100,
            borderBottomWidth: 1,
            justifyContent: 'center',
          }}>
          <Text style={{textAlign: 'center'}}>Item Component</Text>
          <Text style={{textAlign: 'center'}}>{props.item.title}</Text>
        </View>
      )}
    />
  );
}

function VirtualizedListListFooterComponent() {
  return (
    <VirtualizedList
      style={{height: 128}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      ListFooterComponent={() => (
        <View
          style={{
            backgroundColor: 'lightgray',
            alignSelf: 'center',
            width: '90%',
            height: 100,
            borderBottomWidth: 1,
            justifyContent: 'center',
          }}>
          <Text style={{textAlign: 'center'}}>List Footer Component</Text>
        </View>
      )}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

function VirtualizedListListFooterComponentStyles() {
  return (
    <VirtualizedList
      style={{height: 128}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      ListFooterComponent={() => (
        <View>
          <Text style={{textAlign: 'center'}}>List Footer Component</Text>
        </View>
      )}
      ListFooterComponentStyle={{
        backgroundColor: 'red',
        alignSelf: 'center',
        width: '90%',
        height: 100,
        borderBottomWidth: 1,
        justifyContent: 'center',
      }}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

function VirtualizedListListHeaderComponent() {
  return (
    <VirtualizedList
      style={{height: 128}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      ListHeaderComponent={() => (
        <View
          style={{
            backgroundColor: 'lightgray',
          }}>
          <Text style={{textAlign: 'center'}}>List Header Component</Text>
        </View>
      )}
      ListHeaderComponentStyle={{
        backgroundColor: 'red',
        alignSelf: 'center',
        width: '90%',
        height: 100,
        borderBottomWidth: 1,
        justifyContent: 'center',
      }}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

function VirtualizedListDebugTest() {
  return (
    <VirtualizedList
      debug
      disableVirtualization
      style={{height: 256}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

const CustomItem = ({
  title,
  isSelected,
}: {
  isSelected: boolean;
  title: string;
}) => {
  return (
    <TouchableOpacity
      style={{
        backgroundColor: isSelected ? 'red' : 'lightgray',
      }}>
      <Item title={title} />
    </TouchableOpacity>
  );
};

function VirtualizedListExtraDataTest() {
  const [items] = useState(GENERATED_DATA);
  const [selectedId, setSelectedId] = useState<string | undefined>(undefined);

  return (
    <View>
      <Button label="Select 5 item" onPress={() => setSelectedId('4')} />
      <VirtualizedList
        style={{height: 256}}
        data={items}
        getItem={(_, index: number) => items[index]}
        getItemCount={() => items.length}
        getItemLayout={(_, index: number) => ({
          length: 48,
          offset: 48 * index,
          index,
        })}
        extraData={selectedId}
        renderItem={({item}: {item: ItemData}) => (
          <CustomItem isSelected={selectedId === item.id} title={item.title} />
        )}
        keyExtractor={(item: ItemData) => item.id}
      />
    </View>
  );
}

function VirtualizedListHorizontalTest() {
  return (
    <VirtualizedList
      style={{height: 256}}
      horizontal
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      renderItem={({item}: {item: ItemData}) => (
        <HorizontalItem title={item.title} />
      )}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

function VirtualizedListHorizontalInvertedTest() {
  return (
    <VirtualizedList
      style={{height: 256}}
      horizontal
      inverted
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      renderItem={({item}: {item: ItemData}) => (
        <HorizontalItem title={item.title} />
      )}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

function VirtualizedListOnRefreshTest() {
  const [refreshing, setRefreshing] = useState(false);

  const handleOnRefresh = () => {
    setRefreshing(true);
    setTimeout(() => {
      setRefreshing(false);
    }, 1000);
  };

  return (
    <View>
      <View
        style={{
          padding: 10,
          backgroundColor: refreshing ? 'red' : 'transparent',
        }}>
        <Text>{refreshing ? 'isRefreshing' : 'Refresh!'}</Text>
      </View>
      <VirtualizedList
        style={{height: 256}}
        onRefresh={handleOnRefresh}
        refreshing={refreshing}
        data={GENERATED_DATA}
        getItem={(_, index: number) => GENERATED_DATA[index]}
        getItemCount={() => GENERATED_DATA.length}
        getItemLayout={(_, index: number) => ({
          length: 48,
          offset: 48 * index,
          index,
        })}
        renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
        keyExtractor={(item: ItemData) => item.id}
      />
    </View>
  );
}

function VirtualizedListPersistentScrollbarTest() {
  return (
    <VirtualizedList
      style={{height: 256}}
      persistentScrollbar
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

function VirtualizedListUpdateCellsBatchingPeriodTest() {
  return (
    <VirtualizedList
      style={{height: 128}}
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
      updateCellsBatchingPeriod={2000}
      maxToRenderPerBatch={1}
    />
  );
}

function VirtualizedListRefreshControlTest() {
  const [refreshing, setRefreshing] = useState(false);

  const handleOnRefresh = () => {
    setRefreshing(true);
    setTimeout(() => {
      setRefreshing(false);
    }, 1000);
  };

  return (
    <VirtualizedList
      style={{height: 256}}
      refreshControl={
        <RefreshControl refreshing={refreshing} onRefresh={handleOnRefresh} />
      }
      data={GENERATED_DATA}
      getItem={(_, index: number) => GENERATED_DATA[index]}
      getItemCount={() => GENERATED_DATA.length}
      getItemLayout={(_, index: number) => ({
        length: 48,
        offset: 48 * index,
        index,
      })}
      renderItem={({item}: {item: ItemData}) => <Item title={item.title} />}
      keyExtractor={(item: ItemData) => item.id}
    />
  );
}

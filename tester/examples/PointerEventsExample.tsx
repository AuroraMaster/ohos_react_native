/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, { useState } from 'react';
import {
  ActivityIndicator,
  FlatList,
  Image,
  RefreshControl,
  ScrollView,
  StyleSheet,
  Switch,
  Text,
  TextInput,
  TouchableOpacity,
  View,
} from 'react-native';

const REACT_NATIVE_LOGO_URL =
  'https://upload.wikimedia.org/wikipedia/commons/thumb/a/a7/React-icon.svg/1920px-React-icon.svg.png';

const Item = ({
  title,
  onPress,
  selected = false,
}: {
  title: string;
  onPress: () => void;
  selected?: boolean;
}) => {
  return (
    <TouchableOpacity onPress={onPress}>
      <View
        style={{
          height: 50,
          backgroundColor: selected ? 'rgb(240,240,240)' : 'lightgrey',
          borderWidth: 1,
          padding: 4,
          justifyContent: 'center',
          alignItems: 'center',
        }}>
        <Text>{title}</Text>
      </View>
    </TouchableOpacity>
  );
};

const ScrollViewItem = ({
  color,
  width,
  height,
}: {
  color: string;
  width?: number;
  height?: number;
}) => {
  return (
    <View
      style={{
        backgroundColor: color,
        width: width,
        height: height,
      }}
    />
  );
};
type PointerEventsExample = {
  component: () => React.JSX.Element;
  title: string;
};

function CounterButton() {
  const [pressCount, setPressCount] = useState(0);
  return (
    <View
      style={styles.button}
      onTouchEnd={() => setPressCount(prev => prev + 1)}>
      <Text style={styles.buttonText}>CLICK ME ({pressCount})</Text>
    </View>
  );
}

export function PointerEventsExample() {
  const [pointerEventsNone, setPointerEventsNone] = React.useState<
    'none' | undefined
  >(undefined);
  const [example, setExample] = React.useState(0);
  const [value, setValue] = React.useState(false);
  const [text, onChangeText] = React.useState('Text');
  const [isRefreshing, setIsRefreshing] = React.useState(false);
  const ref = React.useRef<ScrollView>(null);

  const onPress = (index: number) => () => {
    setExample(index);
  };

  const BoxOnlyMultiLayerNestingExample = () => {
    return (
      <View style={styles.container}>
        <Text>Clicking should not increase the counter value</Text>
        <View
          style={[
            styles.box,
            {
              backgroundColor: 'pink',
              flex: 1,
            },
          ]}
          pointerEvents="box-only"
          collapsable={false}>
          <Text>box-only</Text>
          <View
            style={[styles.button, {backgroundColor: 'lightgreen'}]}
            pointerEvents="box-none"
            collapsable={false}>
            <Text>box-none</Text>
            <CounterButton />
          </View>
        </View>
      </View>
    );
  };

  const BoxNoneMultiLayerNestingExample = () => {
    return (
      <View style={styles.container}>
        <ScrollView>
          <View
            style={[styles.box, {backgroundColor: 'lightblue', width: 300}]}
          />
        </ScrollView>
        <View
          style={[
            styles.box,
            {
              backgroundColor: 'pink',
              zIndex: 1,
              position: 'absolute',
              height: '100%',
            },
          ]}
          pointerEvents="box-none"
          collapsable={false}>
          <Text>
            Clicking should increase the counter value AND the lightblue
            ScrollView shouldn't be able to scroll when dragging over 'click me'
            rectangle
          </Text>
          <Text>box-none</Text>

          <View
            style={[styles.button, {backgroundColor: 'lightgreen'}]}
            pointerEvents="box-none"
            collapsable={false}>
            <Text>box-none</Text>
            <CounterButton />
          </View>
        </View>
      </View>
    );
  };

  const TouchableOpacityInViewExample = () => {
    return (
      <View style={styles.view} pointerEvents={pointerEventsNone}>
        <TouchableOpacity onPress={() => {}} style={{alignSelf: 'center'}}>
          <Text
            style={
              styles.text
            }>{`I am ${pointerEventsNone ? 'not ' : ''}pressable!`}</Text>
        </TouchableOpacity>
      </View>
    );
  };
  const TextInViewExample = () => {
    return (
      <View style={styles.view} pointerEvents={pointerEventsNone}>
        <Text style={styles.text}>
          {`I ${pointerEventsNone ? 'do not ' : ''}eat touches!`}
        </Text>
      </View>
    );
  };
  const ActivityIndicatorBareExample = () => {
    return (
      <ActivityIndicator
        animating
        style={{alignSelf: 'center'}}
        size={150}
        pointerEvents={pointerEventsNone}
      />
    );
  };
  const RefreshControlBareExample = () => {
    return (
      <>
        <ScrollView
          ref={ref}
          style={styles.scrollView}
          refreshControl={
            <RefreshControl
              progressViewOffset={10}
              refreshing={isRefreshing}
              onRefresh={() => {
                setIsRefreshing(true);
                setTimeout(() => setIsRefreshing(false), 1000);
              }}
              pointerEvents={pointerEventsNone}
            />
          }>
          <ScrollViewItem height={100} color="white" />
          <ScrollViewItem height={100} color="black" />
          <ScrollViewItem height={100} color="white" />
          <ScrollViewItem height={100} color="black" />
          <ScrollViewItem height={100} color="white" />
        </ScrollView>
      </>
    );
  };
  const ScrollViewInViewExample = () => {
    return (
      <View style={styles.view} pointerEvents={pointerEventsNone}>
        <ScrollView style={styles.scrollView}>
          <ScrollViewItem height={100} color="white" />
          <ScrollViewItem height={100} color="black" />
          <ScrollViewItem height={100} color="white" />
          <ScrollViewItem height={100} color="black" />
          <ScrollViewItem height={100} color="white" />
        </ScrollView>
      </View>
    );
  };
  const ScrollViewBareExample = () => {
    return (
      <>
        <ScrollView
          ref={ref}
          style={styles.scrollView}
          pointerEvents={pointerEventsNone}>
          <ScrollViewItem height={100} color="white" />
          <ScrollViewItem height={100} color="black" />
          <ScrollViewItem height={100} color="white" />
          <ScrollViewItem height={100} color="black" />
          <ScrollViewItem height={100} color="white" />
        </ScrollView>
        <Item
          title="scroll to 250"
          onPress={() => ref.current?.scrollTo({x: 0, y: 250})}
        />
      </>
    );
  };
  const ImageInViewExample = () => {
    return (
      <View style={styles.view} pointerEvents={pointerEventsNone}>
        <Image
          source={{uri: REACT_NATIVE_LOGO_URL}}
          height={100}
          resizeMode="contain"
        />
      </View>
    );
  };
  const ImageBareExample = () => {
    return (
      <Image
        source={{uri: REACT_NATIVE_LOGO_URL}}
        height={100}
        resizeMode="contain"
        // @ts-ignore
        pointerEvents={pointerEventsNone}
      />
    );
  };
  const SwitchInViewExample = () => {
    return (
      <View style={styles.view} pointerEvents={pointerEventsNone}>
        <Switch
          value={value}
          onChange={event => setValue(event.nativeEvent.value)}
        />
      </View>
    );
  };
  const SwitchBareExample = () => {
    return (
      <Switch
        value={value}
        onChange={event => setValue(event.nativeEvent.value)}
        pointerEvents={pointerEventsNone}
      />
    );
  };
  const TextInputInViewExample = () => {
    return (
      <View style={styles.view} pointerEvents={pointerEventsNone}>
        <TextInput
          style={styles.input}
          onChangeText={onChangeText}
          value={text}
        />
      </View>
    );
  };

  const TextInputBareExample = () => {
    return (
      <TextInput
        style={styles.input}
        onChangeText={onChangeText}
        value={text}
        pointerEvents={pointerEventsNone}
      />
    );
  };

  const PointerEventsExamples: PointerEventsExample[] = [
    {component: TextInputBareExample, title: 'TextInput'},
    {component: TextInputInViewExample, title: 'TextInput inside View'},
    {component: SwitchBareExample, title: 'Switch'},
    {component: SwitchInViewExample, title: 'Switch inside View'},
    {component: ImageBareExample, title: 'Image'},
    {component: ImageInViewExample, title: 'Image inside View'},
    {component: ScrollViewBareExample, title: 'ScrollView'},
    {component: ScrollViewInViewExample, title: 'ScrollView inside View'},
    {component: RefreshControlBareExample, title: 'RefreshControl'},
    {component: ActivityIndicatorBareExample, title: 'ActivityIndicator'},
    {component: TextInViewExample, title: 'Text inside View'},
    {
      component: TouchableOpacityInViewExample,
      title: 'TouchableOpacity inside View',
    },
    {
      component: BoxOnlyMultiLayerNestingExample,
      title: 'box-only multi-layer nesting',
    },
    {
      component: BoxNoneMultiLayerNestingExample,
      title: 'box-none multi-layer nesting',
    },
  ];
  return (
    <View style={styles.container}>
      <View style={styles.container80}>
        <Text style={styles.instructionsText}>
          Switching pointer events to none will allow to scroll the horizontal
          ScrollView in the background.
        </Text>
        <View>
          <FlatList
            horizontal
            data={PointerEventsExamples}
            renderItem={item => (
              <Item
                key={item.index}
                title={item.item.title}
                onPress={onPress(item.index)}
                selected={example === item.index}
              />
            )}
          />
          <ScrollView style={styles.backgroundScrollView} horizontal>
            <ScrollViewItem color="rgb(255, 230, 230)" width={300} />
            <ScrollViewItem color="rgb(230, 255, 230)" width={300} />
            <ScrollViewItem color="rgb(255, 230, 230)" width={300} />
          </ScrollView>
          <View style={styles.exampleContainer} pointerEvents="box-none">
            {PointerEventsExamples[example].component()}
          </View>
          <Item
            title={`Switch pointer events to ${pointerEventsNone ? 'default' : 'none'}`}
            onPress={() => {
              setPointerEventsNone(prev =>
                prev === undefined ? 'none' : undefined,
              );
            }}
          />
        </View>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
  },
  container80: {
    width: '80%',
  },
  instructionsText: {
    color: 'white',
  },
  backgroundScrollView: {
    position: 'absolute',
    height: 500,
    marginTop: 50,
  },
  exampleContainer: {
    height: 500,
    borderWidth: 1,
    padding: 4,
  },
  input: {
    height: 40,
    margin: 12,
    borderWidth: 1,
    padding: 10,
  },
  view: {
    borderWidth: 1,
    padding: 20,
  },
  scrollView: {
    maxHeight: 300,
    width: '50%',
    alignSelf: 'center',
  },
  text: {
    backgroundColor: 'lightgrey',
    width: 200,
    height: 50,
    verticalAlign: 'middle',
    textAlign: 'center',
    alignSelf: 'center',
  },
  box: {
    width: 200,
    height: 1200,
    justifyContent: 'center',
    alignItems: 'center',
  },
  button: {
    marginTop: 20,
    width: 160,
    paddingLeft: 0,
    paddingRight: 0,
    paddingBottom: 80,
    paddingTop: 80,
    backgroundColor: 'lightgray',
    borderRadius: 5,
  },
  buttonText: {
    fontSize: 16,
    textAlign: 'center',
  },
});

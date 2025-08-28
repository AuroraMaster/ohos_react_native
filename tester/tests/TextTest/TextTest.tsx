/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {Platform, StyleSheet, Text, View} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../../components';
import {useState} from 'react';
import {TextStyleTest} from './TextStyleTest';
import {TextMeasuringTest} from './TextMeasuringTest';
import {TextPaddingTest} from './TextPaddingTest';
import {TextAccessibilityTest} from './TextAccessibilityTest';
import {TextNestedTest} from './TextNestedTest';

export function TextTest() {
  return (
    <TestSuite name="Text">
      <TextMeasuringTest />
      <TextStyleTest />
      <TextPaddingTest />
      <TextAccessibilityTest />
      <TextNestedTest />
      <TestCase.Manual
        itShould='pass after pressing "Press me!"'
        initialState={false}
        arrange={({setState}) => (
          <Text onPress={() => setState(false)}>
            <Text>中中文中中文中中文</Text>
            <Text
              style={{
                color: 'red',
              }}
              onPress={() => {
                setState(true);
              }}>
              Press Me!!
            </Text>
            <Text>rest of the text that's not pressable</Text>
          </Text>
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example itShould="show selectable text">
        <View style={styles.smallContainer}>
          <Text style={styles.smallText} selectable={true}>
            Selectable text
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="Text bind touch">
      <View>
                <Text>
                    <Text>🎁 亲爱的hank：{'\n'}</Text>
                    <Text>🎉 请查收新一期活动邀约 👉 评论转想法，轻松赚盐粒{'\n'}</Text>
                    <Text>
                        💎「写评论同时发布到想法」即可参与盐粒瓜分，最高奖池 1200 万盐粒！{'\n'}
                    </Text>
                    <Text>
                        🌟 专属邀请通道已为你开启，马上报名了解详情{'>'}
                        {'>'}
                    </Text>
                    <Text
                        style={{
                            backgroundColor: 'green',
                        }}
                        onPress={() => {
                            console.log("点击了链接")
                        }}
                    >
                        评论区开麦计划
                    </Text>
                    <Text>{'\n'}</Text>,<Text>爱你的创作者小助手{'\n'}</Text>
                    <Text>gggggggg{'\n'}</Text>
                </Text>
            </View>
      </TestCase.Example>
      <TestCase.Example itShould="render text in one line">
        <View style={{flexDirection: 'row'}}>
          <Text style={{fontSize: 10}}>/ {'100'}</Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="show 3 texts each with a different text break startegy">
        <View style={styles.bigContainer}>
          <Text style={styles.smallTextWidth} textBreakStrategy="simple">
            Lorem ipsum dolor sit amet
          </Text>
          <Text style={styles.smallTextWidth} textBreakStrategy="highQuality">
            Lorem ipsum dolor sit amet
          </Text>
          <Text style={styles.smallTextWidth} textBreakStrategy="balanced">
            Lorem ipsum dolor sit amet
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Example
        itShould="show 3 texts each with a different line break startegy"
        skip={{android: true, harmony: true}}
        //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/274
      >
        <View style={styles.bigContainer}>
          <Text style={styles.smallTextWidth} lineBreakStrategyIOS="none">
            Lorem ipsum dolor sit amet
          </Text>
          <Text style={styles.smallTextWidth} lineBreakStrategyIOS="standard">
            Lorem ipsum dolor sit amet
          </Text>
          <Text style={styles.smallTextWidth} lineBreakStrategyIOS="push-out">
            Lorem ipsum dolor sit amet
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Example
        itShould="wrap two texts differently (hangul-word linebreak stategy)"
        skip={{android: true, harmony: true}}
        //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/274
      >
        <View style={styles.container}>
          <Text style={styles.smallTextWidth} lineBreakStrategyIOS="none">
            ㄱㄱㄱㄱㄱㄱㄱㄱㄱㄱ ㄱㄱㄱㄱ
          </Text>
          <Text
            style={styles.smallTextWidth}
            lineBreakStrategyIOS="hangul-word">
            ㄱㄱㄱㄱㄱㄱㄱㄱㄱㄱ ㄱㄱㄱㄱ
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="show two texts, both selectable, but one disabled ">
        <View
          style={{
            ...styles.smallContainer,
            flexDirection: 'row',
            justifyContent: 'space-between',
          }}>
          <Text style={styles.smallText} disabled selectable>
            Disabled
          </Text>
          <Text style={styles.smallText} selectable>
            Enabled
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="show text with different ellipsize mode and clip in the middle of second sentence">
        <View style={styles.hugeContainer}>
          <Text style={styles.smallText} ellipsizeMode="tail" numberOfLines={1}>
            head, AReallyReallyLongSentenceWithoutSeperatorsOrSpaces.
          </Text>
          <Text style={styles.smallText} ellipsizeMode="clip" numberOfLines={1}>
            head, AReallyReallyLongSentenceWithoutSeperatorsOrSpaces.
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Manual
        itShould="fire onTextLayoutEvent after layout change"
        initialState={false}
        arrange={ctx => <OnTextLayoutView ctx={ctx} />}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
        skip={{android: false, harmony: {arkTS: true, cAPI: true}}}
        //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/277
      />
      <TestCase.Manual
        itShould="fire onLayout event after layout change"
        initialState={{}}
        arrange={ctx => <OnLayoutView ctx={ctx} />}
        assert={({expect, state}) => {
          expect(state).to.have.all.keys('x', 'y', 'width', 'height');
        }}
      />
      <TestCase.Example
        itShould="display text with a selection color of Indian Red"
        skip={Platform.select({harmony: 'Missing prop on ArkUI side'})}>
        <Text selectionColor="#F15156" selectable>
          Text with indian red selection color
        </Text>
      </TestCase.Example>
      <TestCase.Example
        itShould="display text with a selection color of Blue"
        skip={Platform.select({harmony: 'Missing prop on ArkUI side'})}>
        <Text selectionColor="blue" selectable>
          Text with blue selection color but on longer text. In React Native,
          the selectionColor prop in the Text component is used to customize the
          color of the text highlight when the text field is selected. This is
          useful when you want the text highlight color to match your app's
          theme. 一 乙 二 十 丁 厂 七 卜 人 入 八 九 几 儿 了 力 乃 又
        </Text>
      </TestCase.Example>
      <TestCase.Example itShould="render <Text /> without content/text">
        <View
          style={{
            width: '100%',
            backgroundColor: 'pink',
          }}>
          <Text style={{fontSize: 20, backgroundColor: 'lightgreen'}}>
            {''}
          </Text>
          <Text style={{fontSize: 20, backgroundColor: 'pink'}}>
            {undefined}
          </Text>
          <Text style={{fontSize: 20, backgroundColor: 'lightblue'}} />
        </View>
      </TestCase.Example>
    </TestSuite>
  );
}
const OnLayoutView = (props: {
  ctx: {
    state: {};
    setState: React.Dispatch<React.SetStateAction<{}>>;
    reset: () => void;
  };
}) => {
  const [width, setWidth] = useState(100);
  return (
    <View>
      <Text
        style={{
          width: width,
          height: 40,
          borderWidth: 1,
          fontSize: 16,
          backgroundColor: 'rgba(100,100,255,0.5)',
        }}
        onLayout={event => {
          props.ctx.setState(event.nativeEvent.layout);
        }}
        onPress={() => setWidth((prev: number) => (prev === 100 ? 200 : 100))}>
        resize
      </Text>
    </View>
  );
};
const OnTextLayoutView = (props: {
  ctx: {
    state: boolean;
    setState: React.Dispatch<React.SetStateAction<boolean>>;
    reset: () => void;
  };
}) => {
  const [width, setWidth] = useState(100);

  return (
    <View style={styles.container}>
      <Text
        style={{
          ...styles.smallText,
          width: width,
          backgroundColor: 'blue',
        }}
        onTextLayout={() => props.ctx.setState(true)}>
        Lorem ipsum dolor sit amet
      </Text>
      <Button
        label="Restart"
        onPress={() => {
          setWidth(width === 100 ? 200 : 100);
          props.ctx.reset();
        }}
      />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    width: 200,
    height: 80,
    backgroundColor: 'red',
  },
  smallContainer: {
    width: 200,
    height: 40,
    backgroundColor: 'red',
  },
  smallContainerRow: {
    width: 200,
    height: 40,
    backgroundColor: 'red',
    flexDirection: 'row',
  },
  bigContainer: {
    width: 200,
    height: 120,
    backgroundColor: 'red',
  },
  hugeContainer: {
    width: 200,
    height: 160,
    backgroundColor: 'red',
  },
  text: {
    width: '100%',
    color: 'white',
  },
  smallText: {
    height: 30,
    color: 'white',
  },
  smallTextWidth: {
    height: 30,
    color: 'white',
    width: 150,
  },
  blueShortText: {
    height: 30,
    width: 50,
    color: 'white',
    backgroundColor: 'blue',
  },
  blackText: {
    width: '100%',
    height: '100%',
    color: 'black',
  },
  styledBox: {
    width: '100%',
    padding: 10,
    marginTop: 16,
    marginBottom: 16,
    textDecorationColor: 'red',
    textDecorationLine: 'underline',
  },
});

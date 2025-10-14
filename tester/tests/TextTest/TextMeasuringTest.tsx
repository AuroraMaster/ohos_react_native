/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {Text, View, Button} from 'react-native';
import {TestSuite, TestCase} from '@rnoh/testerino';
import {SAMPLE_PARAGRAPH_TEXT} from './fixtures';
import {useState, useRef} from 'react';

export function TextMeasuringTest() {
  return (
    <TestSuite name="Text">
      <TestSuite name="text measuring">
        <TestCase itShould="not wrap any text">
          <View
            style={{
              backgroundColor: 'yellow',
              width: '100%',
              height: 40,
              display: 'flex',
              flexDirection: 'row',
              flexWrap: 'wrap',
            }}>
            {[
              {
                sortCode: '0',
                sortName: '综合',
              },
              {
                sortCode: '2',
                sortName: '最新',
              },
              {
                sortCode: '3',
                sortName: '评论',
              },
              {
                sortCode: '1',
                sortName: '价格',
              },
            ].map((item, index) => (
              <View
                style={{width: '20%', flexDirection: 'column'}}
                key={item.sortCode + '_' + index}>
                <View
                  style={{
                    width: '100%',
                    height: 40,
                    justifyContent: 'center',
                    alignItems: 'center',
                    flexDirection: 'row',
                  }}>
                  <Text
                    ellipsizeMode={'tail'}
                    numberOfLines={1}
                    style={{fontSize: 14, lineHeight: 19}}>
                    {item.sortName}
                  </Text>
                </View>
              </View>
            ))}
          </View>
        </TestCase>
        <TestCase itShould="display all texts in one line">
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'cyan'}}>
              {'0:12'}
              {'场'}
            </Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'pink'}}>
              {'0;12'}
              {'场'}
            </Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'cyan'}}>
              {'0.12'}
              {'场'}
            </Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'pink'}}>
              {'0,12'}
              {'场'}
            </Text>
          </View>
        </TestCase>
        <TestCase itShould="display: 'FOO''BAR' next to each other">
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{height: '100%', backgroundColor: 'pink'}}>FOO</Text>
            <Text style={{height: '100%', backgroundColor: 'cyan'}}>BAR</Text>
          </View>
        </TestCase>
        <TestCase itShould="display: 'FOO''BAR' in two lines">
          <View
            style={{
              width: 32,
              flexDirection: 'row',
              borderWidth: 1,
            }}>
            <Text style={{height: '100%', backgroundColor: 'pink'}}>
              FOO BAR
            </Text>
          </View>
        </TestCase>
        <TestCase itShould="display: 'FOO''BAR' next to each other with different fonts">
          <View
            style={{height: 32, alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text
              style={{
                height: '100%',
                backgroundColor: 'pink',
                fontFamily: 'StintUltraCondensed-Regular',
              }}>
              FOO
            </Text>
            <Text
              style={{
                height: '100%',
                backgroundColor: 'pink',
                fontFamily: 'Pacifico-Regular',
              }}>
              BAR
            </Text>
          </View>
        </TestCase>
        <TestCase itShould="display: 'FOO''BAR' next to each other with different letterSpacing">
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text
              style={{
                height: '100%',
                backgroundColor: 'pink',
                letterSpacing: 8,
              }}>
              FOO
            </Text>
            <Text
              style={{
                height: '100%',
                backgroundColor: 'cyan',
                letterSpacing: 4,
              }}>
              BAR
            </Text>
          </View>
        </TestCase>
        <TestCase itShould="text should not exceed cyan background (measuring text on cpp side)">
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'silver'}}>FONT SIZE</Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{fontSize: 20, backgroundColor: 'cyan'}}>
              FONT SIZE
            </Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'silver'}}>FONT WEIGHT</Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{fontWeight: 'bold', backgroundColor: 'cyan'}}>
              FONT WEIGHT
            </Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'silver'}}>LETTER SPACING!</Text>
          </View>
          {/* On Android letter spacing may cause the bounding box to be full width (remove ! to see the problem) */}
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{letterSpacing: 1, backgroundColor: 'cyan'}}>
              LETTER SPACING!
            </Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'silver'}}>
              NUMBER OF LINES @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @
              @ @ @
            </Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text
              style={{width: 256, backgroundColor: 'cyan'}}
              numberOfLines={1}>
              NUMBER OF LINES @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @
              @ @ @
            </Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text style={{backgroundColor: 'silver'}}>LINE HEIGHT</Text>
          </View>
          <View style={{alignSelf: 'flex-start', flexDirection: 'row'}}>
            <Text
              style={{lineHeight: 32, backgroundColor: 'cyan'}}
              numberOfLines={1}>
              LINE HEIGHT
            </Text>
          </View>
        </TestCase>
        <TestSuite name="views in text">
          <TestCase itShould="vertically align text and view">
            <Text style={{color: 'red', lineHeight: 82}}>
              {'Hello World'}
              <View style={{width: 12, height: 12, backgroundColor: 'blue'}} />
            </Text>
          </TestCase>
          <TestCase itShould="not crash when a view is trimmed by number of lines (#1)">
            <Text numberOfLines={1}>
              '中文测试中文测试中文测试中文测试中文测试中文测试中文测试中文测试'
              {<View style={{width: 64, height: 64, backgroundColor: 'red'}} />}
            </Text>
          </TestCase>
          <TestCase itShould="not crash when a view is trimmed by number of lines (#2)">
            <Text numberOfLines={1}>
              {'中文测试中文测试'}
              {<View style={{width: 308, height: 5, backgroundColor: 'red'}} />}
            </Text>
          </TestCase>
          <TestCase itShould="wrap first and second paragraph in the same way">
            <View style={{width: 200, backgroundColor: 'silver'}}>
              <Text>
                天地玄黄 宇宙洪荒 日月盈昃 辰宿列张 寒来暑往 秋收冬藏 闰馀成岁
              </Text>
              <Text style={{marginTop: 16}}>
                <Text style={{fontWeight: 'bold'}}>天地玄黄</Text> 宇宙洪荒
                日月盈昃 辰宿列张 寒来暑往 秋收冬藏 闰馀成岁
              </Text>
            </View>
          </TestCase>
          <TestCase itShould="not crash the app">
            <Text>
              <View style={{width: 64, height: 64, backgroundColor: 'red'}} />
            </Text>
          </TestCase>
          <TestCase itShould="render red rectangle after 'FOO'">
            <View
              style={{
                height: 32,
                alignSelf: 'flex-start',
                flexDirection: 'row',
              }}>
              <Text style={{height: '100%', backgroundColor: 'pink'}}>
                FOO
                <View style={{width: 32, height: 16, backgroundColor: 'red'}} />
                BAR
              </Text>
            </View>
          </TestCase>
          <TestCase itShould="render view in first line and text in second">
            <View
              style={{
                width: 256,
                borderWidth: 1,
              }}>
              <Text numberOfLines={2}>
                <View
                  style={{
                    width: 200,
                    height: 30,
                    backgroundColor: 'red',
                  }}
                />
                test12345678901234567890
              </Text>
            </View>
          </TestCase>
          <TestCase itShould="[fails on Android/Harmony] render red rectangle after 'FOO' (flex)">
            <View
              style={{
                height: 32,
                alignSelf: 'flex-start',
                flexDirection: 'row',
              }}>
              <Text
                style={{
                  height: '100%',
                  backgroundColor: 'pink',
                  flexDirection: 'row',
                }}>
                FOO
                <View style={{flex: 1, height: 32, backgroundColor: 'red'}} />
                BAR
              </Text>
            </View>
          </TestCase>
          <TestCase itShould="[buggy on Android/Harmony] render red rectangle after 'FOO' (width: 50%, height: 50%)">
            <View
              style={{
                height: 32,
                alignSelf: 'flex-start',
                flexDirection: 'row',
              }}>
              <Text style={{height: '100%', backgroundColor: 'pink'}}>
                FOO
                <View
                  style={{width: '50%', height: '50%', backgroundColor: 'red'}}
                />
                BAR
              </Text>
            </View>
          </TestCase>
          <TestCase itShould="render red rectangle with 'BAZ' inside after 'FOO'">
            <View
              style={{
                height: 32,
                alignSelf: 'flex-start',
                flexDirection: 'row',
              }}>
              <Text style={{height: '100%', backgroundColor: 'pink'}}>
                FOO
                <View style={{backgroundColor: 'red'}}>
                  <Text>BAZ</Text>
                </View>
                BAR
              </Text>
            </View>
          </TestCase>
        </TestSuite>
        <TestCase itShould="show a long text without a space below or above">
          <Text>{SAMPLE_PARAGRAPH_TEXT}</Text>
        </TestCase>
        <TestCase itShould="show a long text without a space below or above (fontFamily)">
          <Text style={{fontFamily: 'StintUltraCondensed-Regular'}}>
            {SAMPLE_PARAGRAPH_TEXT}
          </Text>
        </TestCase>
        <TestCase itShould="show a long text without a space below or above (font size)">
          <Text style={{fontSize: 8}}>{SAMPLE_PARAGRAPH_TEXT}</Text>
        </TestCase>
        <TestCase itShould="show a long text without a space below or above (line height)">
          <Text style={{lineHeight: 21}}>{SAMPLE_PARAGRAPH_TEXT}</Text>
        </TestCase>
        <TestCase itShould="show 2 lines of text">
          <Text numberOfLines={2}>{SAMPLE_PARAGRAPH_TEXT}</Text>
        </TestCase>
        <TestCase itShould="click to update numberOfLines">
          <TextUpdateNumberOfLinesTest />
        </TestCase>
        <TestCase itShould="show text without a space below or above (fragments)">
          <Text>
            <Text>
              Nostrud irure ex sunt dolor [\n]{'\n'}cillum irure laboris ex ut
              adipisicing magna reprehenderit Lorem.
            </Text>
            <Text style={{fontSize: 24}}>
              Do ullamco excepteur quis labore Lorem mollit tempor ex minim.
            </Text>
            <Text>
              Excepteur consequat officia ut incididunt consectetur qui
              reprehenderit quis quis ut cillum ad.
            </Text>
          </Text>
        </TestCase>
        <TestCase
          itShould="show text without a space below or above (fragments with different fonts)"
          skip
          //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/564
          //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/565
        >
          <Text>
            <Text style={{fontFamily: 'StintUltraCondensed-Regular'}}>
              Nostrud irure ex sunt dolor [\n]{'\n'}cillum irure laboris ex ut
              adipisicing magna reprehenderit Lorem.
            </Text>
            <Text style={{fontSize: 24, fontFamily: 'Pacifico-Regular'}}>
              Do ullamco excepteur quis labore Lorem mollit tempor ex minim.
            </Text>
            <Text>
              Excepteur consequat officia ut incididunt consectetur qui
              reprehenderit quis quis ut cillum ad.
            </Text>
          </Text>
        </TestCase>
        <TestCase
          itShould="auto pass when onTextLayout provides complete text information"
          initialState={null}
          arrange={({setState, state, done}: any) => {
            const safeDone = typeof done === 'function' ? done : () => {};
            return (
              <TextLayoutAutoPassTestWithCallback
                onLayoutComplete={(isValid: boolean) => {
                  setState(isValid);
                  safeDone();
                }}
              />
            );
          }}
          assert={({expect, state}: any) => {
            expect(state).to.be.true;
          }}
        />
      </TestSuite>
    </TestSuite>
  );
}

const TextUpdateNumberOfLinesTest = () => {
  const [caseIndex, setCaseIndex] = useState(0);
  const testCases = [...Array.from({length: 7}, (_, i) => i + 1), undefined];
  return (
    <View>
      <Button
        title={`Click, numberOfLines=${testCases[caseIndex]}`}
        onPress={() => {
          setCaseIndex(prev => (prev + 1) % testCases.length);
        }}
      />
      <Text numberOfLines={testCases[caseIndex]}>{SAMPLE_PARAGRAPH_TEXT}</Text>
    </View>
  );
};

const TextLayoutAutoPassTestWithCallback = (
  { onLayoutComplete }: { onLayoutComplete: (isValid: boolean) => void }
) => {
  const [textInfo, setTextInfo] = useState<{
    text: string;
    lines: number;
    width: number;
    height: number;
  } | null>(null);
  const testCaseRef = useRef(null);

  const expectedText =
    '这是一段示例文本，用于演示onTextLayout回调的使用。你可以通过这个回调获取文本的布局信息，例如行数、宽度和高度。';

  const handleTextLayout = (event: any) => {
    const { lines } = event.nativeEvent;
    if (lines && lines.length > 0) {
      const allText = lines.map((line: any) => line.text ?? '').join('');
      const firstLine = lines[0];
      const lastLine = lines[lines.length - 1];
      const totalHeight = lastLine.y + lastLine.height;
      const info = {
        text: allText,
        lines: lines.length,
        width: firstLine.width,
        height: totalHeight,
      };
      setTextInfo(info);
      const isValid =
        allText === expectedText &&
        lines.length > 0 &&
        firstLine.width > 0 &&
        totalHeight > 0;
      onLayoutComplete(isValid);
    }
  };

  return (
    <View style={{ padding: 10 }}>
      <Text
        ref={testCaseRef}
        onTextLayout={handleTextLayout}
        numberOfLines={2}
        style={{ fontSize: 16, marginBottom: 10 }}
      >
        {expectedText}
      </Text>
      {textInfo && (
        <View
          style={{
            marginTop: 10,
            padding: 8,
            backgroundColor: '#e7f3ff',
            borderRadius: 4,
          }}
        >
          <Text style={{ fontWeight: 'bold', marginBottom: 4 }}>
            文本布局信息:
          </Text>
          <Text>行数: {textInfo.lines}</Text>
          <Text>宽度: {textInfo.width}px</Text>
          <Text>高度: {textInfo.height}px</Text>
          <Text>文本: {textInfo.text}</Text>
        </View>
      )}
    </View>
  );
};

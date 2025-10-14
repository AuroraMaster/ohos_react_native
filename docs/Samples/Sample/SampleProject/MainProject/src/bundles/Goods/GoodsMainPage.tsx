/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import * as React from 'react';
import { useState, useRef } from 'react';
import {
  Text,
  StyleSheet,
  View,
  ScrollView,
  UIManager,
  findNodeHandle,
  requireNativeComponent,
  DeviceEventEmitter,
  Platform,
} from 'react-native';

import MarqueeView from '../basic/MarqueeView';
import ButtonView from '../basic/ButtonView';
import SampleTurboModule from '../basic/SampleTurboModule';
import SampleTurboModule2, {
  Fruit,
  ResultModel,
} from '../basic/SampleTurboModule2';
import GoodsButton from './GoodsButton';

const isCAPI =
  'rnohArchitecture' in Platform.constants &&
  Platform.constants.rnohArchitecture === 'C_API';

function AppGoods() {
  const [nativeCallbackValue, setNativeCallbackValue] = useState("");
  const [nativeAsyncCallbackValue, setNativeAsyncCallbackValue] = useState("");
  const [marqueeStop, setMarqueeStop] = useState(false);
  const [nativeEmitterParam, setNativeEmitterParam] = useState("");
  const [callFunctionParam, setCallFunctionParam] = useState("");
  const [nativeButtonEmitterParam, setNativeButtonEmitterParam] = useState("");
  const [buttonClick, setButtonClick] = useState(false);
  const nativeRef = useRef<any>(null);
  const nativeRefButton = useRef<any>(null);

  DeviceEventEmitter.addListener('clickMarqueeEvent', e => {
    setNativeEmitterParam(JSON.stringify(e.params.age));
  });

  DeviceEventEmitter.addListener('callFunction', e => {
    setCallFunctionParam(JSON.stringify(e.params.size));
  });

  return (
    <ScrollView>
      <View style={styles.container}>
        <MarqueeView
          src="双十一大促，消费是社会再生产过程中的一个重要环节，也是最终环节。它是指利用社会产品来满足人们各种需要的过程。"
          ref={nativeRef}
          style={{ height: 180, width: '100%', backgroundColor: '#1980E6' }}
          onStop={(e) => {
            SampleTurboModule.rnLog(
              'native调用了RN的 onStop，isStop = ' + e.nativeEvent.isStop,
            );
            setMarqueeStop(e.nativeEvent.isStop);
          }}
        />

        if (isCAPI) {
          <ButtonView
            buttonText={'ButtonView: ' + (buttonClick ? 'Click' : 'No Click')}
            ref={nativeRefButton}
            style={{ height: 50, width: 200, backgroundColor: 'blue' }}
            onButtonClick={(e) => {
              setButtonClick(e.nativeEvent.isButtonClick);
            }}
          />
        }
        <GoodsButton
          buttonText="•跳转商品详情页"
          onPress={() => {
            SampleTurboModule.pushStringToHarmony('pages/Details', 1);
          }}
        />
        <GoodsButton
          buttonText={'•跳转预创建RN实例页'}
          onPress={() => {
            SampleTurboModule.pushStringToHarmony('pages/PrecreateRN', 1);
          }}
        />
        <GoodsButton
          buttonText={'•跳转多RNSurface页'}
          onPress={() => {
            SampleTurboModule.pushStringToHarmony('pages/MultiSurface', 1);
          }}
        />
        <GoodsButton
          buttonText="•加载应用沙箱bundle"
          onPress={() => {
            SampleTurboModule.pushStringToHarmony('pages/Sandbox', 1);
          }}
        />
        <GoodsButton
          buttonText={'•调用TurboModule回调方法：' + nativeCallbackValue}
          onPress={() => {
            SampleTurboModule.registerFunction(value => {
              setNativeCallbackValue(value);
            });
          }}
        />
        <GoodsButton
          buttonText={
            '•调用TurboModule异步回调方法：' + nativeAsyncCallbackValue
          }
          onPress={() => {
            SampleTurboModule.doAsyncJob(false).then(
              result => {
                setNativeAsyncCallbackValue(result);
              },
              err => {
                setNativeAsyncCallbackValue((err as Error).message);
              },
            );
          }}
        />
        <GoodsButton
          buttonText="•调用原生console.log方法"
          onPress={() => {
            SampleTurboModule.rnLog('在Native中打印日志');
          }}
        />
        <GoodsButton
          buttonText={
            '•调用原生方法暂停跑马灯：' + (marqueeStop ? '停止' : '运动')
          }
          onPress={() => {
            UIManager.dispatchViewManagerCommand(
              findNodeHandle(nativeRef.current),
              'toggleMarqueeState',
              [],
            );
          }}
        />
        if (isCAPI) {
          <GoodsButton
            buttonText={'•调用原生方法点击Button,改变ButtonText'}
            onPress={() => {
              UIManager.dispatchViewManagerCommand(
                findNodeHandle(nativeRefButton.current),
                'changeButtonText',
                ['changeButtonText'],
              );
            }}
          />
        }
        <GoodsButton
          buttonText={'•调用TurboModule2的test方法'}
          onPress={() => {
            SampleTurboModule2.test();
          }}
        />
        <GoodsButton
          buttonText={'•调用TurboModule2的getObject方法'}
          onPress={() => {
            SampleTurboModule2.getObject({ x: 100 });
          }}
        />
        <GoodsButton
          buttonText={'•调用TurboModule2的getRequest方法'}
          onPress={() => {
            SampleTurboModule2.getRequest()
              .then((result: ResultModel) => {
                console.log(JSON.stringify(result));
                console.log(result.result.note);
              })
              .catch(err => {
                console.log(JSON.stringify(err));
              });
          }}
        />
        <GoodsButton
          buttonText={'•调用TurboModule2的eatFruit方法'}
          onPress={() => {
            const fruit: Fruit = {
              name: '大菠萝',
            };
            SampleTurboModule2.eatFruit(fruit);
          }}
        />
        <GoodsButton
          buttonText={'•调用TurboModule2的checkPwd方法'}
          onPress={() => {
            SampleTurboModule2.checkPwd(
              {},
              () => {
                SampleTurboModule.rnLog(
                  '调用TurboModule2的checkPwd回调了success',
                );
              },
              () => { },
            );
          }}
        />
        <GoodsButton
          buttonText={
            '•点击上方的跑马灯接收原生发送的消息：native -> js = ' +
            nativeEmitterParam
          }
          onPress={() => { }}
        />
        <GoodsButton
          buttonText={
            '•点击调用callFunction发送事件到js侧 size：' + callFunctionParam
          }
          onPress={() => {
            SampleTurboModule2.callFunction();
          }}
        />
      </View>
    </ScrollView>
  );
}

export default AppGoods;

const styles = StyleSheet.create({
  //创建样式
  container: {
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#E6E6E6',
  },
  textContainer: {
    justifyContent: 'center',
    backgroundColor: 'white',
    width: '90%',
    marginVertical: 10,
    marginHorizontal: 50,
    borderRadius: 10,
    padding: 15,
  },
  titleTextFont: {
    fontSize: 18,
    textAlign: 'left',
  },
  constantsFont: {
    width: '100%',
    marginTop: 5,
    padding: 10,
    borderRadius: 5,
    fontSize: 16,
    textAlign: 'left',
    alignSelf: 'flex-start',
    color: '#31525B',
    backgroundColor: 'white',
  },
});

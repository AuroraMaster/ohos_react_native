/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import * as React from 'react';
import { useState, useRef } from 'react';
import {
  StyleSheet,
  View,
  ScrollView,
  DeviceEventEmitter,
  Platform,
  Button
} from 'react-native';

import SampleTurboModule from '../basic/SampleTurboModule';
import SampleTurboModule2, {
  Fruit,
  ResultModel,
} from '../basic/SampleTurboModule2';
import SampleWorkerTurboModule from '../basic/SampleWorkerTurboModule';


const isCAPI =
  'rnohArchitecture' in Platform.constants &&
  Platform.constants.rnohArchitecture === 'C_API';

function App() {
  const [nativeCallbackValue, setNativeCallbackValue] = useState("");
  const [nativeAsyncCallbackValue, setNativeAsyncCallbackValue] = useState("");
  const [nativeWorkerCallbackValue, setNativeWorkerCallbackValue] = useState("");
  const [nativeWorkerAsyncCallbackValue, setNativeWorkerAsyncCallbackValue] = useState("");

  return (
    <ScrollView style={{ marginTop: 30 }}>
      <View style={styles.container}>
        <Button
          title={'•调用TurboModule回调方法：' + nativeCallbackValue}
          onPress={() => {
            SampleTurboModule.registerFunction(value => {
              setNativeCallbackValue(value);
            });
          }}
        />
        <Button
          title={
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
        <Button
          title="•调用原生console.log方法"
          onPress={() => {
            SampleTurboModule.rnLog('在Native中打印日志');
          }}
        />
        <Button
          title={'•调用TurboModule2的test方法'}
          onPress={() => {
            SampleTurboModule2.test();
          }}
        />
        <Button
          title={'•调用TurboModule2的getObject方法'}
          onPress={() => {
            SampleTurboModule2.getObject({ x: 100 });
          }}
        />
        <Button
          title={'•调用TurboModule2的getRequest方法'}
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
        <Button
          title={'•调用TurboModule2的eatFruit方法'}
          onPress={() => {
            const fruit: Fruit = {
              name: '大菠萝',
            };
            SampleTurboModule2.eatFruit(fruit);
          }}
        />
        <Button
          title={'•调用TurboModule2的checkPwd方法'}
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
        <Button
          title={'•调用WorkerTurboModule回调方法：' + nativeWorkerCallbackValue}
          onPress={() => {
            SampleWorkerTurboModule.registerFunction(value => {
              setNativeWorkerCallbackValue(value);
            });
          }}
        />
        <Button
          title={
            '•调用WorkerTurboModule异步回调方法：' + nativeWorkerAsyncCallbackValue
          }
          onPress={() => {
            SampleWorkerTurboModule.doAsyncJob(false).then(result => {
              setNativeWorkerAsyncCallbackValue(result);
            }).catch(err => {
              setNativeWorkerAsyncCallbackValue((err as Error).message);
            });
          }}
        />
        <Button
          title={
            '•在worker线程中调用原生console.log方法：'
          }
          onPress={() => {
            SampleWorkerTurboModule.rnLog("在Native的worker线程中打印日志")
          }}
        />
      </View>
    </ScrollView>
  );
}

export default App;

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

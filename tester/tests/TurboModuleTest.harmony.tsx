/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {
  SampleTurboModule,
  GeneratedSampleTurboModule,
  SampleWorkerTurboModule,
  CodegenLibSampleTurboModule,
} from 'react-native-sample-package';
import {
  SomeEnum1,
  SomeEnum2,
  SomeEnum3,
} from 'react-native-sample-package/src/specs/v2/NativeGeneratedSampleTurboModule';
import {Button, TestCase} from '../components';
import {ScrollView, Platform, DeviceEventEmitter} from 'react-native';

export function TurboModuleTest() {
  return (
    <TestSuite name="TurboModule">
      <TestSuite name="manual implementation">
        <TestCase.Logical
          itShould="return null when calling getNull()"
          fn={({expect}) => {
            expect(SampleTurboModule.getNull(null)).to.be.null;
          }}
        />
        <TestCase.Logical
          itShould="return [1, 2, 3] when calling getArray"
          fn={({expect}) => {
            expect(SampleTurboModule.getArray([1, 2, 3])).to.eql([1, 2, 3]);
          }}
        />
        <CommonTurboModuleTestCases sampleTurboModule={SampleTurboModule} />
      </TestSuite>
      <TestSuite name="generated">
        <CommonTurboModuleTestCases
          sampleTurboModule={GeneratedSampleTurboModule}
        />
        <TestCase.Logical
          itShould="get union value"
          fn={async ({expect}) => {
            expect(GeneratedSampleTurboModule.getUnionValue('foo')).to.be.eq(
              'foo',
            );
          }}
        />
        <TestCase.Logical
          itShould="support enums"
          fn={async ({expect}) => {
            const result = GeneratedSampleTurboModule.getEnum(
              SomeEnum1.FOO,
              SomeEnum2.FOO,
              SomeEnum3.FOO,
            );
            expect(result.enum1).to.be.eq(SomeEnum1.FOO);
            expect(result.enum2).to.be.eq(SomeEnum2.FOO);
            expect(result.enum3).to.be.eq(SomeEnum3.FOO);
          }}
        />
        <TestCase.Logical
          itShould="handle enums without specified values correctly"
          skip="RN codegen doesn't parse enums without explicit values correctly"
          fn={async ({expect}) => {
            const result = GeneratedSampleTurboModule.getEnum(
              SomeEnum1.FOO,
              SomeEnum2.FOO,
              SomeEnum3.FOO,
            );
            expect(result.hardcodedEnum1 === SomeEnum1.FOO).to.be.true;
          }}
        />
        <TestCase.Logical
          itShould="calculate fibonacci numbers on the Worker thread"
          fn={async ({expect}) => {
            const result =
              await GeneratedSampleTurboModule.calculateOnWorker(25);
            expect(result).to.equal(75025);
          }}
        />
      </TestSuite>
      <TestSuite name="WorkerTurboModule">
        <TestCase.Logical
          itShould="support workerTM 2 workerTM communication (WorkerRNInstance::getWorkerTurboModule)"
          fn={async ({expect}) => {
            expect(
              await SampleWorkerTurboModule.testCommunicatingWithOtherWorkerTurboModules(),
            ).to.be.true;
          }}
        />
        <TestCase.Logical
          itShould="support worker -> cpp communication (WorkerRNInstance::postMessageToCpp)"
          fn={async ({expect}) => {
            expect(
              await SampleWorkerTurboModule.testCommunicationBetweenWorkerAndCpp(),
            ).to.be.true;
          }}
        />
        <TestCase.Logical
          itShould="support WorkerRNInstance::getArchitecture"
          fn={({expect}) => {
            expect(SampleWorkerTurboModule.getArchitecture()).to.be.eq(
              (Platform.constants as any).rnohArchitecture,
            );
          }}
        />
        <TestCase.Logical
          itShould="not crash"
          fn={({}) => {
            SampleWorkerTurboModule.runComputeIntensiveTask();
          }}
        />
        <TestCase.Logical
          itShould="support async methods"
          fn={async ({expect}) => {
            const result = await SampleWorkerTurboModule.runAsyncTask();
            expect(result).to.be.eq(42);
          }}
        />
        <TestCase.Example
          modal
          itShould="not impact the performance of ScrollView when running intensive task">
          <ScrollView
            style={{height: 256}}
            contentContainerStyle={{height: 384, justifyContent: 'center'}}>
            <Button
              label="Run task"
              onPress={() => {
                SampleWorkerTurboModule.runComputeIntensiveTask();
              }}
            />
          </ScrollView>
        </TestCase.Example>
        <TestCase.Logical
          itShould="emit device event"
          fn={async ({expect}) => {
            const result = await new Promise(resolve => {
              const subscription = DeviceEventEmitter.addListener(
                'WORKER_TURBO_MODULE_TEST',
                p => {
                  resolve(p);
                  subscription.remove();
                },
              );
              SampleWorkerTurboModule.emitDeviceEvent(
                'WORKER_TURBO_MODULE_TEST',
                42,
              );
            });
            expect(result).to.be.eq(42);
          }}
        />
        <TestSuite name="runOnUIThread">
          <TestCase.Logical
            itShould="support running tasks on UI thread from worker"
            fn={async ({expect}) => {
              const result =
                await SampleWorkerTurboModule.getRNVersionFromUIContext('RN ');
              const rnVersion = Platform.constants.reactNativeVersion;
              expect(result).to.be.eq(
                `RN ${rnVersion.major}.${rnVersion.minor}.${rnVersion.patch}`,
              );
            }}
          />
          <TestCase.Example itShould="show alert">
            <Button
              label="Show Alert"
              onPress={() => {
                SampleWorkerTurboModule.showAlert();
              }}
            />
          </TestCase.Example>
        </TestSuite>
      </TestSuite>
      <TestSuite name="generated by codegen-lib-harmony">
        <TestCase.Logical
          itShould="not crash when calling method of Turbo Module partially generated by this command"
          fn={({expect}) => {
            expect(CodegenLibSampleTurboModule.getNumber()).to.be.eq(42);
          }}
        />
      </TestSuite>
    </TestSuite>
  );
}

function CommonTurboModuleTestCases({
  sampleTurboModule,
}: {
  sampleTurboModule: typeof GeneratedSampleTurboModule;
}) {
  return (
    <>
      <TestCase.Logical
        itShould="return null when calling voidFunc()"
        fn={({expect}) => {
          expect(sampleTurboModule.voidFunc()).to.be.null;
        }}
      />
      <TestCase.Logical
        itShould="return true when calling getBool(true)"
        fn={({expect}) => {
          expect(sampleTurboModule.getBool(true)).to.be.true;
        }}
      />

      <TestCase.Logical
        itShould="return { x: { y: 1 } } when calling getObject"
        fn={({expect}) => {
          expect(sampleTurboModule.getObject({x: {y: 1}})).to.eql({
            x: {y: 1},
          });
        }}
      />
      <TestCase.Logical
        itShould="call the callback and providing string argument"
        fn={async ({expect}) => {
          const promise = new Promise<string>(resolve => {
            sampleTurboModule.registerFunction(resolve);
          });
          expect(typeof (await promise)).to.be.eq('string');
        }}
      />
      <TestCase.Logical
        itShould="handle async jobs"
        fn={async ({expect}) => {
          const result = await sampleTurboModule?.doAsyncJob(true);
          expect(typeof result).to.be.eq('string');
        }}
      />
      <TestCase.Logical
        itShould="handle errors in async jobs"
        fn={async ({expect}) => {
          let errMsg: string | undefined;
          try {
            await sampleTurboModule?.doAsyncJob(false);
          } catch (err) {
            errMsg = (err as Error).message;
          }

          expect(errMsg).to.be.eq('rejected on native side');
        }}
      />
      <TestCase.Logical
        itShould="get an array asynchronously"
        fn={async ({expect}) => {
          expect(await sampleTurboModule.getPromisedArray()).to.be.eql([
            1, 2, 3,
          ]);
        }}
      />
    </>
  );
}

/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {Text, View} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import React from 'react';
import {Button, TestCase} from '../components';
import {useEnvironment} from '../contexts';
// @ts-ignore
import resolveAssetSource from 'react-native/Libraries/Image/resolveAssetSource';

const RCTNetworking =
  require('react-native/Libraries/Network/RCTNetworking').default;

const FILE_URI = '/data/storage/el2/base/files/testFile.txt';
const SVG_TEXT = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="-11.5 -10.23174 23 20.46348">
  <title>React Logo</title>
  <circle cx="0" cy="0" r="2.05" fill="#61dafb"/>
  <g stroke="#61dafb" stroke-width="1" fill="none">
    <ellipse rx="11" ry="4.2"/>
    <ellipse rx="11" ry="4.2" transform="rotate(60)"/>
    <ellipse rx="11" ry="4.2" transform="rotate(120)"/>
  </g>
</svg>
`;

export const NetworkingTest = () => {
  const {
    env: {isConnectedToInternet},
  } = useEnvironment();

  const noInternetSkipMsg = isConnectedToInternet
    ? undefined
    : 'Internet connection required';

  const canFetch = async (url: string) => {
    try {
      const response = await fetch(url);
      await response.json();
      return true;
    } catch (error) {
      return false;
    }
  };

  return (
    <TestSuite name="Networking">
      <TestSuite name="Fetch API">
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="download data"
          fn={async ({expect}) => {
            const response = await fetch('https://reactnative.dev/movies.json');
            const result = await response.text();
            const parsed = JSON.parse(result);
            expect(parsed.title).to.be.eq('The Basics - Networking');
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="download and parse JSON data"
          fn={async ({expect}) => {
            const response = await fetch('https://reactnative.dev/movies.json');
            const result = await response.json();
            expect(result.title).to.be.eq('The Basics - Networking');
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="download data to an ArrayBuffer"
          fn={async ({expect}) => {
            const response = await fetch('https://reactnative.dev/movies.json');
            const result = await response.arrayBuffer();
            expect(result.byteLength).to.be.eq(458);
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="download data to a blob"
          fn={async ({expect}) => {
            const response = await fetch('https://reactnative.dev/movies.json');
            const result = await response.blob();
            expect(result.size).to.be.eq(458);
          }}
        />
        <TestCase.Logical
          itShould="fetch local assets correctly (test needs to be run without metro)"
          fn={async ({expect}) => {
            const uri = require('../assets/react-icon.svg');
            const source = resolveAssetSource(uri);
            const response = await fetch(source.uri);
            const result = await response.text();
            expect(result).to.be.eq(SVG_TEXT);
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="correctly send requests with FormData"
          fn={async ({expect}) => {
            const formData = new FormData();
            formData.append('name', 'test');
            formData.append('surname', 'test2');
            formData.append('boolean', true);
            formData.append('number', 72);
            const response = await fetch('https://httpbin.org/post', {
              method: 'POST',
              body: formData,
            });
            const result = await response.json();
            expect(result.form.name).to.be.eq('test');
            expect(result.form.boolean).to.be.eq('true');
            expect(result.form.number).to.be.eq('72');
            expect(result.form.surname).to.be.eq('test2');
          }}
        />
        <TestCase.Manual<undefined | FormData>
          initialState={undefined}
          skip={{android: true, harmony: noInternetSkipMsg ?? false}}
          itShould="correctly send a text file in FormData"
          arrange={({setState}) => (
            <>
              <Text>
                {
                  'To run this test, you need to create a file at the correct path on the phone. You can run this command in the hdc shell: echo "Test file content" >> /data/app/el2/100/base/com.rnoh.tester/files/testFile.txt'
                }
              </Text>
              <Button
                label="Run"
                onPress={() => {
                  let formData = new FormData();
                  formData.append('file', {
                    uri: FILE_URI,
                    type: 'text/plain',
                    name: 'testFile.txt',
                  });
                  formData.append('text', 'test text field');
                  setState(formData);
                }}
              />
            </>
          )}
          assert={async ({expect, state: formData}) => {
            try {
              let response = await fetch('https://httpbin.org/post', {
                method: 'POST',
                body: formData,
                headers: {
                  'Content-Type': 'multipart/form-data',
                },
              });
              let responseJson = await response.json();
              expect(responseJson.form.text).to.be.eq('test text field');
              expect(responseJson.files.file).to.exist;
            } catch (error) {
              if (error instanceof Error) {
                expect.fail(error.message);
              } else {
                throw error;
              }
            }
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="correctly read response headers"
          fn={async ({expect}) => {
            const response = await fetch(
              'https://httpbin.org/response-headers?first-header=first&second-header=second',
            );
            const firstHeader = response.headers.get('first-header');
            const secondHeader = response.headers.get('second-header');
            expect(firstHeader).to.be.eq('first');
            expect(secondHeader).to.be.eq('second');
          }}
        />
        <TestCase.Logical 
          skip={noInternetSkipMsg}
          itShould="correctly send params with special symbols"
          fn={async ({expect}) => {
            const response = await fetch(
              'https://httpbin.org/get?url=https%3A%2F%2Fexample.org%2Fuser%2F12345%3Fref%3Dabc%252Fdef%253A',
            );
            const data = await response.json();
            expect(data.args.url).to.be.eq('https://api.example.org/v1/qr-generator?url=https://example.org/user/12345?ref=abc%2Fdef:');
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="fail on bad url"
          fn={async ({expect}) => {
            const received = await canFetch(
              'https://reactnative.dev/bad_url.json',
            );
            expect(received).to.be.false;
          }}
        />
      </TestSuite>
      <TestSuite name="XmlHttpRequest">
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="verify if correct data is received in onprogress event"
          fn={async ({expect}) => {
            // emojis use 4 bytes so theres a higher chance of them getting cut when loading incrementally
            const emoji = '😀';
            const longStr = emoji.repeat(10000); //the value needs to be high enough so that several onprogress events are triggered
            let responseText = '';

            const xhr = new XMLHttpRequest();
            xhr.open('POST', 'https://postman-echo.com/post', true);
            xhr.setRequestHeader('Content-Type', 'text/plain; charset=UTF-8');

            await new Promise<void>((resolve, reject) => {
              xhr.onprogress = function (this, ev) {
                try {
                  expect(ev.total).to.not.be.null.and.not.be.undefined;
                  expect(ev.loaded).to.not.be.null.and.not.be.undefined;
                } catch (error) {
                  reject(error);
                }
                responseText = xhr.responseText;
              };

              xhr.onload = function () {
                const response = JSON.parse(responseText);
                try {
                  expect(response.data).to.be.eq(longStr);
                  resolve();
                } catch (error) {
                  reject(error);
                }
              };

              xhr.onerror = function () {
                reject('Request failed');
              };

              xhr.send(longStr);
            });
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="verify if correct data is received in onupload progress event"
          fn={async ({expect}) => {
            let longStr = 'a'.repeat(100000); //the value needs to be high enough so that several events are triggered
            const xhr = new XMLHttpRequest();
            xhr.open('POST', 'https://postman-echo.com/post', true);
            xhr.setRequestHeader('Content-Type', 'text/plain; charset=UTF-8');
            await new Promise<void>((resolve, reject) => {
              xhr.upload.onprogress = function (event) {
                try {
                  expect(event.total).to.equal(longStr.length);
                  expect(event.loaded).to.lessThanOrEqual(longStr.length);
                } catch (error) {
                  reject(error);
                }
              };

              xhr.onloadend = function () {
                resolve();
              };

              xhr.onerror = function () {
                throw reject('Upload failed');
              };
              xhr.send(longStr);
            });
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="verify that correct events are emitted"
          fn={async ({expect}) => {
            let longStr = 'a'.repeat(100000); //the value needs to be high enough so that several events are triggered
            const xhr = new XMLHttpRequest();
            xhr.open('POST', 'https://postman-echo.com/post', true);
            xhr.setRequestHeader('Content-Type', 'text/plain; charset=UTF-8');
            const events = await new Promise<string[]>((resolve, reject) => {
              const allEvents: string[] = [];
              xhr.upload.onprogress = function () {
                allEvents.push('upload progress');
              };
              xhr.onprogress = function () {
                allEvents.push('progress');
              };
              xhr.onload = function () {
                allEvents.push('load');
              };
              xhr.onloadend = function () {
                allEvents.push('loadend');

                resolve(allEvents);
              };
              xhr.onreadystatechange = function () {
                allEvents.push('readystatechange');
              };

              xhr.onerror = function () {
                throw reject('Upload failed');
              };
              xhr.send(longStr);
            });
            expect(events).to.contain('upload progress');
            expect(events).to.contain('progress');
            expect(events).to.contain('load');
            expect(events).to.contain('loadend');
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="verify that the abort event gets triggered"
          fn={async ({expect}) => {
            const event = await new Promise<ProgressEvent<EventTarget>>(
              (resolve, reject) => {
                const xhr = new XMLHttpRequest();
                xhr.open('POST', 'invalidurlr.jpg', true);
                xhr.onabort = function (abortEvent) {
                  resolve(abortEvent);
                };
                xhr.onerror = function () {
                  reject('Request should have been aborted');
                };
                xhr.onload = function () {
                  reject('Request should have been aborted');
                };
                xhr.send();
                xhr.abort();
              },
            );
            expect(event.type).to.equal('abort');
          }}
        />
        <TestCase.Logical
          skip={{android: true, harmony: false}}
          itShould="verify that the timeout event gets triggered"
          fn={async ({expect}) => {
            const event = await new Promise<ProgressEvent<EventTarget>>(
              (resolve, reject) => {
                const xhr = new XMLHttpRequest();
                xhr.open('POST', 'https://postman-echo.com/delay/10', true);
                xhr.timeout = 1;
                xhr.ontimeout = function (timeoutEvent) {
                  resolve(timeoutEvent);
                };
                xhr.onerror = function () {
                  reject('Request should have timed out');
                };
                xhr.onload = function () {
                  reject('Request should have timed out');
                };
                xhr.send();
              },
            );
            expect(event.type).to.equal('timeout');
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="verify that cookies are saved and sent correctly (withCredentials)"
          fn={async ({expect}) => {
            await sendCookieRequest(
              'https://httpbin.org/cookies/set?theme=light',
              true,
            );
            await sendCookieRequest(
              'https://httpbin.org/cookies/set?theme=dark&secondattribute=test',
              true,
            );

            const thirdCookies = (await sendCookieRequest(
              'https://httpbin.org/cookies',
              true,
            )) as Record<string, unknown>;
            expect(thirdCookies.theme).to.equal('dark');
            expect(thirdCookies.secondattribute).to.equal('test');

            const fourthCookies = await sendCookieRequest(
              'https://httpbin.org/cookies',
              false,
            );
            expect(fourthCookies).to.be.empty;
          }}
        />
        <TestCase.Manual
          skip={noInternetSkipMsg}
          initialState={{cookies: {}}}
          arrange={({setState}) => {
            const requestSetCookies = async () => {
              const xhr = new XMLHttpRequest();
              xhr.open(
                'GET',
                'https://httpbin.org/cookies/set?testcookie=storedcookie',
                true,
              );
              xhr.send();
            };

            const getCookies = async () => {
              const xhr = new XMLHttpRequest();
              xhr.open('GET', 'https://httpbin.org/cookies', true);
              xhr.onloadend = function () {
                const response = JSON.parse(xhr.responseText);
                setState({cookies: response.cookies});
              };
              xhr.send();
            };
            const clearCookies = async () => {
              RCTNetworking.clearCookies(() => {});
            };

            return (
              <>
                <View>
                  <Text style={{fontSize: 16, marginBottom: 10}}>
                    <Text style={{fontSize: 16, marginBottom: 10}}>
                      1. Press "Clear cookies".{'\n'}
                      2. Press "Set cookies".{'\n'}
                      4. Close and reopen the app.{'\n'}
                      5. Press "Get cookies".
                    </Text>
                  </Text>
                  <Button onPress={requestSetCookies} label="Set cookies" />
                  <Button onPress={getCookies} label="Get cookies" />
                  <Button onPress={clearCookies} label="Clear cookies" />
                </View>
              </>
            );
          }}
          assert={({state, expect}) => {
            expect(
              (state.cookies as {testcookie?: string}).testcookie,
            ).to.equal('storedcookie');
          }}
          modal
          itShould="store cookies in a persistent way"
        />
      </TestSuite>
      <TestSuite name="WebSocket">
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="receive back submitted data"
          fn={async ({expect}) => {
            const result = await new Promise<{type: string; payload: any}[]>(
              (resolve, reject) => {
                // connect to Postman's echo server
                const ws = new WebSocket('wss://ws.postman-echo.com/raw');
                const events: {type: string; payload: unknown}[] = [];

                ws.onopen = () => {
                  events.push({type: 'onOpen', payload: undefined});
                  ws.send('something');
                };

                ws.onmessage = e => {
                  events.push({type: 'onMessage', payload: e});
                  setTimeout(() => {
                    ws.close();
                  }, 1000);
                };

                ws.onerror = e => {
                  events.push({type: 'onError', payload: e});
                  reject(e.message);
                };

                ws.onclose = e => {
                  events.push({type: 'onClose', payload: e});
                  resolve(events);
                };
              },
            );
            expect(result.length).to.be.eq(3);
            expect(result[0].type).to.be.eq('onOpen');
            expect(result[1].type).to.be.eq('onMessage');
            expect(result[2].type).to.be.eq('onClose');
            expect(result[1].payload?.data).to.be.eq('something');
          }}
        />
        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="send and receive arraybuffer through websocket"
          fn={async ({expect}) => {
            const input = 'Hello World - arraybuffer!';

            const result = await new Promise<string>((resolve, reject) => {
              const ws = new WebSocket('wss://echo.websocket.org/');
              let output: string = '';

              // @ts-ignore
              ws.binaryType = 'arraybuffer';
              ws.onopen = () => {
                const buffer = stringToArrayBuffer(input);
                ws.send(buffer);
              };

              ws.onmessage = event => {
                if (event.data instanceof ArrayBuffer) {
                  output = arrayBufferToStr(event.data);

                  ws.close();
                }
              };

              ws.onerror = error => {
                reject(error);
              };

              ws.onclose = () => {
                resolve(output);
              };
            });

            expect(result).to.be.eq(input);
          }}
        />

        <TestCase.Logical
          skip={noInternetSkipMsg}
          itShould="send and receive blob through websocket"
          fn={async ({expect}) => {
            const input = 'Hello World - blob!';
            const result = await new Promise<Blob | undefined>(
              (resolve, reject) => {
                let output: Blob | undefined;
                const ws = new WebSocket('wss://echo.websocket.org/');

                // @ts-ignore
                ws.binaryType = 'blob';

                ws.onopen = () => {
                  const blob = new Blob([input], {
                    type: 'text/plain',
                    lastModified: Date.now(),
                  });
                  ws.send(blob);
                };

                ws.onmessage = event => {
                  if (event.data instanceof Blob) {
                    output = event.data;
                    ws.close();
                  }
                };

                ws.onerror = error => {
                  reject(error.message);
                };

                ws.onclose = () => {
                  resolve(output);
                };
              },
            );
            expect(result?.size ?? 0).to.be.eq(input.length);
          }}
        />
      </TestSuite>
    </TestSuite>
  );
};

function arrayBufferToStr(buf: ArrayBuffer | ArrayBufferLike) {
  return String.fromCharCode(...new Uint8Array(buf));
}

function stringToArrayBuffer(str: string) {
  const buffer = new ArrayBuffer(str.length);
  const bufferView = new Uint8Array(buffer);
  for (let i = 0; i < str.length; i++) {
    bufferView[i] = str.charCodeAt(i);
  }
  return buffer;
}

const sendCookieRequest = (
  url: string,
  withCredentials: boolean,
): Promise<{cookies: Record<string, unknown>}> => {
  return new Promise((resolve, reject) => {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.withCredentials = withCredentials;
    xhr.onloadend = () => {
      try {
        const response = JSON.parse(xhr.responseText);
        resolve(response.cookies);
      } catch (error) {
        reject(error);
      }
    };
    xhr.send();
  });
};

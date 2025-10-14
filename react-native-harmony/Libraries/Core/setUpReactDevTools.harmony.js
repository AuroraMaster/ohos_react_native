/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

'use strict';

if (__DEV__) {
  let isWebSocketOpen = false;
  let ws = null;

  const reactDevTools = require('react-devtools-core');
  const connectToDevTools = () => {
    if (ws !== null && isWebSocketOpen) {
      // If the DevTools backend is already connected, don't recreate the WebSocket.
      // This would break the connection.
      // If there isn't an active connection, a backend may be waiting to connect,
      // in which case it's okay to make a new one.
      return;
    }

    // not when debugging in chrome
    // TODO(t12832058) This check is broken
    if (!window.document) {
      const AppState = require('react-native/Libraries/AppState/AppState');
      const getDevServer = require('react-native/Libraries/Core/Devtools/getDevServer');

      // Don't steal the DevTools from currently active app.
      // Note: if you add any AppState subscriptions to this file,
      // you will also need to guard against `AppState.isAvailable`,
      // or the code will throw for bundles that don't have it.
      const isAppActive = () => AppState.currentState !== 'background';

      // RNOH patch: wait until appstate is active before connecting
      if (!isAppActive()) {
        const subscription = AppState.addEventListener('change', () => {
          if (isAppActive()) {
            connectToDevTools();
            subscription.remove();
          }
        });
        return;
      }

      // Get hostname from development server (packager)
      const devServer = getDevServer();
      const host = devServer.bundleLoadedFromServer
        ? devServer.url
            .replace(/https?:\/\//, '')
            .replace(/\/$/, '')
            .split(':')[0]
        : 'localhost';

      // Read the optional global variable for backward compatibility.
      // It was added in https://github.com/facebook/react-native/commit/bf2b435322e89d0aeee8792b1c6e04656c2719a0.
      const port =
        window.__REACT_DEVTOOLS_PORT__ != null
          ? window.__REACT_DEVTOOLS_PORT__
          : 8097;

      const WebSocket = require('react-native/Libraries/WebSocket/WebSocket');
      ws = new WebSocket('ws://' + host + ':' + port);
      ws.addEventListener('close', event => {
        isWebSocketOpen = false;
      });
      ws.addEventListener('open', event => {
        isWebSocketOpen = true;
      });

      const ReactNativeStyleAttributes = require('react-native/Libraries/Components/View/ReactNativeStyleAttributes');
      const devToolsSettingsManager = require('react-native/Libraries/DevToolsSettings/DevToolsSettingsManager');

      reactDevTools.connectToDevTools({
        isAppActive,
        resolveRNStyle: require('react-native/Libraries/StyleSheet/flattenStyle'),
        nativeStyleEditorValidAttributes: Object.keys(
          ReactNativeStyleAttributes,
        ),
        websocket: ws,
        devToolsSettingsManager,
      });
    }
  };

  const RCTNativeAppEventEmitter = require('react-native/Libraries/EventEmitter/RCTNativeAppEventEmitter');
  RCTNativeAppEventEmitter.addListener('RCTDevMenuShown', connectToDevTools);
  connectToDevTools(); // Try connecting once on load
}

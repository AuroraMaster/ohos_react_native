/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNOHLogger } from './RNOHLogger';
import type { DevToolsController } from "./DevToolsController"
import type { DevMenu } from "./types"
import { ReconnectingWebSocket } from './ReconnectingWebSocket';

export interface JSPackagerClientConfig {
  host: string,
  port: number | string,
}

export class JSPackagerClient {
  private webSocket: ReconnectingWebSocket;
  private logger: RNOHLogger;

  constructor(logger: RNOHLogger, private devToolsController: DevToolsController, private devMenu:DevMenu) {
    this.logger = logger.clone("JSPackagerClient");
  }

  public connectToMetroMessages(config: JSPackagerClientConfig) {
    const url = this.buildUrl(config);

    const onMessage = (data) => {
      if (typeof data === "string") {
        const message = JSON.parse(data);
        switch (message.method) {
          case "devMenu":
            this.devMenu.show()
            break;
          case "reload":
            this.devToolsController.reload(undefined)
            break;
          default:
            this.logger.warn(`Unsupported action: ${message.method}`)
        }
      }
    }

    const onDisconnected = (err) => {
      if (err) {
        this.logger.error("Websocket connection failed, err: " + JSON.stringify(err));
      }
    }

    this.webSocket = new ReconnectingWebSocket(url, { onMessage, onDisconnected })
  }

  public async onDestroy() {
    this.webSocket.close()
  }

  private buildUrl(config: JSPackagerClientConfig): string {
    return `ws://${config.host}:${config.port}/message`;
  }
}
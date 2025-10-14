/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import webSocket from '@ohos.net.webSocket';
import { RNOHError } from './ts';

type OnMessageCallback = (message: string | ArrayBuffer) => void;
type OnDisconnectedCallback = (err: unknown) => void;

const RECONNECT_DELAY_MS: number = 2000;

interface ReconnectingWebSocketOptions {
  onMessage: OnMessageCallback,
  onDisconnected?: OnDisconnectedCallback,
}

export class ReconnectingWebSocket {
  private ws: webSocket.WebSocket | undefined
  private silenceErrors: boolean = false
  private closed: boolean = false
  private onMessage?: OnMessageCallback
  private onDisconnected?: OnDisconnectedCallback

  constructor(
    private url: string,
    options: ReconnectingWebSocketOptions) {
    this.onMessage = options.onMessage;
    this.onDisconnected = options.onDisconnected;
    this.connect();
  }

  public sendMessage(message: string | ArrayBuffer) {
    if (!this.ws) {
      throw new RNOHError({ whatHappened: "WebSocket is disconnected", howCanItBeFixed: [] });
    }
    this.ws.send(message);
  }

  public close() {
    this.closed = true;
    this.onMessage = undefined;
    this.ws?.close({
      code: 1000,
      reason: "End of session",
    }, (err) => this.onClosed(err));
    this.ws = undefined;
  }

  private connect() {
    const ws = webSocket.createWebSocket();
    ws.connect(this.url, (err, connected) => {
    });
    ws.on('open', () => {
      this.ws = ws;
      this.silenceErrors = false;
    })
    ws.on("error", (err) => {
      this.onError(err)
    });
    ws.on('close', (err) => {
      this.onClosed(err)
    })
    ws.on("message", (_err, data) => this.onMessage?.(data));
  }

  private reconnect() {
    if (this.closed) {
      throw new RNOHError({ whatHappened: "Can't reconnect a closed ReconnectingWebSocket", howCanItBeFixed: [] });
    }

    setTimeout(() => {
      if (!this.closed) {
        this.connect()
      }
    }, RECONNECT_DELAY_MS);
  }

  private onError(err: unknown) {
    this.ws = undefined;
    if (!this.closed) {
      if (!this.silenceErrors) {
        this.onDisconnected?.(err);
        this.silenceErrors = true;
      }
      this.reconnect();
    }
  }

  private onClosed(err: unknown) {
    this.ws = undefined;
    if (!this.closed) {
      this.onDisconnected?.(err);
      this.reconnect();
    }
  }
}
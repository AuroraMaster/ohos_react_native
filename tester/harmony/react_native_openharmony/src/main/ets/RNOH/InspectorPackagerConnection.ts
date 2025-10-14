/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import webSocket from '@ohos.net.webSocket';
import { RNOHLogger } from './RNOHLogger';
import { BusinessError } from '@ohos.base';
import { InspectorPage, InspectorInstance, InspectorLocalConnection, InspectorRemoteConnection } from './types';

class RemoteConnectionImpl implements InspectorRemoteConnection {
  constructor(private packagerConnection: InspectorPackagerConnection, private pageId: number) {
  }

  onMessage(message: string) {
    this.packagerConnection.sendWrappedEvent(this.pageId, message);
  }

  onDisconnect() {
    this.packagerConnection.removeConnection(this.pageId);
    this.packagerConnection.sendEvent("disconnect", { pageId: this.pageId });
  }
}

const RECONNECT_DELAY_MS = 2000;

// This is a port of the Android impl, at
// ReactAndroid/src/main/java/com/facebook/react/devsupport/InspectorPackagerConnection.java
export class InspectorPackagerConnection {
  private localConnectionByPageId: Map<number, InspectorLocalConnection> = new Map();
  private socket: webSocket.WebSocket;
  private isClosed: boolean = false;
  private logger: RNOHLogger;

  constructor(private url: string, logger: RNOHLogger, private inspectorInstance: InspectorInstance) {
    if (url.startsWith("http")) {
      url = url.replace("http", "ws");
    }
    this.url = url;
    this.logger = logger.clone("InspectorPackagerConnection");
  }

  // WebSocket lifecycle:
  public connect() {
    if (this.isClosed) {
      this.logger.error("Illegal state: Can't reconnect after having previously been closed.");
      return;
    }

    const socket = webSocket.createWebSocket();
    socket.on("open", (_err) => this.onOpen());
    socket.on("message", (_err, message) => this.onMessage(message))
    socket.on("error", (err) => this.onError(err))
    socket.on("close", (_err, closeResult) => this.onClose(closeResult))

    socket.connect(this.url, (err) => {
      if (err) {
        this.onError(err)
      }
    });
    this.socket = socket;
  }

  private reconnect() {
    if (this.isClosed) {
      this.logger.error("Illegal state: Can't reconnect after having previously been closed.");
      return;
    }

    setTimeout(() => {
      if (!this.isClosed) {
        this.connect();
      }
    }, RECONNECT_DELAY_MS)
  }

  public closeAllConnections() {
    this.localConnectionByPageId.forEach((connection) => connection.disconnect());
    this.localConnectionByPageId.clear();
  }

  public closeQuietly() {
    this.isClosed = true;
    this.disposeWebSocket();
  }

  private abort() {
    this.closeAllConnections();
    this.disposeWebSocket();
  }

  private disposeWebSocket() {
    this.socket?.close({ code: 1000, reason: "End of session" });
    this.socket = undefined;
  }

  // WebSocket handlers:
  private onOpen() {
  }

  private onMessage(message: string | ArrayBuffer) {
    if (typeof message !== "string") {
      this.logger.warn("Unrecognized inspector message");
      return;
    }

    let parsed: Record<string, unknown>
    try {
      parsed = JSON.parse(message);
    } catch {
      this.logger.warn(`Unrecognized inspector message, string was not valid JSON: ${message}`);
      return;
    }

    const { event, payload } = parsed;
    switch (event) {
      case "getPages": {
        this.sendEvent("getPages", this.getPages());
        break;
      }
      case "wrappedEvent": {
        this.handleWrappedEvent(payload as any);
        break;
      }
      case "connect": {
        this.handleConnect(payload as any);
        break;
      }
      case "disconnect": {
        this.handleDisconnect(payload as any);
        break;
      }
      default: {
        this.logger.error(`Unknown event: ${event}`);
      }
    }
  }

  private onClose(_closeResult: webSocket.CloseResult) {
    this.socket = undefined;
    this.closeAllConnections();
    if (!this.isClosed) {
      this.reconnect();
    }
  }

  private onError(_err: BusinessError) {
    if (this.socket) {
      this.abort();
    }
    if (!this.isClosed) {
      this.reconnect();
    }
  }

  // Packager connection handlers:
  private handleConnect(payload: Record<string, unknown>) {
    let { pageId } = payload;
    if (typeof pageId === 'string') {
      pageId = Number(pageId)
    }
    if (typeof pageId !== 'number' || Number.isNaN(pageId)) {
      this.logger.error("Invalid 'connect' payload: 'pageId' is missing");
      return;
    }

    let existingConnection: InspectorLocalConnection;
    if (existingConnection = this.localConnectionByPageId.get(pageId)) {
      this.localConnectionByPageId.delete(pageId);
      existingConnection.disconnect();
      this.logger.warn("Already connected: " + pageId);
    }

    const remoteConnection = new RemoteConnectionImpl(this, pageId);
    const localConnection = this.inspectorInstance.connect(pageId, remoteConnection);
    this.localConnectionByPageId.set(pageId, localConnection);
  }

  private handleDisconnect(payload: Record<string, unknown>) {
    let { pageId } = payload;
    if (typeof pageId === 'string') {
      pageId = Number(pageId)
    }
    if (typeof pageId !== 'number' || Number.isNaN(pageId)) {
      this.logger.error("Invalid 'connect' payload: 'pageId' is missing");
      return;
    }

    const inspectorConnection = this.localConnectionByPageId.get(pageId);
    if (inspectorConnection) {
      this.removeConnection(pageId);
      inspectorConnection.disconnect();
    }
  }

  private handleWrappedEvent(payload: Record<string, unknown>) {
    let { pageId, wrappedEvent } = payload;
    if (typeof pageId === 'string') {
      pageId = Number(pageId)
    }
    if (typeof pageId !== 'number' || Number.isNaN(pageId)) {
      this.logger.error("Invalid 'connect' payload: 'pageId' is missing");
      return;
    }

    const inspectorConnection = this.localConnectionByPageId.get(pageId);
    if (!inspectorConnection) {
      this.logger.warn(`Not connected to page: ${pageId}, failed trying to handle event: ${wrappedEvent}`)
      return;
    }
    inspectorConnection.sendMessage(wrappedEvent);
  }

  // Sending methods:
  public sendEventToAllConnections(event: string) {
    this.localConnectionByPageId.forEach(connection => connection.sendMessage(event));
  }

  public sendWrappedEvent(pageId: number, wrappedEvent: string) {
    this.sendEvent("wrappedEvent", { pageId, wrappedEvent });
  }

  public sendEvent(event: string, payload: Object) {
    const message = JSON.stringify({ event, payload });
    this.socket?.send(message);
  }

  // Business logic:
  public isConnected() {
    return this.socket !== undefined;
  }

  private getPages(): InspectorPage[] {
    return this.inspectorInstance.getPages();
  }

  public removeConnection(pageId: number) {
    this.localConnectionByPageId.delete(pageId);
  }
}
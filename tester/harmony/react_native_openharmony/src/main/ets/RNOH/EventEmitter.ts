/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export class EventEmitter<TArgsByEventType extends Record<string, unknown[]>> {
  private listenersByEventType: Partial<{
    [K in keyof TArgsByEventType]: Set<(...args: TArgsByEventType[K]) => void>;
  }> = {};

  public subscribe<TEventType extends keyof TArgsByEventType>(
    eventType: TEventType,
    listener: (...args: TArgsByEventType[TEventType]) => void,
  ): () => void {
    if (!this.listenersByEventType[eventType]) {
      this.listenersByEventType[eventType] = new Set();
    }
    this.listenersByEventType[eventType]!.add(listener);
    return () => {
      this.listenersByEventType[eventType]!.delete(listener);
    };
  }

  public emit<TEventType extends keyof TArgsByEventType>(
    eventType: TEventType,
    ...args: TArgsByEventType[TEventType]
  ): void {
    this.listenersByEventType[eventType]?.forEach(listener => {
      listener(...args);
    });
  }
}

/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { CodegenError } from './CodegenError';

export function createEventNameFromEventHandlerName(
  eventHandlerName: string
): string {
  const eventName = eventHandlerName.replace('on', '');
  if (eventName.length === 0) {
    throw new CodegenError({
      whatHappened: `Received invalid event handler name: ${eventHandlerName}`,
      whatCanUserDo: [
        'Change your event listener name to match naming convention "onEventName" e.g. "onPress"',
      ],
    });
  }
  return eventName.charAt(0).toLowerCase() + eventName.slice(1);
}

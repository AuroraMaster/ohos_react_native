/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { useEffect } from 'react';

export function Effect({
  onMount,
  children = null,
}: {
  onMount: () => Promise<() => void> | (() => void) | void | Promise<void>;
  children?: any;
}) {
  useEffect(() => {
    let cleanUp: () => void = () => { };
    (async () => {
      const cleanUp_ = await onMount();
      if (cleanUp_) {
        cleanUp = cleanUp_;
      }
    })();
    return () => {
      cleanUp();
    };
  }, []);
  return children;
}

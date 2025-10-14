/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {useState} from 'react';

export function StateKeeper<T>(props: {
  initialValue: T;
  renderContent: (
    value: T | undefined,
    setValue: React.Dispatch<React.SetStateAction<T>>,
  ) => void;
}) {
  const [value, setValue] = useState<T>(props.initialValue);

  return <>{props.renderContent(value, setValue)}</>;
}

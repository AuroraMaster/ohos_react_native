/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {RefObject, useRef} from 'react';

export function Ref<T>({render}: {render: (ref: RefObject<T>) => any}) {
  const ref = useRef<T>(null);
  return render(ref);
}

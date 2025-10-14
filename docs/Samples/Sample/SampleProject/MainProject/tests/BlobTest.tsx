/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TestCase, TestSuite } from '@rnoh/testerino';
import React from 'react';

export const BlobTest = () => {
  return (
    <TestSuite name="Blob">
      <TestCase
        itShould="return blob size and type correctly"
        fn={async ({ expect }) => {
          const blob = new Blob(['aaabbbccc'], {
            type: 'text/plain',
            lastModified: Date.now(),
          });
          expect(blob.size).to.be.eq(9); //each character is 1 byte
          expect(blob.type).to.be.eq('text/plain');
        }}
      />
    </TestSuite>
  );
};

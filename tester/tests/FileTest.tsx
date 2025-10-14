/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../components';

const CHINESE_SELECTED_CHARACTERS = `
一 乙 二 十 丁 厂 七 卜 人 入 八 九 几 儿 了 力 乃 又
三 于 干 亏 士 工 土 才 寸 下 大 与 上 小 口 巾 山
千 乞 川 亿 个 勺 久 凡 及 夕 丸 么 广 亡 门 义 之 尸
己 已 子 卫 也 女 飞 刃 习 叉 马 乡 丰 王 井 开 夫 天
无 元 专 云 扎 艺 木 五 支 厅 不 太 犬 区 历 尤 友 匹
车 巨 牛 羊 网 羽 办 事 年 白 上 皮 化 百 耕 分 特 电
京 斯 世 气
`;

export const FileTest = () => {
  return (
    <TestSuite name="File & FileReader">
      <TestCase.Logical
        itShould="read basic file content correctly"
        fn={async ({expect}) => {
          const fileTextString = 'Hello, World!';
          const file = new File([fileTextString], 'test.txt', {
            type: 'text/plain',
            lastModified: Date.now(),
          });

          const reader = new FileReader();

          const onLoad = () =>
            new Promise(resolve => {
              reader.onload = event => {
                const result = event.target?.result;
                resolve(result);
              };
            });

          reader.readAsText(file);

          const onLoadResult = await onLoad();

          expect(onLoadResult).to.be.eq(fileTextString);
          expect(file.size).to.be.eq(fileTextString.length);
          expect(file.name).to.be.eq('test.txt');
        }}
      />

      <TestCase.Logical
        itShould="read basic file with chinese characters correctly"
        fn={async ({expect}) => {
          const file = new File([CHINESE_SELECTED_CHARACTERS], 'chinese.txt', {
            type: 'text/plain',
            lastModified: Date.now(),
          });
          const expectedSize = new Blob([CHINESE_SELECTED_CHARACTERS]).size;

          const reader = new FileReader();

          const onLoad = () =>
            new Promise(resolve => {
              reader.onload = event => {
                const result = event.target?.result;
                resolve(result);
              };
            });

          reader.readAsText(file);

          const onLoadResult = await onLoad();

          expect(onLoadResult).to.be.eq(CHINESE_SELECTED_CHARACTERS);
          expect(file.size).to.be.eq(expectedSize);
          expect(file.name).to.be.eq('chinese.txt');
        }}
      />

      <TestCase.Logical
        itShould="read the concatenated blob and file text correctly"
        fn={async ({expect}) => {
          const blobString = "I'am blob string";
          const fileTextString = '- Hello, World!';

          const blob = new Blob([blobString], {
            type: 'text/plain',
            lastModified: Date.now(),
          });
          const file = new File([blob, fileTextString], 'test2.txt', {
            type: 'text/plain',
            lastModified: Date.now(),
          });

          const reader = new FileReader();

          const onLoad = () =>
            new Promise(resolve => {
              reader.onload = event => {
                const result = event.target?.result;
                resolve(result);
              };
            });

          reader.readAsText(file);

          const onLoadResult = await onLoad();

          expect(onLoadResult).to.be.eq(blobString + fileTextString);
          expect(file.size).to.be.eq(blobString.length + fileTextString.length);
          expect(file.name).to.be.eq('test2.txt');
        }}
      />

      <TestCase.Logical
        itShould="read basic file content correctly and return data: URL"
        fn={async ({expect}) => {
          const Buffer = require('buffer').Buffer;
          const fileTextString = 'Hello, World!';
          const encodedString = Buffer.from(fileTextString).toString('base64');
          const expectedDataURL = 'data:text/plain;base64,' + encodedString;

          const file = new File([fileTextString], 'test.txt', {
            type: 'text/plain',
            lastModified: Date.now(),
          });

          const reader = new FileReader();

          const onLoad = () =>
            new Promise(resolve => {
              reader.onload = event => {
                const result = event.target?.result;
                resolve(result);
              };
            });

          reader.readAsDataURL(file);

          const onLoadResult = await onLoad();

          expect(onLoadResult).to.be.eq(expectedDataURL);
          expect(file.size).to.be.eq(fileTextString.length);
          expect(file.name).to.be.eq('test.txt');
        }}
      />
    </TestSuite>
  );
};

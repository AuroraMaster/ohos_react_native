/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

export default function getNumberOfLines(
  str: string,
  fontSize: number,
  width: number,
) {
  if (!str) {
    return 0;
  }
  return Math.ceil(getStrWidth(str, fontSize) / width);
}

function getStrWidth(str: string, fontSize: number) {
  const scale = fontSize / 17;
  const capitalWidth = 11 * scale;
  const lowerWidth = 8.6 * scale;
  const spaceWidth = 4 * scale;
  const numberWidth = 9.9 * scale;
  const chineseWidth = 17.3 * scale;

  const width = Array.from(str).reduce(
    (sum, char) =>
      sum +
      getCharWidth(char, {
        capitalWidth,
        lowerWidth,
        spaceWidth,
        numberWidth,
        chineseWidth,
      }),
    0,
  );

  // 0.1 个字算 1 个字
  return Math.ceil(width / fontSize) * fontSize;
}

interface ChartWidthType {
  capitalWidth: number;
  lowerWidth: number;
  spaceWidth: number;
  numberWidth: number;
  chineseWidth: number;
}

export function getCharWidth(
  char: string,
  {
    capitalWidth,
    lowerWidth,
    spaceWidth,
    numberWidth,
    chineseWidth,
  }: ChartWidthType,
) {
  let width;

  // 大写字母
  if (/[A-Z]/.test(char)) {
    width = capitalWidth;
    // 小写字母 和部分常见符号
  } else if (/([a-z]|[\u0021-\u002F])/.test(char)) {
    width = lowerWidth;
    // 数字
  } else if (/\d/.test(char)) {
    width = numberWidth;
    // 空格
  } else if (/\s/.test(char)) {
    width = spaceWidth;
    // 汉字或其他
  } else {
    width = chineseWidth;
  }

  return width;
}

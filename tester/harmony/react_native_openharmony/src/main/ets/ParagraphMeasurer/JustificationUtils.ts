/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {MeasuredFragment, TextFragment, TextFragmentMeasurer} from './types';
const VERY_THIN_SPACE_SYMBOL = ' ' as const;
const TEST_CHAR = 'l' as const; // used to measure space width

function measureInterWordSpace(
  fragment: TextFragment<any>,
  textFragmentMeasurer: TextFragmentMeasurer<any>,
) {
  const spaceWithTestFragment = {
    ...fragment,
    content: VERY_THIN_SPACE_SYMBOL + TEST_CHAR,
  };
  const testCharFragment = {
    ...fragment,
    content: TEST_CHAR,
  };
  const spaceWidth =
    textFragmentMeasurer.measureTextFragment(spaceWithTestFragment).width -
    textFragmentMeasurer.measureTextFragment(testCharFragment).width;
  return spaceWidth;
}

// justification algorithm could be improved, but this greedy/naive implementation better fits current text processing
export function justifyLineOfMeasuredFragments(
  lineOfMeasuredFragments: MeasuredFragment[],
  contentContainerWidth: number,
  textFragmentMeasurer: TextFragmentMeasurer<any>,
): MeasuredFragment[] {
  const fragmentsMetadata = [];
  const justifiedLine = [...lineOfMeasuredFragments];
  let fragmentsWidth = 0;
  const fragmentsLength = justifiedLine.length;
  for (let i = 0; i < fragmentsLength; i++) {
    const fragment = justifiedLine[i].fragment;
    if (fragment.type === 'text' && fragment.content.includes(' ')) {
      const content = fragment.content;
      const contentArray = content.split('');
      const spaceIndexes = contentArray
        .map((char, idx) => (char === ' ' ? idx : null))
        .filter(Number.isInteger);

      fragmentsMetadata.push({
        spacesCount: spaceIndexes.length,
        spaceWidth: measureInterWordSpace(fragment, textFragmentMeasurer),
        idx: i,
        content: contentArray,
        spaceIndexes,
      });
    }
    if (fragment.type === 'text') {
      const preciseWidth =
        textFragmentMeasurer.measureTextFragment(fragment).width;
      fragmentsWidth += preciseWidth;
      justifiedLine[i].size.width = preciseWidth;
    } else {
      fragmentsWidth += justifiedLine[i].size.width;
    }
  }

  // won't add spaces, need to adjust the width
  if (fragmentsMetadata.length === 0) {
    return justifiedLine;
  }
  const availableWidth = contentContainerWidth - fragmentsWidth;

  let addedSpaces = 0;
  let addedWidth = 0;
  // add spaces until we reach the available width
  // algorithm will add spaces in a round-robin fashion
  while (addedWidth < availableWidth) {
    const fragmentIdx = addedSpaces % fragmentsMetadata.length;
    const {spaceIndexes, spaceWidth, spacesCount, content} =
      fragmentsMetadata[fragmentIdx];
    if (addedWidth + spaceWidth > availableWidth) {
      break;
    }
    const addedSpacesForCurrentFragment = Math.floor(
      addedSpaces / fragmentsMetadata.length,
    );
    const spaceIdx = addedSpacesForCurrentFragment % spacesCount;
    // after adding spaces, indexes will shift so we need to adjust idx
    const spaceIdxAdjustment =
      spaceIdx * Math.ceil(addedSpacesForCurrentFragment / spacesCount);
    const spaceIndex = spaceIndexes[spaceIdx]! + spaceIdxAdjustment;
    content.splice(spaceIndex, 0, VERY_THIN_SPACE_SYMBOL);
    fragmentsMetadata[fragmentIdx].content = content;
    addedSpaces++;
    addedWidth += spaceWidth;
  }

  for (let fragmentMetadata of fragmentsMetadata) {
    const {idx, content, spaceWidth} = fragmentMetadata;
    (justifiedLine[idx].fragment as TextFragment<any>).content =
      content.join('');
    const newWidth =
      justifiedLine[idx].size.width +
      (content.join('').split(VERY_THIN_SPACE_SYMBOL).length - 1) * spaceWidth;
    justifiedLine[idx].size.width = newWidth;
  }
  let newWidthSum = 0;
  for (let j = 0; j < fragmentsLength - 1; j++) {
    newWidthSum += justifiedLine[j].size.width;
  }
  justifiedLine[fragmentsLength - 1].size.width =
    contentContainerWidth - newWidthSum;
  return justifiedLine;
}

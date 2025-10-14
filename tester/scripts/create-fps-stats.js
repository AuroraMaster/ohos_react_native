/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

const {
  createFpsStats,
  generateFPSStatsConsoleOutput,
} = require('./lib/create-fps-stats');

let inputData = '';

process.stdin.on('data', chunk => {
  inputData += chunk;
});

process.stdin.on('end', () => {
  const frameTimes = JSON.parse(inputData);

  const result = createFpsStats(frameTimes);

  frameTimes.forEach(frameTime => {
    console.log(frameTime);
  });
  console.log(generateFPSStatsConsoleOutput(result));
});

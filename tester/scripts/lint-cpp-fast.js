/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

const {exec} = require('child_process');
const {promisify} = require('util');

const execAsync = promisify(exec);

async function findChangedCppFiles() {
  try {
    const {stdout} = await execAsync('npm run --silent find_changed_files:cpp');
    const files = stdout
      .trim()
      .split('\n')
      .filter(file => file.endsWith('.cpp') || file.endsWith('.h'))
      .map(file =>
        file.replace('react-native-harmony', '../react-native-harmony'),
      );
    if (files.length === 0) {
      console.log('No changed .cpp or .h files to lint.');
      return;
    }

    const clangTidyCmd = `clang-tidy ${files.join(' ')} -p ./harmony/entry/.cxx/default/default/arm64-v8a/ -extra-arg=-Wno-unused-command-line-argument`;

    const {stdout: lintOutput} = await execAsync(clangTidyCmd);
    console.log(lintOutput);
  } catch (error) {
    console.log(error.stdout);
  }
}

findChangedCppFiles();

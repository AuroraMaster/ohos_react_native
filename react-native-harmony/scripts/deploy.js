/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// @ts-check
const { execSync } = require('child_process');
const fs = require('node:fs');
const readline = require('readline');
const https = require('node:https');

const RNOH_REPO_TOKEN = process.env.RNOH_REPO_TOKEN ?? '';

if (!RNOH_REPO_TOKEN) {
  console.log('RNOH_REPO_TOKEN not found');
  process.exit(1);
}

const GITLAB_URL = 'https://gl.swmansion.com';
const HAR_FILE_OUTPUT_PATH =
  'tester/harmony/react_native_openharmony/build/default/outputs/default/react_native_openharmony.har';

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

function runDeployment() {
  if (!process.cwd().endsWith('react-native-harmony')) {
    console.log(
      'This script should be executed from react-native-harmony directory'
    );
    process.exit(1);
  }

  if (!isRepositoryClean()) {
    console.log(
      'Repository should be clean, on main branch and up to date with upstream.'
    );
    process.exit(1);
  }

  let version = '';

  const currentVersion = JSON.parse(
    fs.readFileSync('./package.json').toString()
  )['version'];
  rl.question(
    `Current version: ${currentVersion}. Enter new version: `,
    (newVersion) => {
      version = newVersion;

      execSync(`npm run update_version  -- --new-version ${version}`, {
        stdio: 'inherit',
      });

      rl.question(
        `Please generate ${HAR_FILE_OUTPUT_PATH} file. Open DevEco Studio, select any file in 'react_native_openharmony' module, and run Build > Make Module 'react_native_openharmony'.\nOnce you finish type 'done': `,
        (answer) => {
          if (answer !== 'done') {
            console.log('Deployment aborted');
            process.exit(1);
          }
          console.log(
            `Copying ${`../${HAR_FILE_OUTPUT_PATH}`} to ./harmony dir`
          );
          if (!fs.existsSync(`../${HAR_FILE_OUTPUT_PATH}`)) {
            console.log(`Couldn't find ${HAR_FILE_OUTPUT_PATH}.`);
            process.exit(1);
          }
          fs.rmSync('./harmony', { recursive: true, force: true });
          fs.mkdirSync('./harmony');
          fs.renameSync(
            `../${HAR_FILE_OUTPUT_PATH}`,
            './harmony/react_native_openharmony.har'
          );

          const changelogForCurrentVersion = execSync(
            `npm run -s gen:changelog`
          ).toString();

          updateChangelog(version, changelogForCurrentVersion);

          execSync(`npm publish --dry-run`, { stdio: 'inherit' });

          rl.question(
            'Are changes good to be published and pushed to the upstream? (yes/no): ',
            async (answer) => {
              if (answer.toLowerCase() === 'yes') {
                execSync(`npm publish`, { stdio: 'inherit' });
                console.log('NPM Package was published successfully.');
                execSync(
                  `git checkout -b release-react-native-harmony-${version}`
                );
                execSync('git add -A');
                execSync(
                  `git commit -m "release: react-native-harmony@${version}"`,
                  {
                    stdio: 'inherit',
                  }
                );
                execSync(`git push -u origin HEAD --no-verify`, {
                  stdio: 'inherit',
                });

                execSync(`git tag v${version}`);
                execSync(`git push -u origin v${version} --no-verify`, {
                  stdio: 'inherit',
                });
                const mergeRequestId = await createMergeRequest(
                  `release-react-native-harmony-${version}`,
                  `release: react-native-harmony@${version}`
                );
                console.log(`Please merge the following Merge Request:\n
                https://gl.swmansion.com/rnoh/react-native-harmony/-/merge_requests/${mergeRequestId}`);
                rl.close();
              } else {
                console.log('Deployment aborted.');
                rl.close();
              }
            }
          );
        }
      );
    }
  );
}

/**
 * @returns {boolean}
 */
function isRepositoryClean() {
  const status = execSync('git status --porcelain', { encoding: 'utf-8' });
  const branch = execSync('git branch --show-current', {
    encoding: 'utf-8',
  }).trim();
  const isUpdated =
    execSync('git rev-list HEAD...origin/main --count', {
      encoding: 'utf-8',
    }).trim() === '0';
  return !status && branch === 'main' && isUpdated;
}

/**
 * @param {string} version
 *  @param {string} changelogForCurrentVersion
 */
function updateChangelog(version, changelogForCurrentVersion) {
  let data = fs.readFileSync('../CHANGELOG.md').toString();
  data = data.replace(
    '# Changelog',
    `# Changelog\n\n## v${version}\n ${changelogForCurrentVersion}`
  );
  fs.writeFileSync('../CHANGELOG.md', data);
}

/**
 * @param {string} sourceBranch
 * @param {string} title
 * @returns {Promise<number>}
 */
async function createMergeRequest(sourceBranch, title) {
  try {
    const response = await fetch(
      `${GITLAB_URL}/api/v4/projects/447/merge_requests`,
      {
        method: 'POST',
        headers: {
          'PRIVATE-TOKEN': RNOH_REPO_TOKEN,
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          source_branch: sourceBranch,
          target_branch: 'main',
          title: title,
          squash: false,
          remove_source_branch: true,
        }),
      }
    );
    if (!response.ok) {
      throw new Error(
        `Failed to create merge request: ${response.statusText} ${response.status}`
      );
    }
    const responseData = await response.json();
    return responseData.iid;
  } catch (error) {
    console.error('Error creating merge request:', error);
    throw error;
  }
}

runDeployment();

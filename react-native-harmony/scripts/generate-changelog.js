/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// @ts-check
const { exec } = require('child_process');

/** @const {string} */
const REPO_URL = 'https://gl.swmansion.com/rnoh/react-native-harmony/-';

/**
 * @typedef {("breaking"|"addition"|"fix"|"removal"|"other"|"change"|"depreciation")} ChangeType
 */

/**
 * @typedef {Object} Change
 * @property {string} text
 * @property {string} commitHash
 * @property {ChangeType} type
 */

async function main() {
  const latestVersionTag = await findLatestVersionTag();
  if (!latestVersionTag) {
    console.error("Couldn't find the latest version tag");
    return;
  }
  exec(
    `git log ${latestVersionTag}..HEAD --pretty=format:"%H %B"`,
    (err, stdout, stderr) => {
      if (err) {
        console.error('Error executing git log:', stderr);
        return;
      }
      const changes = extractChangesFromLog(stdout);
      const sortedChanges = sortChanges(changes);
      printChanges(sortedChanges);
    }
  );
}

/**
 * @returns {Promise<string | null>}
 */
function findLatestVersionTag() {
  return new Promise((resolve, reject) => {
    exec("git tag --list 'v*' --sort=-v:refname", (error, stdout, stderr) => {
      if (error) {
        console.error('Error fetching tags:', stderr);
        reject(stderr);
        return;
      }
      const tags = stdout.split('\n').filter((tag) => tag.trim() !== '');
      if (tags.length > 0) {
        resolve(tags[0].trim());
      } else {
        resolve(null);
      }
    });
  });
}

/**
 * @param {string} gitLogOutput
 * @returns {Array<Change>}
 */
function extractChangesFromLog(gitLogOutput) {
  const commits = gitLogOutput.split(/See merge request .*\n/);
  return commits.flatMap((commit) => extractChangesFromCommit(commit));
}

/**
 * @param {string} commit
 * @returns {Array<Change>}
 */
function extractChangesFromCommit(commit) {
  const lines = commit.trim().split('\n');
  const hash = lines[0].split(' ')[0];
  const body = lines
    .slice(1)
    .join('\n')
    .replace(/<!--[\s\S]*?-->/g, '');
  return extractChangesFromCommitBody(body, hash);
}

/**
 * @param {string} body
 * @param {string} hash
 * @returns {Array<Change>}
 */
function extractChangesFromCommitBody(body, hash) {
  const sectionIdentifier = '## Changes';
  const changesIndex = body.indexOf(sectionIdentifier);
  if (changesIndex === -1) return [];
  const changesEndIndex = body.indexOf(
    '##',
    changesIndex + sectionIdentifier.length
  );
  const changesText =
    changesEndIndex === -1
      ? body.slice(changesIndex)
      : body.slice(changesIndex, changesEndIndex);
  const items = changesText.match(/[-*] .*/g) || [];
  return items.map((item) => {
    let text = item.replace(/^[-*]\s*/, '');
    const [firstWord, ...otherWords] = text.split(' ');
    text = [firstWord.toLowerCase(), ...otherWords].join(' ');
    return {
      text: text,
      commitHash: hash,
      type: determineChangeType(text),
    };
  });
}

/**
 * @param {string} item
 * @returns {ChangeType}
 */
function determineChangeType(item) {
  item = item.toLowerCase();
  if (item.includes('[breaking]')) return 'breaking';
  if (item.startsWith('add')) return 'addition';
  if (item.startsWith('change')) return 'change';
  if (item.startsWith('fix')) return 'fix';
  if (item.startsWith('remove')) return 'removal';
  if (item.startsWith('deprecate')) return 'depreciation';
  return 'other';
}

/**
 * @param {Array<Change>} changes
 * @returns {Array<Change>}
 */
function sortChanges(changes) {
  const priority = {
    breaking: 1,
    depreciation: 2,
    addition: 3,
    fix: 4,
    change: 5,
    removal: 6,
    other: 7,
  };
  return changes.sort((a, b) => priority[a.type] - priority[b.type]);
}

/**
 * @param {Array<Change>} changes
 */
function printChanges(changes) {
  changes.forEach((change) => {
    console.log(
      `- ${change.text} ([${change.commitHash.substring(
        0,
        10
      )}](${REPO_URL}/commit/${change.commitHash}))`
    );
  });
}

main();

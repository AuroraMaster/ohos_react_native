/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

const fs = require('fs');
const JSON5 = require('json5');
const path = require('path');

const templatePath = path.join(
  __dirname,
  '..',
  'harmony',
  'build-profile.template.json5',
);
const existingProfilePath = path.join(
  __dirname,
  '..',
  'harmony',
  'build-profile.json5',
);

if (fs.existsSync(existingProfilePath)) {
  let existingProfile = JSON5.parse(
    fs.readFileSync(existingProfilePath, 'utf-8'),
  );
  let template = JSON5.parse(fs.readFileSync(templatePath, 'utf-8'));
  let signingConfigs =
    existingProfile.app && existingProfile.app.signingConfigs;

  existingProfile = {...template};

  if (signingConfigs) {
    existingProfile.app.signingConfigs = signingConfigs;
  }

  fs.writeFileSync(
    existingProfilePath,
    JSON5.stringify(existingProfile, null, 2),
  );
} else {
  // File doesn't exist, create a copy from the template
  fs.copyFileSync(templatePath, existingProfilePath);
}

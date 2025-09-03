/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import path from 'path';
import fs from 'fs-extra';
import { spawn } from 'child_process';
import { AbsolutePath, DescriptiveError } from '../core';
import { Logger } from '../io';
import { RealCliExecutor } from '../io/CliExecutor';

interface LaunchOptions {
  devEcoStudioToolsPath: AbsolutePath;
  sdkToolchainsPath: AbsolutePath;
  cli: RealCliExecutor;
  logger: Logger;
  env: NodeJS.ProcessEnv;
}

const MIN_SUPPORTED_VERSION = [6, 0, 0, 828];
function compareVersionTuple(a: number[], b: number[]): number {
  for (let i = 0; i < Math.max(a.length, b.length); i++) {
    const av = a[i] ?? 0;
    const bv = b[i] ?? 0;
    if (av !== bv) return av - bv;
  }
  return 0;
}
function isSupportedVersion(versionDir: string): boolean {
  const m = versionDir.match(/^(\d+)\.(\d+)\.(\d+)\.(\d+)/);
  if (!m) return false;
  const current = m.slice(1).map(n => parseInt(n, 10));
  return compareVersionTuple(current, MIN_SUPPORTED_VERSION) >= 0;
}

async function getCreatedSimulators(cli: RealCliExecutor, emulator: string): Promise<string[]> {
  try {
    let simulatorInfos = '';
    await cli.run(`"${emulator}"`, {
      args: ['--list'],
      shell: true,
      onStderr: (msg) => {
        simulatorInfos += msg;
      },
    });
    if (!simulatorInfos || simulatorInfos.trim() === '' ||
      simulatorInfos.trim() === '[Empty]' ||
      simulatorInfos.includes('not ')) {
      return [];
    }

    return simulatorInfos.split('\n').filter(line => line.trim() !== '');
  } catch (error) {
    throw new DescriptiveError({
      whatHappened: `Failed to get simulator list: ${(error as any)?.message || error}`,
      whatCanUserDo: [
        'Ensure DevEco Studio is properly installed and configured.',
        'Check if the emulator path is correct.',
        'Verify that the emulator has read/write permissions.'
      ],
      extraData: error,
    });
  }
}

async function getDevEcoVersion(devEcoStudioToolsPath: AbsolutePath): Promise<string | null> {
  try {
    let productInfoPath = path.join(devEcoStudioToolsPath.toString(),
                                    '..',
                                    'Resources',
                                    'product-info.json');
    if (process.platform === 'win32') {
      productInfoPath = path.join(devEcoStudioToolsPath.toString(),
                                  '..',
                                  'product-info.json');
    }
    const content = await fs.readFile(productInfoPath, 'utf-8');
    const info = JSON.parse(content);
    return info.version || null;
  } catch (error) {
    throw new DescriptiveError({
      whatHappened: `Failed to read DevEco Studio version from product-info.json: ${(error as any)?.message || error}`,
      whatCanUserDo: [
        'Verify DevEco Studio installation is complete and not corrupted.',
        'Check if the DevEco Studio tools path is correct.',
        'Ensure product-info.json file exists and is readable.'
      ],
      extraData: error,
    });
  }
}

function resolveDeviceManagerXml(env: NodeJS.ProcessEnv, devEcoVersion: string): string[] {
  const candidates: string[] = [];
  if (process.platform === 'darwin') {
    const home = env.HOME || '';
    candidates.push(
      path.join(home,
                'Library',
                'Application Support',
                'Huawei',
                `DevEcoStudio${devEcoVersion}`,
                'options',
                'deviceManager.xml'),
    );
  } else if (process.platform === 'win32') {
    const appdata = env.APPDATA || '';
    candidates.push(
      path.join(appdata,
                'Huawei',
                `DevEcoStudio${devEcoVersion}`,
                'options',
                'deviceManager.xml'),
    );
  } else {
    throw new DescriptiveError({
      whatHappened: `Unsupported operating system.`,
      whatCanUserDo: ['Please run the RNOH project on Windows or macOS.']
    });
  }
  return candidates;
}

interface DevicePathInfo {
  instancePath?: string;
  imageRoot?: string;
}

function parseDeviceManagerXml(xmlContent: string): DevicePathInfo {
  const info: DevicePathInfo = {};
  const optionRegex = /<option\b[^>]*?>/gi;
  let m: RegExpExecArray | null;
  while ((m = optionRegex.exec(xmlContent))) {
    const tag = m[0];
    const nameMatch = tag.match(/\bname=["']([^"']+)["']/i);
    if (!nameMatch) continue;
    const valueMatch = tag.match(/\bvalue=["']([^"']+)["']/i);
    if (!valueMatch) continue;
    const name = nameMatch[1];
    const value = valueMatch[1];
    if (name === 'imageDeployPath' && !info.imageRoot) {
      info.imageRoot = value;
    } else if (name === 'openharmonyDeployPath' && !info.instancePath) {
      info.instancePath = value;
    }
    if (info.imageRoot && info.instancePath) break;
  }
  return info;
}

function candidateEmulatorExecutables(devEcoStudioToolsPath: AbsolutePath): string[] {
  const list: string[] = [];
  const add = (...segs: string[]) =>
    list.push(devEcoStudioToolsPath.copyWithNewSegment(...segs).toString());

  if (process.platform === 'win32') {
    add('emulator', 'Emulator.exe');
  } else {
    add('emulator', 'Emulator');
  }
  return list;
}

function pickExisting(paths: string[]): string | undefined {
  return paths.find(p => fs.existsSync(p));
}

async function resolveInstanceAndImageRoot(
  env: NodeJS.ProcessEnv, devEcoVersion: string
): Promise<{ instancePath: string; imageRoot: string } | null> {
  const defaultInstanceCandidates: string[] = [];
  const defaultImageCandidates: string[] = [];

  if (process.platform === 'darwin') {
    const home = env.HOME || '';
    defaultInstanceCandidates.push(
      path.join(home,
                '.Huawei',
                'Emulator',
                'deployed'),
    );
    defaultImageCandidates.push(
      path.join(home,
                'Library',
                'Huawei',
                'Sdk'),
    );
  } else if (process.platform === 'win32') {
    const localAppData = env.LOCALAPPDATA || '';
    defaultInstanceCandidates.push(
      path.join(localAppData,
                'Huawei',
                'Emulator',
                'deployed')
    );
    defaultImageCandidates.push(
      path.join(localAppData,
                'Huawei',
                'Sdk')
    );
  } else {
    throw new DescriptiveError({
      whatHappened: `Unsupported operating system.`,
      whatCanUserDo: ['Please run the RNOH project on Windows or macOS.']
    });
  }

  let xmlInfo: DevicePathInfo = {};
  for (const xmlPath of resolveDeviceManagerXml(env, devEcoVersion)) {
    if (fs.existsSync(xmlPath)) {
      try {
        const content = await fs.readFile(xmlPath, 'utf-8');
        xmlInfo = parseDeviceManagerXml(content);
        if (xmlInfo.instancePath || xmlInfo.imageRoot) break;
      } catch (error) {
        throw new DescriptiveError({
          whatHappened: `Failed to read or parse device manager XML at ${xmlPath}: ${(error as any)?.message || error}`,
          whatCanUserDo: [
            'Verify that the DevEco Studio deviceManager.xml file exists and is readable.',
            'Check if DEVECO_SDK_HOME environment variable is set correctly.',
            'Ensure DevEco Studio has been launched at least once to generate configuration files.',
            'Check file permissions for the DevEco Studio configuration directory.'
          ],
          extraData: error,
        });
      }
    }
  }

  let resolvedInstance = pickExisting(defaultInstanceCandidates);
  let resolvedImageRoot = pickExisting(defaultImageCandidates);
  if (xmlInfo.imageRoot) {
    resolvedImageRoot = xmlInfo.imageRoot;
  }
  if (xmlInfo.instancePath) {
    resolvedInstance = xmlInfo.instancePath;
  }
  if (!resolvedInstance || !resolvedImageRoot) {
    throw new DescriptiveError({
      whatHappened: `No simulator instance or image root directory detected.`,
      whatCanUserDo: [
        'Have you already launched the simulator via DevEco Studio?'
      ]
    });
  }
  return { instancePath: resolvedInstance, imageRoot: resolvedImageRoot };
}

export async function launchHarmonySimulator(opt: LaunchOptions): Promise<void> {
  const {
    devEcoStudioToolsPath,
    cli,
    logger,
  } = opt;

  const devEcoVersion = await getDevEcoVersion(devEcoStudioToolsPath);
  if (!devEcoVersion) {
    throw new DescriptiveError({
      whatHappened: `DevEco Studio version could not be obtained.`,
      whatCanUserDo: [
        'Verify DevEco Studio is properly installed and the tools path is correct.',
        'Check if product-info.json file exists in the DevEco Studio installation directory.',
        'Ensure you have the latest version of DevEco Studio installed.'
      ]
    });
  }

  if (!isSupportedVersion(devEcoVersion)) {
    throw new DescriptiveError({
      whatHappened: `Current DevEco SDK version (${devEcoVersion}) does not support launching the simulator via command line (requires >= ${MIN_SUPPORTED_VERSION.join('.')}).`,
      whatCanUserDo: [
        `Update DevEco Studio to version ${MIN_SUPPORTED_VERSION.join('.')} or higher.`,
        'Check the official DevEco Studio website for the latest version.',
        'Launch the simulator manually from DevEco Studio IDE.'
      ]
    });
  }

  const versionList = devEcoVersion.split('.');
  const majorVersion = `${versionList[0]}.${versionList[1]}`;
  const paths = await resolveInstanceAndImageRoot(opt.env, majorVersion);
  if (!paths) {
    throw new DescriptiveError({
      whatHappened: `Could not resolve simulator instance path or image root.`,
      whatCanUserDo: [
        'Open DevEco Studio and launch the simulator at least once to generate configuration files.',
        'Check if DEVECO_SDK_HOME environment variable is set correctly.',
        'Verify that the simulator has been properly configured in DevEco Studio.',
        'Ensure the default simulator paths exist in your system.'
      ]
    });
  }

  const emulatorExe = pickExisting(candidateEmulatorExecutables(devEcoStudioToolsPath));
  if (!emulatorExe) {
    throw new DescriptiveError({
      whatHappened: `Emulator executable not found in DevEco Studio installation.`,
      whatCanUserDo: [
        'Verify DevEco Studio installation is complete and not corrupted.',
        'Check if the emulator component is installed in DevEco Studio.',
        'Reinstall DevEco Studio if the emulator binary is missing.',
        'Ensure the DevEco Studio tools path is correctly configured.'
      ]
    });
  }
  const simulatorName = await getCreatedSimulators(cli, emulatorExe);
  if (simulatorName.length <= 0) {
    throw new DescriptiveError({
      whatHappened: `No existing HarmonyOS simulator instance detected.`,
      whatCanUserDo: [
        'Create a simulator instance in DevEco Studio first.',
        'Open DevEco Studio Device Manager and create a new virtual device.',
        'Ensure the simulator instance is properly configured and available.',
        'Check if the emulator --list command returns any valid instances.'
      ]
    });
  }

  try {
    const quoteIfNeeded = (p: string) =>
      process.platform === 'win32' ? `"${p}"` : p;
    const args = [
      '-hvd',
      simulatorName[0].trim(),
      '-path',
      quoteIfNeeded(paths.instancePath),
      '-imageRoot',
      quoteIfNeeded(paths.imageRoot),
    ];
    const child = spawn(quoteIfNeeded(emulatorExe), args, {
      cwd: path.dirname(emulatorExe),
      detached: true,
      stdio: 'ignore',
      shell: process.platform === 'win32',
    });
    child.unref();
    logger.info(() => `Launching HarmonyOS simulator in background: ${simulatorName[0]}`);
  } catch (e) {
    throw new DescriptiveError({
      whatHappened: `Failed to start simulator process: ${(e as any)?.message || e}`,
      whatCanUserDo: [
        'Confirm that the DevEco Simulator can be started manually from DevEco Studio.',
        'Ensure no stale simulator process is blocking required ports.',
        'Reinstall or repair DevEco Studio if the simulator binary is corrupted.'
      ],
      extraData: e,
    });
  }
}

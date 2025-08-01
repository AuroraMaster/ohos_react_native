/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import { AbsolutePath, DescriptiveError } from '../core';
import { Logger, RealFS } from '../io';
import { RealCliExecutor } from '../io/CliExecutor';
import { Command } from './types';
import fs from 'fs-extra';
import crypto from 'node:crypto';
import JSON5 from 'json5';
import inquirer from 'inquirer';
import path from 'path';
import {
  getDefaultUserTerminal,
  startServerInNewWindow,
  findDevServerPort,
} from '@react-native-community/cli-tools';

const COMMAND_NAME = 'run-harmony';

export const commandRunHarmony: Command = {
  name: COMMAND_NAME,
  description: 'Builds your app and starts it on your phone.',
  options: [
    {
      name: '--harmony-project-path <path>',
      description: 'Specifies the root of your OpenHarmony project.',
      default: './harmony',
    },
    {
      name: '--module <string>',
      description: 'Name of the OH module to run.',
      default: 'entry',
    },
    {
      name: '--build-mode <string>',
      description: 'Debug or Release.',
      default: 'debug',
    },
    {
      name: '--product <string>',
      description: 'OpenHarmony product defined in build-profile.json5.',
      default: 'default',
    },
    {
      name: '--ability <string>',
      description: 'Name of the ability to start.',
      default: 'EntryAbility',
    },
    {
      name: '--simulator <string>',
      description: 'The name of the simulator that are currently connected.',
      default: 'undefined',
    },
    {
      name: '--port <number>',
      description: 'The port to use for the device.',
      default: process.env.RCT_METRO_PORT || 8081,
    },
    {
      name: '--no-packager',
      description: 'Do not launch packager while running the app.',
    },
  ],
  func: async (_argv, _config, rawArgs: any) => {
    const logger = new Logger();
    try {
      const DEVECO_SDK_HOME = process.env.DEVECO_SDK_HOME;
      if (!DEVECO_SDK_HOME) {
        throw new DescriptiveError({
          whatHappened: 'DEVECO_SDK_HOME environment variable is not set',
          whatCanUserDo: [
            process.platform === 'darwin'
              ? 'On MacOS, the contents directory is typically located at: /Applications/DevEco-Studio.app/Contents/sdk. Set this path as the value of DEVECO_SDK_HOME'
              : "Locate the installation directory of DevEco Studio, and set its 'sdk' subdirectory as the value of DEVECO_SDK_HOME environment variable.",
          ],
        });
      }
      const fs = new RealFS();
      const sdkPath = new AbsolutePath(DEVECO_SDK_HOME);
      const sdkDirectoryNames = fs
        .readDirentsSync(sdkPath)
        .flatMap((dirent) => {
          if (dirent.isDirectory()) {
            return [dirent.name];
          } else {
            return [];
          }
        });
      if (sdkDirectoryNames.length === 0) {
        throw new DescriptiveError({
          whatHappened: `${sdkPath.toString()} has no directories`,
          unexpected: true,
        });
      }
      const sdkToolchainsPath = sdkPath.copyWithNewSegment(
        sdkDirectoryNames[0],
        'openharmony',
        'toolchains'
      );
      const cli = new RealCliExecutor();
      const harmonyProjectPath = new AbsolutePath(rawArgs.harmonyProjectPath);
      const productName: string = rawArgs.product;
      const buildMode: string = rawArgs.buildMode;
      const moduleName: string = rawArgs.module;
      const abilityName: string = rawArgs.ability;
      const simulatorName: string = rawArgs.simulator;
      const defaultPort: number = rawArgs.port;
      const packager: boolean = rawArgs.packager;
      const devEcoStudioToolsPath = new AbsolutePath(
        DEVECO_SDK_HOME
      ).copyWithNewSegment('..', 'tools');
      if (!fs.existsSync(devEcoStudioToolsPath)) {
        throw new DescriptiveError({
          whatHappened: `${devEcoStudioToolsPath.toString()} doesn't exist`,
        });
      }
      const bundleName = JSON5.parse(
        fs.readTextSync(
          harmonyProjectPath.copyWithNewSegment('AppScope', 'app.json5')
        )
      ).app.bundleName;

      const selectDevice = async (deviceAndSimulatorInfos: string) => {
        const lines = deviceAndSimulatorInfos.trim().split('\n');
        const availableDevices = lines.map((line) => {
          const parts = line.split(/\s+/);
          return {
            name: parts[0],
            method: parts[1],
            state: parts[2],
            locate: parts[3],
            connectTool: parts[4]
          };
        });
        if (simulatorName !== 'undefined') {
          const isRequestedDeviceAvailable = availableDevices.some((deviceOrSimulator) => deviceOrSimulator.name === simulatorName);
          if (!isRequestedDeviceAvailable) {
            throw new DescriptiveError({
              whatHappened: `Simulator with name "${simulatorName}" not found.`,
              whatCanUserDo: [
                'Please confirm whether the entered simulator name has been activated.',
              ]
            });
          }
          return simulatorName;
        }
        const connectedDevices = availableDevices.filter((deviceOrSimulator) => deviceOrSimulator.state === 'Connected');
        if (connectedDevices.length > 1) {
          const answers = await inquirer.prompt([
            {
              type: 'list',
              name: 'selectedDeviceIndex',
              message: 'Please select a device:',
              choices: connectedDevices.map((deviceOrSimulator, index) => `${index + 1}. ${deviceOrSimulator.name}`)
            }
          ])
          return connectedDevices[parseInt(answers.selectedDeviceIndex) - 1].name;
        } else if (connectedDevices.length === 1) {
          return connectedDevices[0].name;
        } else {
          throw new DescriptiveError({
            whatHappened: `No devices are connected in the current environment.`,
            whatCanUserDo: [
              'Please connect your HarmonyOS device or open the simulator.',
            ]
          });
        }
      };

      const deviceAndSimulatorInfo: string = await cli.run(
        sdkToolchainsPath.copyWithNewSegment('hdc').toString(),
        {
          args: ['list', 'targets', '-v'],
          cwd: harmonyProjectPath,
        }
      );

      const deviceOrSimulatorName = await selectDevice(deviceAndSimulatorInfo)

      let currentPort: number = defaultPort;
      if (packager) {
        const root: string = _config.root || '.';
        const reactNativePath: string = _config.reactNativePath || path.dirname(
          require.resolve('react-native', { paths: [root] }),
        );
        const { port: newPort, startPackager } = await findDevServerPort(
          defaultPort,
          root,
        );

        if (newPort !== 8081) {
          await trySetMetroPort(harmonyProjectPath.copyWithNewSegment(
            'entry', 'src', 'main', 'ets', 'pages', 'Index.ets').toString(), newPort);
        }
        if (startPackager) {
          startServerInNewWindow(
            newPort,
            root,
            reactNativePath,
            getDefaultUserTerminal(),
          );
        }
        currentPort = newPort;
      }

      const runJob = async (name: string, job: () => Promise<void>) => {
        const stop = logger.start((s) => s.bold(name));
        logger.info((s) => s.bold(`${name} started`));
        try {
          await job();
          logger.succeed((s) => s.bold(`${name} finished`));
        } catch (err) {
          if (err instanceof DescriptiveError) {
            throw err;
          }
          throw new DescriptiveError({
            whatHappened: `${name} failed`,
            extraData: err,
          });
        } finally {
          stop();
        }
      };

      await runJob('[1/3] installing dependencies', async () => {
        let ohpmPath = devEcoStudioToolsPath
          .copyWithNewSegment('ohpm', 'bin', 'ohpm');
        if (process.platform === 'win32') {
          ohpmPath = devEcoStudioToolsPath
            .copyWithNewSegment('ohpm', 'bin', 'ohpm.bat');
        }
        await cli.run(
          process.platform === 'win32' ? `"${ohpmPath.toString()}"` :
            ohpmPath.toString(),
          {
            args: [
              'install',
              '--all',
              '--strict_ssl',
              'true',
            ],
            cwd: harmonyProjectPath,
            shell: true,
            onArgsStringified: (commandWithArgs) => {
              logger.debug((s) => s.bold(s.gray(commandWithArgs)));
            },
            onStdout: (msg) => {
              logger.debug((s) => s.gray(msg.trimEnd()));
            },
            onStderr(msg) {
              logger.debug(() => msg.trimEnd());
            },
          }
        );
      });
      await runJob('[2/3] building app', async () => {
        let nodePath = devEcoStudioToolsPath
          .copyWithNewSegment('node', 'bin', 'node');
        if (!fs.existsSync(nodePath)) {
          nodePath = devEcoStudioToolsPath.copyWithNewSegment('node', 'node');
        }
        let hvigorPathRaw = devEcoStudioToolsPath.copyWithNewSegment('hvigor', 'bin', 'hvigorw.js').toString();
        const hvigorPath = process.platform === 'win32' ? `"${hvigorPathRaw}"` : hvigorPathRaw;
        await cli.run(
          process.platform === 'win32' ? `"${nodePath.toString()}"` :
            nodePath.toString(),
          {
            args: [
              hvigorPath,
              `-p`,
              `module=${moduleName}@default`,
              `-p`,
              `product=${productName}`,
              `-p`,
              `buildMode=${buildMode}`,
              `-p`,
              `requiredDeviceType=phone`,
              `assembleHap`,
            ],
            cwd: harmonyProjectPath,
            shell: true,
            onArgsStringified: (commandWithArgs) => {
              logger.debug((s) => s.bold(s.gray(commandWithArgs)));
            },
            onStdout: (msg) => {
              logger.debug((s) => s.gray(msg.trimEnd()));
            },
            onStderr(msg) {
              logger.debug(() => msg.trimEnd());
            },
          }
        );
      });
      await runJob('[3/3] installing and opening app', async () => {
        const tmpDirName = generateRandomString();
        const ohosTmpDirPath = `data/local/tmp/${tmpDirName}`;

        const exec = async (command: string, args: string[]) => {
          const result = await cli.run(command, {
            args: args,
            shell: true,
            onArgsStringified: (commandWithArgs) =>
              logger.debug((s) => s.bold(s.gray(commandWithArgs))),
          });
          if (result.startsWith('[Fail]')) {
            throw new DescriptiveError({ whatHappened: result });
          }
          if (result) {
            logger.debug((s) => s.gray(result.trimEnd() + '\n'));
          } else {
            logger.debug(() => '');
          }
          return result;
        };

        const hdcPathStrRaw = sdkToolchainsPath
          .copyWithNewSegment('hdc')
          .toString();
        const hdcPathStr = process.platform === 'win32' ? `"${hdcPathStrRaw.toString()}"` : hdcPathStrRaw;

        const tryRemoveForwardPorting = async () => {
          let fportInfo: string = '';
          await cli.run(
            hdcPathStrRaw,
            {
              args: ['fport', 'ls'],
              cwd: harmonyProjectPath,
              onStdout: (msg) => {
                fportInfo += msg;
              },
              onStderr: (msg) => {
                logger.debug((s) => s.gray(msg.trimEnd()));
              },
            });
          if (fportInfo.includes(`tcp:${currentPort} tcp:${currentPort}`)) {
            await exec(hdcPathStr, [
              'fport',
              'rm',
              `tcp:${currentPort}`,
              `tcp:${currentPort}`
            ]);
          }
        };

        await exec(hdcPathStr, ['-t', deviceOrSimulatorName, 'shell', 'aa', 'force-stop', bundleName]);
        try {
          await exec(hdcPathStr, ['-t', deviceOrSimulatorName, 'shell', 'mkdir', ohosTmpDirPath]);
          let hapName = `${moduleName}-default-signed.hap`;
          if (simulatorName !== 'undefined') {
            hapName = `${moduleName}-default-unsigned.hap`;
          }
          await exec(hdcPathStr, [
            '-t',
            deviceOrSimulatorName,
            'file',
            'send',
            harmonyProjectPath
              .copyWithNewSegment(
                moduleName,
                'build',
                'default',
                'outputs',
                'default',
                hapName
              )
              .toString(),
            ohosTmpDirPath,
          ]);
          const installationResult = await exec(hdcPathStr, [
            '-t',
            deviceOrSimulatorName,
            'shell',
            'bm',
            'install',
            '-p',
            ohosTmpDirPath,
          ]);
          if (installationResult.includes('failed to install')) {
            throw new DescriptiveError({
              whatHappened: 'Installation failed.',
              whatCanUserDo: [
                `If an application with the same bundle name is already installed, try uninstalling it:\n${hdcPathStr} shell bm uninstall -n ${bundleName}`,
              ],
            });
          }
        } finally {
          await exec(hdcPathStr, ['-t', deviceOrSimulatorName, 'shell', 'rm', '-rf', ohosTmpDirPath]);
        }
        await exec(hdcPathStr, [
          '-t',
          deviceOrSimulatorName,
          'shell',
          'aa',
          'start',
          '-a',
          abilityName,
          '-b',
          bundleName,
        ]);
        await tryRemoveForwardPorting();
        await exec(hdcPathStr, [
          '-t',
          deviceOrSimulatorName,
          'rport',
          `tcp:${currentPort}`,
          `tcp:${currentPort}`
        ]);
      });
    } catch (err) {
      if (err instanceof DescriptiveError) {
        logger.descriptiveError(
          new DescriptiveError({
            whatHappened: err.getMessage(),
            whatCanUserDo: [
              ...err.getSuggestions(),
              'Try building and running the app from DevEco studio.',
            ],
            extraData: err.getRawData().extraData,
          })
        );
      } else {
        throw err;
      }
    }
  },
};

function generateRandomString(length: number = 32): string {
  const chars: string = '0123456789abcdefghijklmnopqrstuvwxyz';
  let result: string = '';
  const randomValues = new Uint8Array(length);
  crypto.getRandomValues(randomValues);
  for (let i = 0; i < length; i++) {
    const randomIndex = randomValues[i] % chars.length;
    result += chars[randomIndex];
  }
  return result;
}

async function trySetMetroPort(filePath: string, port: number): Promise<void> {
  let content = await fs.readFile(filePath, 'utf-8');

  const reFromServerIp = /MetroJSBundleProvider\.fromServerIp\(\s*"([^"]+)"\s*,\s*(\d+)\s*\)/;
  if (reFromServerIp.test(content)) {
    content = content.replace(reFromServerIp, (_match: any, host: any) => {
      return `MetroJSBundleProvider.fromServerIp("${host}", ${port})`;
    });
    await fs.writeFile(filePath, content, 'utf-8');
    return;
  }

  const reNewProvider = /new\s+MetroJSBundleProvider\s*\(\s*\)/;
  if (reNewProvider.test(content)) {
    content = content.replace(reNewProvider, () => {
      return `MetroJSBundleProvider.fromServerIp("localhost", ${port}),\n              new MetroJSBundleProvider()`;
    });
    await fs.writeFile(filePath, content, 'utf-8');
    return;
  }

  throw new DescriptiveError({
    whatHappened: `Unable to set the specified port [${port}] for the Metro server automatically.`,
    whatCanUserDo: [
      `Please manually add "MetroJSBundleProvider.fromServerIp("localhost", ${port})" in the "new AnyJSBundleProvider" section of your ${filePath} file.
e.g:
      new AnyJSBundleProvider([
        MetroJSBundleProvider.fromServerIp("localhost", ${port}),
        new MetroJSBundleProvider(),
        new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'hermes_bundle.hbc'),
      ])`
    ],
  });
}
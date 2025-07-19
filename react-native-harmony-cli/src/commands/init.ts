#!/usr/bin/env node
/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import path from 'node:path';
import os from 'node:os';
import { Command } from 'commander';
import inquirer from 'inquirer';
import chalk from 'chalk';
import fs from 'fs-extra';
import JSON5 from 'json5';
import { getLoader, logger } from '@react-native-community/cli-tools';
import { RealCliExecutor } from '../io/CliExecutor';
import { AbsolutePath } from '../core/AbsolutePath';
import { DescriptiveError } from '../core/DescriptiveError';

const VALID_APP_NAME_REG = /^[A-Za-z][A-Za-z0-9_]*[A-Za-z0-9]$/;
const DEFAULT_PACKAGE_PREFIX = 'com.example.';
const HARMONY_DIR = 'harmony';
const JSON5_EXT = '.json5';
const DEFAULT_ENCODING = 'utf8';
const MODULE_DESC = 'module_desc';
const ENTRY_ABILITY_DESC = 'EntryAbility_desc';
const ENTRY_ABILITY_LABEL = 'EntryAbility_label';
const REACT_NATIVE = 'react-native';
const NODE_MODULES = 'node_modules';
const STRING_TYPE = 'string';
const PROJECT_NAME_INDEX = 0;
const JSON_INDENTATION_LENGTH = 2;
const GIT_EXE = 'git';
const NODE_EXE = 'node';
const NPM_EXE = 'npm';
const APP_JSON = 'app.json';
const APP_JSON5 = 'app.json5';
const STRING_JSON = 'string.json';
const PACKAGE_JSON = 'package.json';
const TEMPLATE_CONFIG_JS = 'template.config.js';
const METRO_CONFIG_JS = 'metro.config.js';
const DEFAULT_TEMPLATE_PKG = '@react-native-oh/template';
const REACT_NATIVE_OH_REACT_NATIVE_HARMONY = '@react-native-oh/react-native-harmony';
const REACT_NATIVE_OH_REACT_NATIVE_HARMONY_CLI = '@react-native-oh/react-native-harmony-cli';

interface CliOptions {
  version?: string;
  template?: string;
}

interface TemplateConfig {
  placeholderName: string;
  templateDir: string;
  postInitScript?: string;
  titlePlaceholder?: string;
}

interface UpdateConfig {
  destDir: string;
  templateRoot: string;
  templateConfig: TemplateConfig;
  projectName: string;
  packageName: string;
  isHarmonyTemplate: boolean;
  rnohVersion: string | undefined;
  rnohCliVersion: string | undefined,
  rnohTemplateConfig: TemplateConfig | undefined;
}

/**
 * Helper class for file operations (JSON and text file updates).
 */
class FileOperationHelper {
  static async updateJsonFile(filePath: string, updateFn: (data: any) => any, errorMessage: string): Promise<void> {
    if (!await fs.pathExists(filePath)) {
      return;
    }

    try {
      const content = await fs.readFile(filePath, DEFAULT_ENCODING);
      const data = JSON5.parse(content);
      const updatedData = updateFn(data);

      const outputContent = filePath.endsWith(JSON5_EXT)
        ? JSON5.stringify(updatedData, null, JSON_INDENTATION_LENGTH)
        : JSON.stringify(updatedData, null, JSON_INDENTATION_LENGTH);

      await fs.writeFile(filePath, outputContent);
    } catch (e: any) {
      throw new DescriptiveError({
        whatHappened: errorMessage,
        whatCanUserDo: ['Please check if the file format is correct.'],
        extraData: e,
      });
    }
  }

  static async updateTextFile(filePath: string, placeholder: string, replacement: string): Promise<void> {
    if (!await fs.pathExists(filePath)) {
      return;
    }

    try {
      let content = await fs.readFile(filePath, DEFAULT_ENCODING);
      if (content.includes(placeholder)) {
        content = content.replace(new RegExp(placeholder, 'g'), replacement);
        await fs.writeFile(filePath, content, DEFAULT_ENCODING);
      }
    } catch (e: any) {
      throw new DescriptiveError({
        whatHappened: `Failed to update ${path.basename(filePath)}.`,
        whatCanUserDo: ['Please check if the file is readable and writable.'],
        extraData: e,
      });
    }
  }
}

/**
 * Helper class for validation operations.
 */
class ValidationHelper {
  static validateProjectName(projectName: string): string | true {
    if (!projectName.trim()) {
      return 'Project name cannot be empty.';
    }
    if (!VALID_APP_NAME_REG.test(projectName.trim())) {
      return 'Project name must start with a letter and can only contain letters, numbers, _.';
    }
    return true;
  }

  static validateOptions(options: CliOptions): void {
    if (options.version && options.template) {
      throw new DescriptiveError({
        whatHappened: 'Cannot specify both --version and --template.',
        whatCanUserDo: ['Please specify only one of --version or --template.'],
      });
    }
  }

  static async validateReactNativeVersion(destDir: string): Promise<string> {
    const packageJsonPath = path.join(destDir, PACKAGE_JSON);
    if (await fs.pathExists(packageJsonPath)) {
      const packageJson = JSON.parse(await fs.readFile(packageJsonPath, DEFAULT_ENCODING));
      const reactNativeVersion = packageJson.dependencies?.[REACT_NATIVE];
      if (!reactNativeVersion || !/^\^?0\.(72)|(77)\./.test(reactNativeVersion)) {
        throw new DescriptiveError({
          whatHappened: `Unsupported RN community template. Please ensure react-native version starts with "0.77.".`,
          whatCanUserDo: ['Please use a template with react-native version 0.77.x.'],
        });
      }
      return reactNativeVersion;
    }
    return "0.77";
  }
}

/**
 * Main entry point for the RNOH init command.
 * Initializes a new RNOH project with proper template and configuration.
 */
export async function rnohInit() {
  const program = new Command();
  program
    .command('init [projectName]')
    .description('Initialize a new RNOH project')
    .option('-v, --version <version>', 'Harmonized RN version')
    .option('-t, --template <template>', 'Template npm package')
    .action(async (projectName: string | undefined, options: CliOptions) => {
      try {
        await runInit(projectName, options);
      } catch (err) {
        handleError(err);
        process.exit(1);
      }
    });

  await program.parseAsync(process.argv);
}

function showWelcomeMessage(version: string) {
  const rnohLogoArray = [
    '                                                              ',
    '                 ____  _   ______  __  __                     ',
    '                / __ \\/ | / / __ \\/ / / /                   ',
    '               / /_/ /  |/ / / / / /_/ /                      ',
    '              / _, _/ /|  / /_/ / __  /                       ',
    '             /_/ |_/_/ |_/\\____/_/ /_/                       ',
    '                                                              ',
  ];
  const versionInfo = version && version !== 'undefined'
    ? `              Welcome to RNOH ${version}!`
    : '                  Welcome to RNOH!';
  logger.log(`${chalk.cyan(rnohLogoArray.join('\n'))}\n${chalk.cyanBright.bold(versionInfo)}\n`);
}

async function runInit(projectName: string | undefined, options: CliOptions) {
  projectName = await getValidatedProjectName(projectName);

  ValidationHelper.validateOptions(options);

  const packageName = `${DEFAULT_PACKAGE_PREFIX}${projectName}`;

  const destDir = path.resolve(process.cwd(), projectName);
  await checkDestinationDirectory(destDir, projectName);

  showWelcomeMessage(options.version as string);
  const tempDir = await createTempDir();

  try {
    await executeInitializationSteps(tempDir, destDir, projectName, packageName, options);
  } catch (error) {
    await cleanupOnError(destDir);
    throw error;
  } finally {
    await cleanupTempDir(tempDir);
  }
}

async function getValidatedProjectName(projectName: string | undefined): Promise<string> {
  if (!projectName) {
    const { name } = await inquirer.prompt([
      {
        type: 'input',
        name: 'name',
        message: 'Please input your project name:',
        validate: ValidationHelper.validateProjectName,
      },
    ]);
    return name.trim();
  }

  const validationResult = ValidationHelper.validateProjectName(projectName);
  if (validationResult !== true) {
    throw new DescriptiveError({
      whatHappened: 'Invalid project name.',
      whatCanUserDo: [validationResult],
    });
  }

  return projectName;
}

async function checkDestinationDirectory(destDir: string, projectName: string): Promise<void> {
  if (await fs.pathExists(destDir)) {
    const { overwrite } = await inquirer.prompt([
      {
        type: 'confirm',
        name: 'overwrite',
        message: `Directory '${projectName}' already exists. Do you want to overwrite it?`,
        default: false,
      },
    ]);

    if (!overwrite) {
      process.exit(0);
    }

    try {
      await fs.remove(destDir);
    } catch (err) {
      throw new DescriptiveError({
        whatHappened: `Failed to remove existing directory ${destDir}`,
        whatCanUserDo: ['Check permissions for the directory.', 'Ensure the directory is not in use.'],
      });
    }
  }
}

async function executeInitializationSteps(
  tempDir: string,
  destDir: string,
  projectName: string,
  packageName: string,
  options: CliOptions
): Promise<void> {
  const loader = getLoader();

  loader.start(`${chalk.bold('Downloading template...')}`);
  try {
    await npmInitAndInstall(tempDir, options.template, options.version);
    loader.succeed('Template downloaded successfully!');
  } catch (err) {
    loader.fail('Failed to download template');
    if (err instanceof DescriptiveError) {
      throw err;
    }
    throw new DescriptiveError({
      whatHappened: 'Template download failed',
      whatCanUserDo: ['Check your internet connection.', 'Ensure the template package exists.'],
    });
  }

  loader.start(`${chalk.bold('Copying template...')}`);
  const { templateConfig, templateRoot } = await getTemplateConfigFromPkg(tempDir);
  let isHarmonyTemplate = true;
  let rnohTemplateConfig: TemplateConfig | undefined = undefined;
  let rnohVersion: string | undefined = undefined;
  let rnohCliVersion: string | undefined = undefined;
  try {
    await copyAndReplaceTemplate(templateConfig, templateRoot, destDir);

    const rnVersion = await ValidationHelper.validateReactNativeVersion(destDir);

    if (!(await fs.pathExists(path.join(destDir, HARMONY_DIR)))) {
      isHarmonyTemplate = false;
      ({ rnohTemplateConfig, rnohVersion, rnohCliVersion } = await handleMissingHarmonyDir(destDir, rnVersion));
    }
    loader.succeed('Template copied successfully!');
  } catch (err) {
    loader.fail('Failed to copy template');
    if (err instanceof DescriptiveError) {
      throw err;
    }
    throw new DescriptiveError({
      whatHappened: 'Template copy failed',
      whatCanUserDo: ['Ensure you have write permissions for the destination directory.'],
    });
  }

  loader.start(`${chalk.bold('Processing template...')}`);
  try {
    const updateConfig: UpdateConfig = {
      destDir,
      templateRoot,
      templateConfig,
      projectName,
      packageName,
      isHarmonyTemplate,
      rnohVersion,
      rnohCliVersion,
      rnohTemplateConfig,
    };
    await processTemplate(updateConfig);
    loader.succeed('Template processed successfully!');
  } catch (err) {
    loader.fail('Failed to process template');
    if (err instanceof DescriptiveError) {
      throw err;
    }
    throw new DescriptiveError({
      whatHappened: 'Template processing failed',
      whatCanUserDo: ['Check if the template files are corrupted.'],
    });
  }

  loader.start(`${chalk.bold('Installing dependencies...')}`);
  try {
    await runCliCommand(NPM_EXE, ['install', '--legacy-peer-deps'], destDir);
    loader.succeed('Dependencies installed successfully!');
  } catch (err) {
    loader.fail('Failed to install dependencies');
    if (err instanceof DescriptiveError) {
      throw err;
    }
    throw new DescriptiveError({
      whatHappened: 'Dependency installation failed',
      whatCanUserDo: ['Check your internet connection.'],
    });
  }

  loader.start(`${chalk.bold('Initializing git repository...')}`);
  const isSuccess = await initGitRepo(destDir);
  if (isSuccess) {
    loader.succeed('Git repository initialized successfully!');
  } else {
    loader.fail('Failed to initialize git repository.');
  }
}

async function createTempDir(): Promise<string> {
  const tempDir = path.join(os.tmpdir(), `rnoh-init-${Date.now()}-${Math.random().toString(36).slice(2)}`);
  await fs.ensureDir(tempDir);
  return tempDir;
}

async function npmInitAndInstall(tempDir: string, customTemplate: string | undefined, version?: string): Promise<void> {
  await runCliCommand(NPM_EXE, ['init', '-y'], tempDir);
  const installTarget = customTemplate ? customTemplate : version ? `${DEFAULT_TEMPLATE_PKG}@${version}` : DEFAULT_TEMPLATE_PKG;
  await runCliCommand(NPM_EXE, ['install', installTarget], tempDir);
}

async function getTemplateConfigFromPkg(tempDir: string): Promise<{ templateConfig: TemplateConfig; templateRoot: string }> {
  const pkgJsonPath = path.join(tempDir, PACKAGE_JSON);
  const packageJson = JSON.parse(await fs.readFile(pkgJsonPath, DEFAULT_ENCODING));
  const dependencies = packageJson.dependencies;
  const templateName = Object.keys(dependencies)[0];
  const templateRoot = path.join(tempDir, NODE_MODULES, ...templateName.split('/'));
  if (!await fs.pathExists(templateRoot)) {
    throw new DescriptiveError({
      whatHappened: 'Cannot find installed template package.',
      whatCanUserDo: ['Check npm install output.'],
    });
  }

  const configPath = path.join(templateRoot, TEMPLATE_CONFIG_JS);

  if (!await fs.pathExists(configPath)) {
    throw new DescriptiveError({
      whatHappened: `${TEMPLATE_CONFIG_JS} not found in template package.`,
      whatCanUserDo: ['Check the template package.'],
    });
  }
  const templateConfig: TemplateConfig = require(configPath);

  validateTemplateConfig(templateConfig);

  return { templateConfig, templateRoot };
}

function validateTemplateConfig(templateConfig: TemplateConfig): void {
  if (!templateConfig.placeholderName || !templateConfig.templateDir) {
    throw new DescriptiveError({
      whatHappened: `${TEMPLATE_CONFIG_JS} missing required fields (placeholderName, templateDir).`,
      whatCanUserDo: [`Check ${TEMPLATE_CONFIG_JS} format.`],
    });
  }

  if (typeof templateConfig.placeholderName !== STRING_TYPE || typeof templateConfig.templateDir !== STRING_TYPE) {
    throw new DescriptiveError({
      whatHappened: `${TEMPLATE_CONFIG_JS} fields must be strings.`,
      whatCanUserDo: [`Check ${TEMPLATE_CONFIG_JS} format.`],
    });
  }
}

async function copyAndReplaceTemplate(templateConfig: TemplateConfig, templateRoot: string, destDir: string): Promise<void> {
  const srcDir = path.join(templateRoot, templateConfig.templateDir);

  if (!await fs.pathExists(srcDir)) {
    throw new DescriptiveError({
      whatHappened: 'Template directory not found.',
      whatCanUserDo: [`Check templateDir in ${TEMPLATE_CONFIG_JS}.`],
    });
  }

  await fs.copy(srcDir, destDir);
}

async function updateRNConfigFiles(updateConfig: UpdateConfig): Promise<void> {
  const updates = [
    FileOperationHelper.updateJsonFile(
      path.join(updateConfig.destDir, APP_JSON),
      (data) => ({ ...data, name: updateConfig.projectName, displayName: updateConfig.projectName }),
      `Failed to update ${APP_JSON}.`
    ),
    FileOperationHelper.updateJsonFile(
      path.join(updateConfig.destDir, PACKAGE_JSON),
      (data) => {
        if (!updateConfig.isHarmonyTemplate) {
          data.dependencies = { ...data.dependencies, [REACT_NATIVE_OH_REACT_NATIVE_HARMONY]: updateConfig.rnohVersion };
          data.devDependencies = { ...data.devDependencies, [REACT_NATIVE_OH_REACT_NATIVE_HARMONY_CLI]: updateConfig.rnohCliVersion };
        }
        return { ...data, name: updateConfig.packageName };
      },
      `Failed to update ${PACKAGE_JSON}.`
    ),
  ];

  await Promise.all(updates);
}

async function renameDotFiles(destDir: string): Promise<void> {
  const dotMap: Record<string, string> = {
    '_bundle': '.bundle',
    '_eslintrc.js': '.eslintrc.js',
    '_gitignore': '.gitignore',
    '_prettierrc.js': '.prettierrc.js',
    '_watchmanconfig': '.watchmanconfig',
    '_buckconfig': '.buckconfig',
    '_flowconfig': '.flowconfig',
    '_gitattributes': '.gitattributes',
    '_editorconfig': '.editorconfig',
    '_ruby-version': '.ruby-version',
    '_node-version': '.node-version',
    '_xcode.env': '.xcode.env',
  };

  const renameOperations = Object.entries(dotMap).map(async ([src, dest]) => {
    const srcPath = path.join(destDir, src);
    if (await fs.pathExists(srcPath)) {
      await fs.move(srcPath, path.join(destDir, dest), { overwrite: true });
    }
  });

  await Promise.all(renameOperations);
}

async function runPostInitScript(scriptPath: string, templateRoot: string, cwd: string, isHarmonyTemplate: boolean): Promise<void> {
  const absScriptPath = path.join(templateRoot, scriptPath);

  if (!await fs.pathExists(absScriptPath)) {
    throw new DescriptiveError({
      whatHappened: 'postInitScript not found.',
      whatCanUserDo: [`Check postInitScript path in ${TEMPLATE_CONFIG_JS}.`],
    });
  }

  if (!isHarmonyTemplate) {
    try {
      await runCliCommand(NODE_EXE, [absScriptPath], cwd);
    } catch (err) {
      logger.error(`Failed to execute postInitScript in RN community template, please execute manually.`);
    }
  } else {
    await runCliCommand(NODE_EXE, [absScriptPath], cwd);
  }
}

async function runCliCommand(cmd: string, args: string[], cwd: string, silent = true, stdOutFunc?: (data: any) => any): Promise<void> {
  const cli = new RealCliExecutor();
  try {
    await cli.run(cmd, {
      args,
      cwd: new AbsolutePath(cwd),
      shell: true,
      onStdout: silent ? undefined : stdOutFunc ? (data) => stdOutFunc(data) : (data) => logger.info(data),
      onStderr: silent ? undefined : (data) => logger.error(data),
    });
  } catch (err) {
    throw new DescriptiveError({
      whatHappened: `Failed to run command: ${cmd} ${args.join(' ')}`,
      extraData: err,
    });
  }
}

async function initGitRepo(destDir: string): Promise<boolean> {
  const gitDir = path.join(destDir, '.git');
  if (await fs.pathExists(gitDir)) {
    logger.warn('Git repository already exists, skipping git init.');
    return false;
  }

  try {
    await runCliCommand(GIT_EXE, ['init'], destDir);
    await runCliCommand(GIT_EXE, ['add', '.'], destDir);
    await runCliCommand(GIT_EXE, ['commit', '-m', `"Initial RNOH by ${REACT_NATIVE_OH_REACT_NATIVE_HARMONY_CLI}"`], destDir);
  } catch (error) {
    logger.warn('Failed to initialize git repository. You can initialize it manually later.');
    if (await fs.pathExists(gitDir)) {
      await fs.remove(gitDir);
    }
    return false;
  }
  return true;
}

function handleError(err: unknown): void {
  if (err instanceof DescriptiveError) {
    logger.error(chalk.red(err.getMessage()));
    for (const suggestion of err.getSuggestions()) {
      logger.info(chalk.yellow('Tip: ' + suggestion));
    }
    if (err.getDetails()) {
      logger.info(chalk.gray(err.getDetails()));
    }
  } else {
    const message = err instanceof Error ? err.message : String(err);
    logger.error(chalk.red('Unexpected error: ' + message));
  }
}

async function cleanupOnError(destDir: string): Promise<void> {
  if (await fs.pathExists(destDir)) {
    await fs.remove(destDir);
  }
}

async function cleanupTempDir(tempDir: string): Promise<void> {
  try {
    await fs.remove(tempDir);
  } catch (cleanupError) {
    logger.warn('Failed to clean up temporary directory:', tempDir);
  }
}

async function getRNOHVersion(rnVersion: string): Promise<string> {
  const versionParts = rnVersion.split('.');
  const majorMinorVersion = `${versionParts[0]}.${versionParts[1]}`;
  let versionInfos = "";
  await runCliCommand(NPM_EXE, ['view', DEFAULT_TEMPLATE_PKG, 'versions', '--json'], process.cwd(), false,
  (data) => {
    versionInfos += data.toString();
  }
  );
  const versions = JSON.parse(versionInfos);
  const matchingVersions = versions.filter((v: string) => v.startsWith(majorMinorVersion));
  if (matchingVersions.length === 0) {
    throw new DescriptiveError({
      whatHappened: `No matching versions found for ${DEFAULT_TEMPLATE_PKG} with major version ${majorMinorVersion}.`,
      whatCanUserDo: ['Please check the available versions of the template package.'],
    });
  }
  return matchingVersions.pop();
}

async function handleMissingHarmonyDir(destDir: string, rnVersion: string): Promise<{ rnohTemplateConfig: TemplateConfig, rnohVersion: string, rnohCliVersion: string }> {
  const lastestVersion = await getRNOHVersion(rnVersion);
  const defaultTempDir = await createTempDir();
  try {
    await npmInitAndInstall(defaultTempDir, `${DEFAULT_TEMPLATE_PKG}@${lastestVersion}`);
    const defaultTemplateConfig = await getTemplateConfigFromPkg(defaultTempDir);

    const sourceHarmonyDir = path.join(defaultTemplateConfig.templateRoot, defaultTemplateConfig.templateConfig.templateDir, HARMONY_DIR);
    const targetHarmonyDir = path.join(destDir, HARMONY_DIR);

    if (!await fs.pathExists(sourceHarmonyDir)) {
      throw new DescriptiveError({
        whatHappened: `Default template does not contain harmony directory at ${sourceHarmonyDir}`,
        whatCanUserDo: ['Please use a template that contains harmony directory.', 'Check if the default template package is correct.'],
      });
    }

    await fs.copy(sourceHarmonyDir, targetHarmonyDir);

    const sourceMetroConfigPath = path.join(defaultTemplateConfig.templateRoot, defaultTemplateConfig.templateConfig.templateDir, METRO_CONFIG_JS);
    const targetMetroConfigPath = path.join(destDir, METRO_CONFIG_JS);
    if (await fs.pathExists(sourceMetroConfigPath)) {
      await fs.copyFile(sourceMetroConfigPath, targetMetroConfigPath);
    } else {
      throw new DescriptiveError({
        whatHappened: `Default template does not contain ${METRO_CONFIG_JS} at ${sourceMetroConfigPath}`,
        whatCanUserDo: [`Please use a template that contains ${METRO_CONFIG_JS}.`, 'Check if the default template package is correct.'],
      });
    }

    const pkgJsonPath = path.join(defaultTemplateConfig.templateRoot, defaultTemplateConfig.templateConfig.templateDir, PACKAGE_JSON);
    const pkgContent = JSON.parse(await fs.readFile(pkgJsonPath, DEFAULT_ENCODING));
    const rnohVersion = pkgContent.dependencies?.[REACT_NATIVE_OH_REACT_NATIVE_HARMONY];
    const rnohCliVersion = pkgContent.devDependencies?.[REACT_NATIVE_OH_REACT_NATIVE_HARMONY_CLI];

    const androidDir = path.join(destDir, 'android');
    const iosDir = path.join(destDir, 'ios');

    if (await fs.pathExists(androidDir)) {
      await fs.remove(androidDir);
    }
    if (await fs.pathExists(iosDir)) {
      await fs.remove(iosDir);
    }
    return { rnohTemplateConfig: defaultTemplateConfig.templateConfig, rnohVersion, rnohCliVersion };
  } catch (error) {
    if (error instanceof DescriptiveError) {
      throw error;
    }
    throw new DescriptiveError({
      whatHappened: 'Failed to handle missing harmony directory from default template.',
      whatCanUserDo: [
        'Check your internet connection.',
        'Ensure you have write permissions for the destination directory.',
        'Verify that the default template package exists and is accessible.',
      ],
      extraData: error,
    });
  } finally {
    await cleanupTempDir(defaultTempDir);
  }
}

async function updateHarmonyFiles(updateConfig: UpdateConfig): Promise<void> {
  const harmonyDir = path.join(updateConfig.destDir, HARMONY_DIR);
  const appScopeDir = path.join(harmonyDir, 'AppScope');
  const entryDir = path.join(harmonyDir, 'entry');

  const updateOperations = [
    updateAppScopeStringJson(appScopeDir, updateConfig.projectName),
    updateEntryIndexEts(entryDir, updateConfig),
    updateAppScopeAppJson5(appScopeDir, updateConfig.packageName),
    updateEntryStringJson(entryDir, updateConfig.projectName),
  ];

  await Promise.all(updateOperations);
}

async function updateAppScopeStringJson(appScopeDir: string, projectName: string): Promise<void> {
  const appScopeStringJson = path.join(appScopeDir, 'resources', 'base', 'element', STRING_JSON);

  await FileOperationHelper.updateJsonFile(
    appScopeStringJson,
    (data) => {
      if (Array.isArray(data.string) && data.string.length > 0) {
        data.string[PROJECT_NAME_INDEX].value = projectName;
      }
      return data;
    },
    `Failed to update AppScope ${STRING_JSON}.`
  );
}

async function updateEntryIndexEts(entryDir: string, updateConfig: UpdateConfig): Promise<void> {
  const entryIndexEts = path.join(entryDir, 'src', 'main', 'ets', 'pages', 'Index.ets');
  let replacedText = updateConfig.templateConfig.placeholderName;
  if (!updateConfig.isHarmonyTemplate && updateConfig.rnohTemplateConfig) {
    replacedText = updateConfig.rnohTemplateConfig.placeholderName;
  }
  await FileOperationHelper.updateTextFile(entryIndexEts, replacedText, updateConfig.projectName);
}

async function updateAppScopeAppJson5(appScopeDir: string, packageName: string): Promise<void> {
  const appScopeAppJson5 = path.join(appScopeDir, APP_JSON5);

  await FileOperationHelper.updateJsonFile(
    appScopeAppJson5,
    (data) => ({
      ...data,
      app: { ...data.app, bundleName: packageName },
    }),
    `Failed to update AppScope ${APP_JSON5}.`
  );
}

async function updateEntryStringJson(entryDir: string, projectName: string): Promise<void> {
  const entryStringJson = path.join(entryDir, 'src', 'main', 'resources', 'base', 'element', STRING_JSON);

  await FileOperationHelper.updateJsonFile(
    entryStringJson,
    (data) => {
      if (Array.isArray(data.string)) {
        data.string.forEach((item: any) => {
          if (item.name === MODULE_DESC || item.name === ENTRY_ABILITY_DESC) {
            item.value = '';
          }
          if (item.name === ENTRY_ABILITY_LABEL) {
            item.value = projectName;
          }
        });
      }
      return data;
    },
    `Failed to update entry ${STRING_JSON}.`
  );
}

async function processTemplate(
  updateConfig: UpdateConfig
): Promise<void> {
  await Promise.all([
    updateRNConfigFiles(updateConfig),
    updateHarmonyFiles(updateConfig),
    renameDotFiles(updateConfig.destDir),
  ]);

  if (updateConfig.templateConfig.postInitScript) {
    await runPostInitScript(
      updateConfig.templateConfig.postInitScript,
      updateConfig.templateRoot,
      updateConfig.destDir,
      updateConfig.isHarmonyTemplate);
  }
}

rnohInit();

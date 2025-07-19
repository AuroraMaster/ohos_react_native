/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Command } from './types';
import {
  AbsolutePath,
  DescriptiveError,
  maybeMakeDirectories,
  maybeRemoveFilesInDirectory,
} from '../core';
import { Logger, findFilePathsWithExtensions } from '../io';
import fs from 'node:fs';
import {
  ArkTSComponentCodeGeneratorCAPI,
  CppComponentCodeGenerator,
  NativeModuleCodeGenerator,
  UberSchema,
  SharedComponentCodeGenerator,
} from '../codegen';
import Case from 'case';
import { LibGlueCodeGenerator } from '../codegen/generators/LibGlueCodeGenerator';

const COMMAND_NAME = 'codegen-lib-harmony';

export const commandCodegenLibHarmony: Command = {
  name: COMMAND_NAME,
  description:
    'Generates boilerplate, types, and utilities from Turbo Module and Fabric Component specifications. Designed for library developers who want to include generated code in their libraries.',
  options: [
    {
      name: '--npm-package-name <string>',
      description:
        'Name of your React Native library. This value is processed and used as a directory name for C++ files to reduce the chances of conflicts between libraries.',
    },
    {
      name: '--turbo-modules-spec-paths [path]',
      description:
        'Path or paths to Turbo Module spec files. Each path can point to a spec file or a directory containing spec files.',
      parse: (val: string) => val.split(' '),
    },
    {
      name: '--arkts-components-spec-paths [path]',
      description:
        'Path or paths to component spec files used to generate code for components to be implemented on the ArkTS side. Each path can point to a spec file or a directory containing spec files.',
      parse: (val: string) => val.split(' '),
    },
    {
      name: '--cpp-components-spec-paths [path]',
      description:
        'Path or paths to component spec files used to generate code for components to be implemented on the C++ side. Each path can point to a spec file or a directory containing spec files.',
      parse: (val: string) => val.split(' '),
    },
    {
      name: '--cpp-output-path <path>',
      description:
        'Specifies the relative path to the output directory intended for storing generated C++ files.',
    },
    {
      name: '--ets-output-path <path>',
      description:
        'Specifies the relative path to the output directory intended for storing generated ETS files.',
    },
    {
      name: '--no-safety-check [boolean]',
      description:
        'Skips the check that prevents file operations outside the current working directory. This command permanently deletes previously generated files. Files are generated in the path specified by cpp-output-path.',
    },
  ],
  func: async (_argv, _config, rawArgs: any) => {
    const logger = new Logger();
    try {
      const { args, uberSchemaByType } =
        validateArgsAndRetrieveUberSchemaByType(rawArgs);
      const fileContentByPath = generateCodeInMemory({
        args,
        uberSchemaByType,
      });
      saveAndLogCodegenResult(args, logger, fileContentByPath);
    } catch (err) {
      if (err instanceof DescriptiveError && !err.isUnexpected()) {
        logger.descriptiveError(err);
        return;
      }
      throw err;
    }
  },
};

type RawArgs = {
  npmPackageName?: string;
  turboModulesSpecPaths?: string[];
  arktsComponentsSpecPaths?: string[];
  cppComponentsSpecPaths?: string[];
  cppOutputPath?: string;
  etsOutputPath?: string;
  safetyCheck?: boolean;
};

function validateArgsAndRetrieveUberSchemaByType(rawArgs: RawArgs) {
  const args = validateRawArgs(rawArgs);
  const arkTSComponentSpecPaths = specPathsToSpecFilePaths(
    args.arkTSComponentsSpecPaths
  );
  const turboModulesUberSchema = UberSchema.fromSpecFilePaths(
    specPathsToSpecFilePaths(args.tmSpecPaths)
  );
  const arkTSComponentsUberSchema = UberSchema.fromSpecFilePaths(
    arkTSComponentSpecPaths
  );
  const cppComponentSpecPaths = specPathsToSpecFilePaths(
    args.cppComponentsSpecPaths
  );
  const cppComponentsUberSchema = UberSchema.fromSpecFilePaths(
    cppComponentSpecPaths
  );
  const componentsUberSchema = UberSchema.fromSpecFilePaths([
    ...arkTSComponentSpecPaths,
    ...cppComponentSpecPaths,
  ]);

  return {
    args,
    uberSchemaByType: {
      turboModulesUberSchema,
      arkTSComponentsUberSchema,
      cppComponentsUberSchema,
      componentsUberSchema,
    },
  };
}

type Args = {
  npmPackageName: string;
  tmSpecPaths: AbsolutePath[];
  arkTSComponentsSpecPaths: AbsolutePath[];
  cppComponentsSpecPaths: AbsolutePath[];
  cppOutputPath: AbsolutePath;
  etsOutputPath: AbsolutePath;
  safetyCheck: boolean;
};

function validateRawArgs(rawArgs: RawArgs): Args {
  if (!rawArgs.npmPackageName) {
    throw new DescriptiveError({
      whatHappened: '--npm-package-name is required',
      whatCanUserDo: ['Please specify --npm-package-name'],
    });
  }
  if (!rawArgs.cppOutputPath) {
    throw new DescriptiveError({
      whatHappened: '--cpp-output-path is required',
      whatCanUserDo: ['Please specify --cpp-output-path'],
    });
  }
  if (!rawArgs.etsOutputPath) {
    throw new DescriptiveError({
      whatHappened: '--ets-output-path is required',
      whatCanUserDo: ['Please specify --ets-output-path'],
    });
  }
  return {
    npmPackageName: rawArgs.npmPackageName!,
    tmSpecPaths: (rawArgs.turboModulesSpecPaths ?? []).map(
      (path) => new AbsolutePath(path)
    ),
    arkTSComponentsSpecPaths: (rawArgs.arktsComponentsSpecPaths ?? []).map(
      (path) => new AbsolutePath(path)
    ),
    cppComponentsSpecPaths: (rawArgs.cppComponentsSpecPaths ?? []).map(
      (path) => new AbsolutePath(path)
    ),
    etsOutputPath: new AbsolutePath(rawArgs.etsOutputPath!),
    cppOutputPath: new AbsolutePath(rawArgs.cppOutputPath!),
    safetyCheck: !!rawArgs.safetyCheck,
  };
}

function specPathsToSpecFilePaths(specPaths: AbsolutePath[]): AbsolutePath[] {
  const SUPPORTED_SPEC_EXTENSIONS = ['js', 'jsx', 'ts', 'tsx'];
  return specPaths.flatMap((specPath) => {
    if (!fs.existsSync(specPath.getValue())) {
      throw new DescriptiveError({
        whatHappened: `No such file or directory: ${specPath.getValue()}`,
        whatCanUserDo: [`Are you sure provided specPath exists?`],
      });
    }
    if (fs.lstatSync(specPath.getValue()).isDirectory()) {
      return findFilePathsWithExtensions(specPath, SUPPORTED_SPEC_EXTENSIONS);
    } else {
      const ext = specPath.getExtension();
      if (ext && SUPPORTED_SPEC_EXTENSIONS.includes(ext)) {
        return [specPath];
      }
      return [];
    }
  });
}

function generateCodeInMemory({
  args,
  uberSchemaByType: {
    arkTSComponentsUberSchema,
    componentsUberSchema,
    cppComponentsUberSchema,
    turboModulesUberSchema,
  },
}: ReturnType<typeof validateArgsAndRetrieveUberSchemaByType>) {
  const CODEGEN_NOTICE_LINES = [
    `This code was generated by "react-native ${COMMAND_NAME}"`,
  ];
  const fileContentByPath = new Map<AbsolutePath, string>();
  const appendToFileContentByPath = (
    fileContent: string,
    path: AbsolutePath
  ) => {
    fileContentByPath.set(path, fileContent);
  };

  const turboModuleGlueCodeData = turboModulesUberSchema
    .findAllSpecSchemasByType('NativeModule')
    .map((specSchema) => {
      const nativeModuleCodeGenerator = new NativeModuleCodeGenerator(
        args.cppOutputPath.copyWithNewSegment(
          'RNOH',
          'generated',
          'turbo_modules'
        ),
        args.etsOutputPath.copyWithNewSegment('turboModules'),
        CODEGEN_NOTICE_LINES,
        '@rnoh/react-native-openharmony/ts'
      );
      nativeModuleCodeGenerator
        .generate(specSchema)
        .forEach(appendToFileContentByPath);
      return nativeModuleCodeGenerator.getGlueCodeData();
    })
    .flat();

  const arkTSComponentCodeGeneratorCAPI = new ArkTSComponentCodeGeneratorCAPI(
    args.cppOutputPath.copyWithNewSegment('RNOH', 'generated', 'components'),
    args.etsOutputPath.copyWithNewSegment('components'),
    CODEGEN_NOTICE_LINES,
    '@rnoh/react-native-openharmony/ts'
  );
  arkTSComponentCodeGeneratorCAPI
    .generate(arkTSComponentsUberSchema)
    .forEach(appendToFileContentByPath);

  const libraryCppName = deriveLibraryCppNameFromNpmPackageName(
    args.npmPackageName
  );
  const cppComponentCodeGenerator = new CppComponentCodeGenerator(
    (filename) => {
      return args.cppOutputPath.copyWithNewSegment(
        'RNOH',
        'generated',
        'components',
        filename
      );
    },
    CODEGEN_NOTICE_LINES
  );
  cppComponentCodeGenerator
    .generate({
      libraryCppName: libraryCppName,
      uberSchema: cppComponentsUberSchema,
    })
    .forEach(appendToFileContentByPath);

  new SharedComponentCodeGenerator((filename) => {
    return args.cppOutputPath.copyWithNewSegment(
      'react',
      'renderer',
      'components',
      libraryCppName,
      filename
    );
  })
    .generate({
      libraryCppName: libraryCppName,
      uberSchema: componentsUberSchema,
    })
    .forEach(appendToFileContentByPath);

  new LibGlueCodeGenerator(
    args.cppOutputPath,
    args.etsOutputPath,
    CODEGEN_NOTICE_LINES
  )
    .generate({
      libraryCppName,
      arkTSComponents: arkTSComponentCodeGeneratorCAPI.getLibGlueCodeData(),
      cppComponents: cppComponentCodeGenerator
        .getGlueCodeData()
        .map(({ name }) => ({ componentName: name })),
      turboModules: turboModuleGlueCodeData,
    })
    .forEach(appendToFileContentByPath);

  return fileContentByPath;
}

function deriveLibraryCppNameFromNpmPackageName(
  npmPackageName: string
): string {
  let result = npmPackageName;
  if (npmPackageName.includes('/')) {
    result.replace('@', '');
    result = npmPackageName.replace('/', '__');
  }
  result = Case.snake(result);
  return result;
}

function saveAndLogCodegenResult(
  args: Args,
  logger: Logger,
  fileContentByPath: ReturnType<typeof generateCodeInMemory>
) {
  saveCodegenResultToFileSystem(
    fileContentByPath,
    args.cppOutputPath,
    args.etsOutputPath,
    args.safetyCheck
  );
  logCodegenResult(logger, fileContentByPath, new AbsolutePath(''));
}

function saveCodegenResultToFileSystem(
  fileContentByPath: Map<AbsolutePath, string>,
  cppOutputPath: AbsolutePath,
  etsOutputPath: AbsolutePath,
  enableSafetyCheck: boolean
) {
  prepareDirectory(cppOutputPath, enableSafetyCheck);
  prepareDirectory(etsOutputPath, enableSafetyCheck);
  prepareDirectory(
    etsOutputPath.copyWithNewSegment('components'),
    enableSafetyCheck
  );
  prepareDirectory(
    etsOutputPath.copyWithNewSegment('turboModules'),
    enableSafetyCheck
  );
  fileContentByPath.forEach((_fileContent, path) => {
    prepareDirectory(path.getDirectoryPath(), enableSafetyCheck);
  });
  fileContentByPath.forEach((fileContent, path) => {
    fs.writeFileSync(path.getValue(), fileContent);
  });
}

function prepareDirectory(path: AbsolutePath, enableSafetyCheck: boolean) {
  maybeMakeDirectories(path);
  if (enableSafetyCheck && !path.getValue().startsWith(process.cwd())) {
    throw new DescriptiveError({
      whatHappened: `Tried to remove files in ${path.getValue()}\nand that path is outside current working directory`,
      whatCanUserDo: [
        'Run this command with --no-safety-check flag',
        'Run codegen from different location',
      ],
    });
  }
  maybeRemoveFilesInDirectory(path);
}

function logCodegenResult(
  logger: Logger,
  fileContentByPath: Map<AbsolutePath, string>,
  rootPath: AbsolutePath
) {
  const sortedRelativePathStrings = Array.from(fileContentByPath.keys()).map(
    (path) => path.relativeTo(rootPath).getValue()
  );
  sortedRelativePathStrings.sort();
  sortedRelativePathStrings.forEach((pathStr) => {
    logger.info((styles) => styles.gray(`• ${pathStr}`));
  });
  logger.info(() => '');
  logger.info(
    (styles) =>
      `Generated ${styles.green(styles.bold(fileContentByPath.size))} file(s)`
  );
  logger.info(() => '');
}

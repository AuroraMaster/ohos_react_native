/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
import { AbsolutePath } from './AbsolutePath';
import { CodegenConfig, RawCodegenConfig } from './CodegenConfig';
import { DescriptiveError } from './DescriptiveError';
import { FS } from './FileSystem';

type RawAutolinkingConfig = {
  etsPackageClassName?: string;
  cppPackageClassName?: string;
  cmakeLibraryTargetName?: string;
  ohPackageName?: string;
};

export class PackageJSON {
  static fromProjectRootPath(
    fs: FS,
    packageRootPath: AbsolutePath,
    projectRootPath: AbsolutePath
  ) {
    const packageJSONPath = packageRootPath.copyWithNewSegment('package.json');
    if (!fs.existsSync(packageJSONPath)) {
      throw new DescriptiveError({
        whatHappened: "Couldn't find 'package.json'",
        whatCanUserDo: [
          'Check if the path to project root is correct',
          'Try changing working directory to project root',
        ],
        extraData: packageJSONPath.getValue(),
      });
    }
    try {
      return new PackageJSON(
        JSON.parse(fs.readTextSync(packageJSONPath).toString()),
        packageRootPath,
        projectRootPath
      );
    } catch (err) {
      throw new DescriptiveError({
        whatHappened: "Couldn't parse package.json",
        extraData: { packageJSONPath, err },
      });
    }
  }

  private constructor(
    private rawPackageJSON: Record<string, any>,
    private packageRootPath: AbsolutePath,
    private projectRootPath: AbsolutePath
  ) {}

  get name(): string {
    return this.rawPackageJSON.name;
  }

  get version(): string {
    return this.rawPackageJSON.version;
  }

  get harmony():
    | undefined
    | {
        alias?: string;
        codegenConfig?: RawCodegenConfig | RawCodegenConfig[];
        autolinking?: RawAutolinkingConfig | null | true;
      } {
    return this.rawPackageJSON.harmony;
  }

  getCodegenConfigs(): CodegenConfig[] {
    /**
     * Shared codegenConfig is not used on purpose. Android, iOS and other platforms shouldn't
     * generate code for harmony-specific packages. The shape of this codegenConfig is different
     * that the shape of the shared config, to allow specifying which specific turbo modules or
     * fabric components should be code generated.
     */
    const rawCodegenConfig = this.rawPackageJSON.harmony?.codegenConfig;
    if (!rawCodegenConfig) {
      return [];
    }
    if (Array.isArray(rawCodegenConfig)) {
      return rawCodegenConfig.map((config) =>
        CodegenConfig.fromRawCodegenConfig(
          config,
          this.packageRootPath,
          this.projectRootPath,
          this.name
        )
      );
    }
    return [
      CodegenConfig.fromRawCodegenConfig(
        rawCodegenConfig,
        this.packageRootPath,
        this.projectRootPath,
        this.name
      ),
    ];
  }
}

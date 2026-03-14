/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AbsolutePath } from './AbsolutePath';
import { PackageJSON } from './PackageJSON';
import { FS } from '../core';

export class ProjectDependency {
  constructor(
    private fs: FS,
    private packageRootPath: AbsolutePath,
    private projectRootPath: AbsolutePath
  ) {}

  getRootPath() {
    return this.packageRootPath;
  }

  getCodegenConfigs() {
    return this.readPackageJSON().getCodegenConfigs();
  }

  /**
   * 获取 HAR 文件路径列表
   * @param mainHarPath 自定义的 HAR 扫描根目录（相对于包根目录），默认为 'harmony'
   */
  getHarFilePaths(mainHarPath?: string): AbsolutePath[] {
    // 如果指定了 mainHarPath，使用自定义路径
    if (mainHarPath) {
      const customPath = this.packageRootPath.copyWithNewSegment(mainHarPath);
      if (this.fs.existsSync(customPath)) {
        return this.fs.findFilePathsWithExtensions(customPath, ['har']);
      }
      return [];
    }

    // 默认行为：优先查找 harmony/ 目录，否则查找包根目录
    const packageHarmonyPath =
      this.packageRootPath.copyWithNewSegment('harmony');
    if (this.fs.existsSync(packageHarmonyPath)) {
      return this.fs.findFilePathsWithExtensions(packageHarmonyPath, ['har']);
    } else {
      return this.fs.findFilePathsWithExtensions(
        this.packageRootPath,
        ['har']
      );
    }
  }

  readPackageJSON() {
    return PackageJSON.fromProjectRootPath(
      this.fs,
      this.packageRootPath,
      this.projectRootPath
    );
  }
}

export class ProjectDependenciesManager {
  constructor(private fs: FS, private projectRootPath: AbsolutePath) {}

  async forEachAsync(
    cb: (dependency: ProjectDependency) => Promise<void> | void
  ) {
    await this.forEachDependencyInDirectory(
      cb,
      this.projectRootPath.copyWithNewSegment('node_modules')
    );
  }

  private async forEachDependencyInDirectory(
    cb: (dependency: ProjectDependency) => Promise<void> | void,
    directoryPath: AbsolutePath
  ) {
    if (!this.fs.existsSync(directoryPath)) {
      return;
    }
    for (let dirent of this.fs.readDirentsSync(directoryPath)) {
      if (dirent.isDirectory()) {
        if (dirent.name.startsWith('.')) {
          continue;
        } else if (dirent.name.startsWith('@')) {
          await this.forEachDependencyInDirectory(
            cb,
            directoryPath.copyWithNewSegment(dirent.name)
          );
        } else {
          const potentialDependencyRootPath = directoryPath.copyWithNewSegment(
            dirent.name
          );
          if (
            !this.fs.existsSync(
              potentialDependencyRootPath.copyWithNewSegment('package.json')
            )
          ) {
            continue;
          }
          await cb(
            new ProjectDependency(
              this.fs,
              potentialDependencyRootPath,
              this.projectRootPath
            )
          );
        }
      }
    }
  }
}

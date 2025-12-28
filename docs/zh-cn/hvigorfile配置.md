# `createRNOHModulePlugin` & `createRNOHProjectPlugin` 配置说明

本文档统一说明 RNOH 中两个核心插件的配置规则：

- `createRNOHModulePlugin`：用于配置 RNOH 模块插件行为（核心能力：Metro 调试、代码生成、原生模块自动链接）；
- `createRNOHProjectPlugin`：用于配置 RNOH 工程级插件行为（核心能力：JS Bundle 打包）。

两个插件均包含 `nodeModulesPath` 配置项（参数名相同但各自独立），其余配置项为各自专属，具体划分如下：

| 配置项            | 归属插件                                                       | 核心能力             |
| ----------------- | -------------------------------------------------------------- | -------------------- |
| `nodeModulesPath` | `createRNOHModulePlugin`/`createRNOHProjectPlugin`（各自独立） | 定位 RN 工程依赖目录 |
| `metro`           | `createRNOHModulePlugin`                                       | Metro 调试端口转发   |
| `codegen`         | `createRNOHModulePlugin`                                       | ETS/C++ 代码生成     |
| `autolinking`     | `createRNOHModulePlugin`                                       | 原生模块自动链接     |
| `bundler`         | `createRNOHProjectPlugin`                                      | JS Bundle 打包       |

---

## 前置说明

### 1. 路径基准规则

本文档中所有**相对路径配置项**（如 `etsOutputPath`、`projectRootPath`、`bundleOutput` 等），均以**当前配置文件所在的Harmony工程根目录**为基准，而非配置文件自身的目录。

### 2. 插件导入路径

两个插件均从 `@rnoh/hvigor-plugin` 包导入，该包的实际物理路径通常位于本地 hvigor 缓存目录（不同用户路径略有差异）：

```ts
// 统一导入方式
import {
  createRNOHModulePlugin,
  createRNOHProjectPlugin,
} from "@rnoh/hvigor-plugin"
```

示例物理路径：`C:\Users\UserName\.hvigor\project_caches\project\workspace\node_modules\@rnoh\hvigor-plugin`

### 3. 插件执行时机

- `createRNOHModulePlugin`：在工程 `sync`（同步/初始化）阶段执行，主要完成模块初始化、代码生成、自动链接等前置准备工作；
- `createRNOHProjectPlugin`：在工程**编译构建 App** 阶段执行，主要完成 JS Bundle 打包等构建相关操作。

---

## 一、`nodeModulesPath` 配置项（插件各自独立）

两个插件均包含 `nodeModulesPath` 配置项，**参数名相同但取值和作用域相互独立**，并非共享配置：

- 类型：`string`
- 作用：指定 React Native 工程的 `node_modules` 目录位置，用于：
  - 定位 npm 包；
  - 推导 Harmony 工程与 RN 工程的相对位置。
- 默认值：`../node_modules`（以Harmony工程根目录为基准的上一级目录）
- 使用建议：
  - 当 RN 工程与 Harmony 工程不在典型的「同级目录结构」时，建议显式配置为**根路径起始的完整路径**或准确的相对路径；
  - 若两个插件需指向同一 RN 工程的 `node_modules`，需手动配置相同值，而非依赖“共享”逻辑。

---

## 二、`createRNOHModulePlugin` 专属配置

### 顶层配置结构：`RNOHModulePluginOptions`

```ts
export type RNOHModulePluginOptions = {
  /** RN 工程 node_modules 目录路径（插件独立配置） */
  nodeModulesPath?: string

  /** Metro hdc转发配置 */
  metro?: MetroConfig | null

  /**
   * 代码生成配置；为 null 时表示显式禁用。
   * 若不为 null，必须至少指定 etsOutputPath 或 rnohModulePath 之一。
   */
  codegen: CodegenConfig | null

  /** 原生自动链接配置；为 null 时表示显式禁用 */
  autolinking?: AutolinkingConfig | null
}
```

### 1. Metro 调试配置：`metro`

```ts
export type MetroConfig = {
  port?: number
}
```

#### 功能说明

`metro` 字段用于配置 **通过 hdc 进行端口转发（`hdc rport`）** 的行为，方便在设备/模拟器无法直连开发机时，把设备上的端口转发到本机，从而访问本机正在运行的 Metro 服务。

> 注意：
>
> - 这里的 `metro` **不会启动 Metro 服务**，也**不会决定应用连接哪个端口**；
> - 它只影响是否执行 `hdc rport tcp:<port> tcp:<port>` 这条命令，以及用哪个端口去转发。

#### 字段说明

- `port?: number`
  - 作用：指定要通过 `hdc rport` 转发的端口号（设备端口和本机端口相同）。
  - 默认值：`8081`
  - 使用示例：
    ```ts
    metro: {
      port: 8082, // 将设备上的 8082 端口转发到本机的 8082
    }
    ```
  - 通常应与 **实际 Metro 服务监听的端口** 保持一致（例如用 `react-native start --port 8082` 启动的端口），但真正的监听端口仍由 Metro 启动命令决定，而不是这里。

#### 启用 / 禁用规则

- 不写（`undefined`）或写成空对象 `{}`：
  - 表示**开启端口转发功能**，并使用默认端口 `8081`；
  - 等价于执行：
    ```bash
    hdc rport tcp:8081 tcp:8081
    ```
- 显式写 `metro: { port: xxx }`：
  - 表示开启端口转发，并使用指定的端口 `xxx`：
    ```bash
    hdc rport tcp:xxx tcp:xxx
    ```
- 显式写 `metro: null`：
  - 表示不会执行任何 `hdc rport` 命令；
  - 不会影响应用是否去连接某个正在运行的 Metro 服务（只是不再由这个插件帮做端口映射）。

### 2. 代码生成配置：`codegen`

```ts
export type CodegenConfig = {
  /** RNOH 模块路径（兼容旧用法，可选） */
  rnohModulePath?: string

  /** ETS 代码生成输出目录 */
  etsOutputPath?: string

  /** C++ 代码生成输出目录 */
  cppOutputPath?: string

  /** RN 工程根目录，用于 codegen 的上下文定位 */
  projectRootPath?: string

  /** 是否输出调试信息 */
  debug?: boolean

  /** 是否跳过安全检查 */
  noSafetyCheck?: boolean
}
```

#### 功能说明

`codegen` 用于配置根据 JS/TS 代码生成 ETS / C++ 胶水代码的行为。  
它会在指定的 RN 工程目录中，调用：

- `node_modules/.bin/react-native codegen-harmony`

来生成 Harmony 侧与 RN 模块对应的接口定义和实现骨架。

#### 启用 / 禁用规则

- `codegen: null`：
  - 显式禁用代码生成功能；
  - 不会执行任何 `react-native codegen-harmony` 命令。
- `codegen: { ... }`：
  - 启用代码生成，并根据提供的参数决定输出位置与行为；
  - 启用时需满足：
    - `etsOutputPath` 或 `rnohModulePath` 至少设置一个；
    - 且能够在 `projectRootPath`（或其默认值）下找到 `node_modules/.bin/react-native`。

#### 字段说明

##### `rnohModulePath?: string`

- 作用：指定 RNOH 模块的入口路径，用于推导 ETS 输出路径（兼容旧的使用习惯）；
- 一般推荐**直接配置 `etsOutputPath`**，`rnohModulePath` 可在迁移阶段使用；
- 当只配置 `rnohModulePath` 而未显式配置 `etsOutputPath` 时，工具会基于该路径推导 ETS 输出目录。

##### `etsOutputPath?: string`

- 作用：指定生成的 ETS 代码输出目录；
- 必填条件：当 `codegen` 启用时（不为 `null`），**`etsOutputPath` 或 `rnohModulePath` 至少设置一个**；
- 默认值（逻辑默认）：`codegen.rnohModulePath` || `./`；
- 典型配置：
  ```ts
  codegen: {
    etsOutputPath: './entry/src/main/ets/generated',
  }
  ```

##### `cppOutputPath?: string`

- 作用：指定生成的 C++ 绑定代码输出目录；
- 默认值（逻辑默认）：`./entry/src/main/cpp/generated`；
- 典型配置：
  ```ts
  codegen: {
    etsOutputPath: './entry/src/main/ets/generated',
    cppOutputPath: './entry/src/main/cpp/generated',
  }
  ```

##### `projectRootPath?: string`

- 作用：指定 RN 工程的根目录，用于代码生成时定位工程上下文（JS/TS 源码、`node_modules` 目录、可执行文件）；
- 内部行为：在 `projectRootPath` 下执行 `node_modules/.bin/react-native codegen-harmony`，因此该路径必须包含 `node_modules` 且已安装依赖；
- 默认行为：未配置时，使用 Harmony 工程默认推断路径作为 RN 工程根目录；
- 常见使用场景（RN 与 Harmony 工程分离）：
  ```ts
  codegen: {
    etsOutputPath: './entry/src/main/ets/generated',
    projectRootPath: '../react-native-app', // 相对 Harmony 工程根目录的 RN 工程路径
  }
  ```

##### `debug?: boolean`

- 作用：是否输出更详细的调试日志（排查 codegen 问题）；
- 默认值：`false`。

##### `noSafetyCheck?: boolean`

- 作用：跳过某些安全检查（路径合法性、环境检测等）（如 codgen 命令报错文件操作权限相关可以设置为 true），仅高级场景/CI 环境慎用；
- 默认值：`false`。

### 3. 自动链接配置：`autolinking`

```ts
export type AutolinkingConfig = {
  ohPackagePath?: string
  etsRNOHPackagesFactoryPath?: string
  cppRNOHPackagesFactoryPath?: string
  cmakeAutolinkPath?: string
  excludeNpmPackages?: string[]
  includeNpmPackages?: string[]
}
```

#### 功能说明

`autolinking` 用于自动扫描 RN 项目中与 Harmony 相关的原生模块，并生成相应的绑定代码 / CMake 配置，避免手动维护重复配置。

#### 字段说明

| 字段                                  | 作用                                                    | 默认值                                         |
| ------------------------------------- | ------------------------------------------------------- | ---------------------------------------------- |
| `ohPackagePath?: string`              | 指定 `oh-package.json5` 的位置                          | `./oh-package.json5`                           |
| `etsRNOHPackagesFactoryPath?: string` | 指定 `RNOHPackagesFactory.ets` 生成位置（ETS 层注册包） | `./entry/src/main/ets/RNOHPackagesFactory.ets` |
| `cppRNOHPackagesFactoryPath?: string` | 指定 `RNOHPackagesFactory.h` 生成位置（C++ 层注册包）   | `./entry/src/main/cpp/RNOHPackagesFactory.h`   |
| `cmakeAutolinkPath?: string`          | 指定 `autolinking.cmake` 生成位置（构建配置）           | `./entry/src/main/cpp/autolinking.cmake`       |
| `excludeNpmPackages?: string[]`       | 指定不参与 Autolinking 的第三方库列表                   | []                                             |
| `includeNpmPackages?: string[]`       | 指定强制参与 Autolinking 的第三方库列表                 | []                                             |

#### 启用 / 禁用规则

- 不写或写为 `{}`：启用 Autolinking，所有路径走默认值；
- 显式写 `autolinking: null`：完全禁用 autolinking 行为。

### 三、`createRNOHProjectPlugin` 专属配置

#### 顶层配置结构：`RNOHProjectPluginOptions`

```ts
export type RNOHProjectPluginOptions = {
  /** RN 工程 node_modules 目录路径（插件独立配置） */
  nodeModulesPath?: string

  /** JS Bundle 打包配置；为 null 时表示显式禁用 */
  bundler?: BundlerConfig | null
}
```

#### 1. 打包配置：`bundler`

```ts
export type BundlerConfig = {
  /** 是否启用 bundler 任务 */
  enabled?: boolean
  /** 控制打包模式（开发/生产） */
  dev?: boolean
  /** 指定 bundle 入口文件 */
  entryFile?: string
  /** 指定 Metro 配置文件路径 */
  config?: string
  /** 指定 JS Bundle 输出路径 */
  bundleOutput?: string
  /** 指定 JS 引擎（仅支持 "any" / "hermes"） */
  jsEngine?: string
  /** 指定静态资源输出目录 */
  assetsDest?: string
  /** 指定 Source Map 输出路径 */
  sourcemapOutput?: string
  /** 控制是否压缩 JS 代码 */
  minify?: boolean
  /** Hermes 编译器额外参数（数组形式，最终拼接为字符串传递） */
  hermescOptions?: string
}
```

##### 功能说明

`bundler` 用于配置在构建 Harmony 工程时的 **JS Bundle 打包行为**。  
插件会在指定的 RN 工程目录中，调用：

- `node_modules/.bin/react-native bundle-harmony`

来完成：

- 根据入口文件打包 JS/TS 代码（支持生成 Hermes 字节码 HBC 或普通 JS Bundle）；
- 拷贝图片、字体等静态资源到指定目录；
- 生成 Source Map（可选）；
- 基于 Hermes 引擎编译优化 Bundle（可选）。

##### 启用 / 禁用规则

###### 顶层 `bundler` 配置

- `bundler: null`：显式禁用打包功能，不会执行任何打包子任务；
- `bundler: { ... }`：启用打包配置，根据字段控制 `bundle-harmony` 行为。

###### 任务执行条件（hvigor 侧）

即使配置了 `bundler: { ... }`，`CreateJSBundle` 任务执行还需满足：

```ts
if (
  !this.input.nodeModulesPath ||
  !bundlerOptions.enabled ||
  buildMode !== "release"
) {
  this.logger.warn(`[bundler] skipped`)
  return
}
```

即需同时满足：

1. `nodeModulesPath` 路径存在；
2. `bundler.enabled === true`（默认 `true`）；
3. 当前构建模式为 `release`。

##### 字段说明

| 字段                       | 作用                                | 默认值                                                                                                                                                                                     | 补充说明                                                                                                                                                                                                     |
| -------------------------- | ----------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `enabled?: boolean`        | 控制是否启用 bundler 任务           | `true`                                                                                                                                                                                     | 执行前会被 `delete` 移除，仅用于任务内部逻辑判断，不传递给 CLI                                                                                                                                               |
| `dev?: boolean`            | 控制打包模式（开发/生产）           | `"false"`（字符串类型，若传入布尔值会自动转为字符串）                                                                                                                                      | 底层 CLI 侧默认值为 `true`，插件层会覆盖为 `"false"`；`true` 为开发模式（未压缩、含调试信息），`false` 为生产模式                                                                                            |
| `entryFile?: string`       | 指定 bundle 入口文件                | `index.js`（CLI 底层默认值）                                                                                                                                                               | 例如可配置为 `index.tsx`、`src/main.tsx` 等，路径会被自动标准化                                                                                                                                              |
| `config?: string`          | 指定 Metro 配置文件路径             | 无（CLI 底层会自动查找 `metro.config.js`）                                                                                                                                                 | 配置后会自动转换为**相对于 RN 工程根目录**的路径传递给 CLI                                                                                                                                                   |
| `bundleOutput?: string`    | 指定 Bundle 输出路径（JS/HBC）      | CLI 底层默认规则：<br/>- `jsEngine: "hermes"` → `./harmony/entry/src/main/resources/rawfile/hermes_bundle.hbc`<br/>- other → `./harmony/entry/src/main/resources/rawfile/bundle.harmony.js` | 配置后会自动转换为**相对于 RN 工程根目录**的路径传递给 CLI；若路径以 `.hbc` 结尾，强制生成 Hermes 字节码                                                                                                     |
| `jsEngine?: string`        | 指定 JS 引擎                        | `"any"`（CLI 底层默认值）                                                                                                                                                                  | 仅支持 `"any"` / `"hermes"`；<br/>- `"any"`：生成普通 JS Bundle；<br/>- `"hermes"`：默认生成 HBC 字节码（可被 `bundleOutput` 覆盖后缀）                                                                      |
| `assetsDest?: string`      | 指定静态资源（图片/字体等）输出目录 | `./harmony/entry/src/main/resources/rawfile/assets`（CLI 底层默认值）                                                                                                                      | 配置后会自动转换为**相对于 RN 工程根目录**的路径传递给 CLI                                                                                                                                                   |
| `sourcemapOutput?: string` | 指定 Source Map 输出路径            | 无                                                                                                                                                                                         | 配置后会自动转换为**相对于 RN 工程根目录**的路径传递给 CLI；仅配置此参数才会生成 Source Map                                                                                                                  |
| `minify?: boolean`         | 控制是否压缩/混淆 JS 代码           | CLI 底层默认规则：`minify = !dev`（即生产模式默认 `true`）                                                                                                                                 | 显式配置会覆盖底层默认逻辑；`true` 减小包体积，`false` 便于调试                                                                                                                                              |
| `hermescOptions?: string`  | Hermes 编译器额外参数               | `"O"`（插件层默认值）；CLI 底层无默认值                                                                                                                                                    | 仅在 `jsEngine: "hermes"` 时生效；插件层传入的字符串会被拆分为数组（如 `"O g"` → `["O", "g"]`），底层会拼接为 `-O -g` 传递给 hermesc；支持参数如 `O`（优化级别）、`g`（生成调试信息）、`reuse-prop-cache` 等 |

> 关键路径处理逻辑：
> `config`/`bundleOutput`/`assetsDest`/`sourcemapOutput` 配置的路径会被插件自动转换为**相对于 RN 工程根目录**的路径（RN 工程根目录 = `nodeModulesPath` 向上推导一级），再传递给 `react-native bundle-harmony` 命令。

> JS 引擎核心逻辑补充：
>
> 1. `jsEngine: "hermes"` 仅改变默认输出文件名（`hermes_bundle.hbc`），但 `bundleOutput` 配置会覆盖该默认名；
> 2. 无论 `jsEngine` 配置为何，只要 `bundleOutput` 路径以 `.hbc` 结尾，就会强制生成 Hermes 字节码；
> 3. 生成 HBC 时，底层会自动查找 `./node_modules/react-native/sdks/hermesc/` 下对应系统的 hermesc 可执行文件（macOS: osx-bin/hermesc、Linux: linux64-bin/hermesc、Windows: win64-bin/hermesc.exe）。

##### 典型配置示例

```ts
// 生产环境配置（Hermes 引擎，生成 HBC 字节码）
bundler: {
  enabled: true, // 显式启用（默认true可省略）
  dev: false,    // 生产模式（默认false可省略）
  minify: true,  // 压缩代码（生产模式默认true）
  jsEngine: 'hermes', // 指定Hermes引擎
  config: "../metro.config.js", // Metro配置文件（相对Harmony工程根目录）
  entryFile: "index.js", // 入口文件
  bundleOutput: "./entry/src/main/resources/rawfile/hermes_bundle.hbc", // 生成HBC字节码
  assetsDest: "./entry/src/main/resources/rawfile/assets", // 静态资源输出路径
  sourcemapOutput: "./entry/src/main/resources/rawfile/hermes_bundle.map", // SourceMap路径
  hermescOptions: "O" // Hermes编译参数
}

// 生产环境配置（普通 JS Bundle）
bundler: {
  jsEngine: 'any', // 显式指定普通引擎（默认值可省略）
  bundleOutput: "./entry/src/main/resources/rawfile/bundle.harmony.js", // 生成普通JS Bundle
  assetsDest: "./entry/src/main/resources/rawfile/assets",
}

// 开发环境配置（精简）
bundler: {
  enabled: true,
  dev: true,     // 开发模式（禁用压缩、保留调试信息）
  minify: false, // 显式禁用压缩（dev=true时默认false）
}

// 禁用打包功能
bundler: null
```

### 补充说明（基于底层源码执行逻辑）

1. **参数透传规则**：

   - `enabled` 仅用于插件内部判断是否执行打包任务，不会传递给 `react-native bundle-harmony` 命令；
   - `dev` 会被自动转换为字符串类型（如 `true` → `"true"`）后传递给 CLI，插件层默认值 `"false"` 会覆盖 CLI 底层的 `true`；
   - `hermescOptions` 插件层传入的字符串会被拆分为数组（如 `"O g"` → `["O", "g"]`），底层会拼接为 `-O -g` 传递给 hermesc；
   - 路径类参数（`config`/`bundleOutput` 等）会被转换为相对 RN 工程根目录的路径，避免跨目录路径问题。

2. **Hermes 字节码生成逻辑**：

   - 生成 HBC 时，底层会先将 JS Bundle 写入临时文件（系统临时目录 + 随机名）；
   - 调用对应系统的 hermesc 可执行文件编译临时 JS 文件为 HBC；
   - 编译完成后自动删除临时 JS 文件，仅保留最终 HBC 文件；
   - 若找不到 hermesc 可执行文件，会抛出明确错误并提示用户通过 `--hermesc-dir` 指定路径（插件层已默认配置为 `./node_modules/react-native/sdks/hermesc/`）。

3. **执行上下文**：
   打包命令的执行目录（`cwd`）固定为 RN 工程根目录（`nodeModulesPath` 向上推导一级），确保 `node_modules/.bin/react-native` 能正确执行。

4. **日志与错误处理**：
   - 打包成功会输出 `[bundler] done generating bundle`；
   - 打包失败会输出 `[bundler] failed: 具体错误信息`；
   - 未满足执行条件会输出 `[bundler] skipped` 并跳过打包；
   - 生成 HBC 时若找不到 hermesc，会输出描述性错误并提示解决方案。

### 总结

1. **默认值核心要点**：

   - 插件层默认值：`enabled: true`、`dev: "false"`、`hermescOptions: "O"`；
   - CLI 底层默认值：`entryFile: "index.js"`、`jsEngine: "any"`、`assetsDest: "./harmony/entry/src/main/resources/rawfile/assets"`；
   - `bundleOutput` 底层默认值随 `jsEngine` 变化，`minify` 底层默认值随 `dev` 变化。

2. **JS 引擎关键规则**：

   - 仅支持 `"any"` / `"hermes"` 两个取值；
   - `bundleOutput` 后缀为 `.hbc` 时，无论 `jsEngine` 配置为何，都会强制生成 Hermes 字节码；
   - `hermescOptions` 仅在生成 HBC 时生效，普通 JS Bundle 会忽略该参数。

3. **路径处理**：
   所有路径配置均基于Harmony工程根目录，插件自动转换为 RN 工程根目录相对路径，无需手动调整。

4. **执行触发**：
   仅在 `release` 构建模式、`nodeModulesPath` 存在、`enabled: true` 三个条件同时满足时，才会执行打包任务。

---

### 最终总结

1. **插件分工与时机**：`createRNOHModulePlugin` 在工程 sync 阶段执行（模块初始化），`createRNOHProjectPlugin` 在编译构建阶段执行（打包）；
2. **路径基准**：所有相对路径均以Harmony工程根目录为基准，而非配置文件目录；
3. **插件导入**：两个插件均从 `@rnoh/hvigor-plugin` 导入（实际路径在本地 hvigor 缓存目录）；
4. **配置项独立性**：`nodeModulesPath` 为两个插件各自独立的配置项（仅参数名相同），并非共享配置；
5. **核心差异**：`bundler` 仅属于 `createRNOHProjectPlugin`，`metro`/`codegen`/`autolinking` 仅属于 `createRNOHModulePlugin`；
6. **禁用规则**：所有配置项设为 `null` 可显式禁用对应功能，空对象 `{}` 则启用默认行为。

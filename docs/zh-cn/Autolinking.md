# Autolinking

**Autolinking**简单的说就是通过在RN前端工程中执行：
```shell
npm install <library-with-native-dependencies> --save
```
之后会在自动发现native端的代码或模块，然后安装依赖并编译。关于Autolinking的更多介绍可以查阅下面的文档：
- [Autolinking](https://github.com/react-native-community/cli/blob/main/docs/autolinking.md)
- [Linking Libraries for ios](https://reactnative.dev/docs/linking-libraries-ios#automatic-linking)
- [Android Auto-linking for New Architecture libraries](https://reactnative.dev/blog/2022/09/05/version-070#android-auto-linking-for-new-architecture-libraries)

在RNOH项目中，为了实现 `Autolinking` 功能，需要依赖RNOH框架、RN三方库以及RNOH项目一起配合。下文会分3种角色来讲述各自所负责的内容或需适配的工作，您可根据自己的角色来选择阅读对应的章节。

另外，您也可以通过运行 [AutolinkingSample](../Samples/AutolinkingSample/README_zh.md) 来体验整个过程。

## RNOH框架（框架维护者）

0.72分支从0.72.101版本（对应 `@react-native-oh/react-native-harmony-cli` 版本是0.0.40）开始可使用完整的 Autolinking 功能。

在这个功能上，RNOH框架主要是向开发者提供一个 `@rnoh/hvigor-plugin` 的 hvigor 插件（它位于RNOH前端工程 `node_modules/@react-native-oh/react-native-harmony-cli/harmony` 目录下），这个插件会通过调用 `@react-native-oh/react-native-harmony-cli` 中的 `link-harmony` 命令完成 har 包的安装、so的链接以及Package注册等手动链接所需执行的操作。

### `link-harmony` 命令的参数

`link-harmony` 命令可单独执行，以下是 `link-harmony` 命令的参数：

- **--harmony-project-path \<path\>**: 指定 `harmony` 工程的相对路径，默认为 `./harmony`；

- **--node-modules-path \<path\>**: 指定 `node_modules` 的相对路径，默认为 `./node_modules`；

- **--cmake-autolink-path-relative-to-harmony \<path\>**: 指定 `autolinking.cmake` 生成的位置，`autolinking.cmake` 会生成在 `harmony` 工程中，默认为 `./entry/src/main/cpp/autolinking.cmake`；

- **--cpp-rnoh-packages-factory-path-relative-to-harmony \<path\>**: 指定 `RNOHPackagesFactory.h` 生成的位置，`RNOHPackagesFactory.h` 会生成在 `harmony` 工程中，默认为 `./entry/src/main/cpp/RNOHPackagesFactory.h`；

- **--ets-rnoh-packages-factory-path-relative-to-harmony \<path\>**: 指定 `RNOHPackagesFactory.ets` 生成的位置，`RNOHPackagesFactory.ets` 会生成在 `harmony` 工程中，默认为 `./entry/src/main/ets/RNOHPackagesFactory.ets`；

- **--oh-package-path-relative-to-harmony \<path\>**: 指定 `oh-package.json5` 的位置，`link-harmony` 命令会修改该文件以安装三方库的har包，默认为 `./oh-package.json5`；

- **--exclude-npm-packages \[string...\]**: 指定不执行 `Autolinking` 的三方库列表。默认情况下 `link-harmony` 命令会尝试链接所有的三方库，但某些情况下可能会因三方库配置错误而导致链接后编译报错，此时可通过这个参数将该包排除。这个参数不可以和 `--included-npm-packages` 一起使用；

- **--include-npm-packages \[string...\]**: 指定需要执行 `Autolinking` 的三方库列表。默认情况下 `link-harmony` 命令会尝试链接所有的三方库。这个参数不可以和 `--excluded-npm-packages` 一起使用。

## RN三方库（库开发者）

对于RN三方库的开发者，如果期望 `link-harmony` 命令能自动链接自己的三方库，会涉及到少量适配工作的。以下是主要的改动点，也可以参照 [AutolinkingSample](../Samples/AutolinkingSample/README_zh.md) 中的 `third-party-library-sample`。

### `package.json` 的改动

库开发者需要在 `package.json` 中配置 `harmony.autolinking`。`harmony.autolinking` 通常需要传入一个对象，属性说明如下：

- **etsPackageClassName**：ets包的类名，如果不指定该字段，会默认根据 `package.json` 中的 `name` 字段生成，生成规则为：[组织名]+[包名]+Package，如：`third-party-library-sample` 会转换成 `ThirdPartyLibrarySamplePackage`；

- **cppPackageClassName**：cpp包的类名，如果不指定该字段，会默认根据 `package.json` 中的 `name` 字段生成，生成规则同 `etsPackageClassName`；

- **cmakeLibraryTargetName**：动态链接库的名字，如果不指定该字段，会默认根据 `package.json` 中的 `name` 字段生成，生成规则为：rnoh__+[组织名]+[包名]，如：`third-party-library-sample` 会转换成 `rnoh__third_party_library_sample`；

- **ohPackageName**：ohos 模块的名字，推荐使用 har 包中 `oh-package.json5` 的 `name` 字段，部分场景中DevEco会严格校验这个名称。如果不指定该字段，会默认根据 `package.json` 中的 `name` 字段生成，生成规则为：@rnoh/+[组织名]+--+[包名]，如：`third-party-library-sample` 会转换成 `@rnoh/third-party-library-sample`；

如果您的代码中以上4个名称都符合默认规则，可将 `harmony.autolinking` 设置成 `true`。

### har 包的要求

1. ohos 模块需要有一个默认的导出

    如 [AutolinkingSample](../Samples/AutolinkingSample/README_zh.md) 中的 `third-party-library-sample/harmony/library` 一样，需要在 `index.ets` 文件中有一个默认的导出，如：

    ```ts
    export { ThirdPartyLibrarySamplePackage as default } from './src/main/ets/ThirdPartyLibrarySamplePackage';
    ```

2. har 包需要放在 `[三方库]/harmony` 目录下

### 注册自定义TurboModule和ArkTS组件

在之前的版本中，自定义ArkTS组件需要开发者在创建 RNInstance 时通过 `wrappedCustomRNComponentBuilder` 参数传入，这并不符合 Autolinking 的设计，为此 RNOH 框架新增了RNOHPackage类，库的开发者可通过继承RNOHPackage类来设计自己的ets Package类：

- 通过重写 `getUITurboModuleFactoryByNameMap` 方法注册模块中的自定义TurboModule；

- 通过重写 `getAnyThreadTurboModuleFactoryByNameMap` 方法来注册模块中的自定义WorkerTurboModule；

- 通过重写 `createWrappedCustomRNComponentBuilderByComponentNameMap` 方法来注册模块中的自定义ArkTS组件。

## RNOH项目（使用RNOH框架和RN三方库的开发者）

除了RN三方库需要适配之外，RNOH项目也是需要改动的，开发者可以参照着 [AutolinkingSample](../Samples/AutolinkingSample/README_zh.md)/NativeProject 来阅读下面的指导。

1. 安装 `@rnoh/hvigor-plugin` 插件（插件的位置见上文）：

    ```diff
      // NativeProject/hvigor/hvigor-config.json5
      {
        "modelVersion": "5.0.0",
        "dependencies": {
    +     "@rnoh/hvigor-plugin": "file:../../ReactProject/node_modules/@react-native-oh/react-native-harmony-cli/harmony/rnoh-hvigor-plugin-0.3.0.tgz"
        },
    ```

2. 添加插件：
   这一步主要涉及修改 `hvigorfile.ts` 文件（具体是哪个模块中的 `hvigorfile.ts` 根据自己项目情况而定），在这个文件中首先是需要从 `@rnoh/hvigor-plugin` 中导入 `createRNOHModulePlugin` 函数，然后通过 `createRNOHModulePlugin` 函数来创建一个plugin实例，如：
    ```ts
    // NativeProject/entry/hvigorfile.ts
    import { hapTasks } from '@ohos/hvigor-ohos-plugin';
    import { createRNOHModulePlugin } from "@rnoh/hvigor-plugin"

    export default {
      system: hapTasks,
      plugins: [
        createRNOHModulePlugin({
          nodeModulesPath: "../ReactProject/node_modules", // node_modules 文件夹相对于 harmony 工程的路径，默认为../node_modules
          codegen: null, // 不涉及 codegen，则配置成null
          autolinking: {}
        }),
      ]
    }
    ```
    `createRNOHModulePlugin` 的配置后续会出相关文档说明，这里先主要讲解一下autolinking的参数配置。

    在这个[Sample](../Samples/AutolinkingSample/README_zh.md)中，给 autolinking 传入的是一个空对象，但实际的项目中可能会涉及额外的配置，autolinking 对象的属性与 `link-harmony` 命令的参数是有关联的，详细的说明如下：

    - **ohPackagePath**：指定 `oh-package.json5` 的位置，默认为 `./oh-package.json5`；

    - **etsRNOHPackagesFactoryPath**：指定 `RNOHPackagesFactory.ets` 生成的位置，默认为 `./entry/src/main/ets/RNOHPackagesFactory.ets`；

    - **cppRNOHPackagesFactoryPath**：指定 `RNOHPackagesFactory.h` 生成的位置，默认为 `./entry/src/main/cpp/RNOHPackagesFactory.h`；

    - **cmakeAutolinkPath**：指定 `autolinking.cmake` 生成的位置，默认为 `./entry/src/main/cpp/autolinking.cmake`;

    - **excludeNpmPackages**：指定不执行 `Autolinking` 的三方库列表；

    - **includeNpmPackages**: 指定需要执行 `Autolinking` 的三方库列表。

    添加完成后，会由hvigor插件配合@react-native-oh/react-native-harmony-cli自动完成以下文件的添加和内容修改：

    - 新增RNOHPackagesFactory.ets、RNOHPackagesFactory.h和autolinking.cmake文件，用来完成so的链接和Package的注册。
    - 在指定的oh-package.json5中添加依赖，完成har包的安装。

3. 添加AutoLinking配置：
<br>CMakeLists.txt文件中增加autolink配置：

    ```ts
    include("${CMAKE_CURRENT_SOURCE_DIR}/autolinking.cmake")
    autolink_libraries(rnoh_app)
    ```

    PackageProvider.cpp文件中增加autolink配置：

    - 导入RNOHPackagesFactory.h中的package对象；

    - 合并手动link的package。
  
    ```cpp
    #include "RNOH/PackageProvider.h"
    #include "RNOHPackagesFactory.h"
    ...

    using namespace rnoh;

    std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(
        Package::Context ctx) {
      const std::vector<std::shared_ptr<Package>> ManualLinkingPackage = {
        ...
      }; // 手动link的package
      auto packages = createRNOHPackages(ctx); // autolinking
      for (const auto& pkg : ManualLinkingPackage) {
        packages.push_back(pkg); // 逐一添加手动link的package
      }
      return packages;
    }
    ```

    RNPackagesFactory.ets文件中增加autolink配置：

    - 导入RNOHPackagesFactory.ets中的package对象；

    - 合并手动link的package。

    ```ts
    import type { RNPackageContext, RNOHPackage } from '@rnoh/react-native-openharmony';
    import { createRNOHPackages as createRNOHPackagesAutolinking } from "./RNOHPackagesFactory"

    export function getRNOHPackages(ctx: RNPackageContext): RNOHPackage[] {
      return [
        ...createRNOHPackagesAutolinking(ctx),
        // 添加手动link的package
        // ...
      ];
    }
    ```
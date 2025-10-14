# Codegen

如[React Native新架构的Codegen文档](https://github.com/reactwg/react-native-new-architecture/blob/main/docs/codegen.md)所介绍，在创建 ` Turbo Native Module` 和 `Fabric Native Component`  时我们通常使用 `Codegen` 来生成胶水代码。

在**RNOH** 中我们也创建了鸿蒙化的 Codegen 工具，下面是详细的介绍。

## 环境准备

**RNOH** 的 `Codegen` 功能是从 `rnoh-react-native-harmony-0.72.13` 开始支持，因此要使用 `Codegen` 必须要安装 `react-native-harmony` 0.72.13 及之后的版本。

`Codegen` 的使用 demo 可参考下面两个工程：

* [TurboModule的demo工程](../Samples/using_turboModule/README_zh.md)
* [自定义Fabric组件](../Samples/FabricComponentSample/README_zh.md)

## 使用说明

### 使用Codegen工具创建Turbo Native Module和Fabric Native Component

> Codegen自动生成的胶水代码会覆盖本地实现的胶水代码。

同 Android 和 iOS ，使用 Codegen 工具创建 Turbo Native Module 和 Fabric Native Component 大致分为以下步骤：

1. _[可选]_ 新建一个前端三方库;
2. 声明 JavaScript 接口；
3. 配置 Codegen；
4. 执行 Codegen；
5. 编写原生代码以实现 TurboModule 或 Fabric 组件的功能；
6. 在 JavaScript 侧调用

详细的流程可参考下面几篇文档的介绍：

* [自定义TurboModule的实现](./TurboModule.md)
* [自定义组件开发指导](./自定义组件.md)
* [C-API组件混合方案的使用](./C-API组件混合方案的使用.md)

### 配置 Codegen

配置 Codegen 涉及到如下几个部分的内容：

#### 1. codegenConfig

同 Android 和 IOS，Codegen 是基于三方库或者前端工程的 package.json 中的 codegenConfig 配置来生成代码的，不过需要注意的是，在 **RNOH** 中，`codegenConfig` 字段需要嵌套在 `harmony` 中，如：

```json
...
"harmony": {
  "alias": "<模块名称>",
  "codegenConfig": [
    {
      "version": 1,
      "specPaths": [
        "./src/specs/v1"
      ]
    },
    {
      "version": 2,
      "specPaths": [
        "./src/specs/v2"
      ]
    }
  ]
}
...
```

其中：

- **version**：表示版本，需要强调一下，这里的版本并没有新旧之说，它们只是使用场景不同而已。  
  `"version": 1` 适用于创建 TurboModule 或者是 ArkTS 组件的场景；  
  `"version": 2` 则适用于创建 **CAPI** 组件的场景。

- **specPaths**：表示声明文件的路径

#### 2. codegen-harmony 命令配置

`codegen-harmony` 是用来生成代码的命令，一般配置在前端工程 package.json 的 scripts 中，然后通过 `npm run xxx` 来执行，如：

```json
"scripts": {
      ...
  "codegen": "react-native codegen-harmony --cpp-output-path ./entry/src/main/cpp/generated --rnoh-module-path ./entry/oh_modules/@rnoh/react-native-openharmony"
}
```

`codegen-harmony` 后面跟着的是传入该命令的一些参数，下面是参数介绍：

- **--rnoh-module-path \<path\><sup>(deprecated)</sup>**：指定 RNOH OHOS 模块的相对路径（如：`--rnoh-module-path ./entry/oh_modules/@rnoh/react-native-openharmony`)，ArkTS 的类型声明文件会生成在这个目录下；

- **--ets-output-path \<path\>**：指定 ArkTS 类型声明文件的输出目录；

> `--rnoh-module-path` 和 `--ets-output-path` 均表示ets文件的输出目录，现不推荐将ets文件生成在 `@rnoh/react-native-openharmony` 目录下，建议改用 `--ets-output-path` 参数将ets文件生成到自己的项目源码目录中；

- **--cpp-output-path \<path\>**：指定 C++ 文件输出目录的相对路径（默认值为`./harmony/entry/src/main/cpp/generated`），C++ 文件会生成在该目录下；

- **--no-safety-check \[boolean\]**：默认情况下 Codegen 无法在工作目录以外进行文件操作，通过配置 `--no-safety-check` 可跳过该检查，如鸿蒙工程不在前端工程目录下时需要传入该参数。


## codegen生成及结构

执行 Codegen 命令后，Codegen 生成的文件结构如下：

```
  └─entry
     └─src
     |  └─main
     |    └─cpp
     |      └─generated
     |          ├─GeneratedSampleViewArkTSComponentDescriptor.h
     |          ├─GeneratedSampleViewArkTSJSIBinder.h
     |          ├─RNOHGeneratedPackage.h
     |          └─react_native_harmony_sample
     |              ├─RNOH
     |              |   └─generated
     |              |       ├─BaseReactNativeHarmonySamplePackage.h
     |              |       ├─components
     |              |       |   ├─BaseGeneratedSampleViewComponentInstance.h
     |              |       |   └─GeneratedSampleViewJSIBinder.h
     |              |       └─turbo_modules
     |              |           ├─GeneratedSampleTurboModule.cpp
     |              |           └─GeneratedSampleTurboModule.h
     |              └─react
     |                  └─renderer
     |                      └─components
     |                          └─react_native_harmony_sample
     |                              ├─ComponentDescriptors.h
     |                              ├─EventEmitters.cpp
     |                              ├─EventEmitters.h
     |                              ├─Props.cpp
     |                              ├─Props.h
     |                              ├─ShadowNodes.cpp
     |                              ├─ShadowNodes.h
     |                              ├─States.cpp
     |                              └─States.h
     └─oh_modules
         └─@rnoh
             └─react-native-harmony
                 └─generated
                     ├─index.ets
                     ├─ts.ts
                     ├─components
                     │  ├─GeneratedSampleViewArkTS.ts
                     │  └─ts.ts
                     └─turboModules
                         ├─GeneratedSampleTurboModule.ts
                         └─ts.ts
```

> 上面的文件结构仅供参考，实际情况根据开发者的声明文件和 Codegen 配置决定。

## Codegen-lib-harmony

`codegen-harmony` 会遍历 `node_module` 目录下所有三方库的 Codegen 配置，然后生成所有的胶水代码。但有的开发者由于各种原因（比如不同三方库胶水代码存放不同位置等），希望能只为某个三方件胶水代码，此时可以使用 `codegen-lib-harmony` 命令。`codegen-lib-harmony` 是  `react-native-harmony-cli@0.0.27` 提供的新能力，使用之前先检查一下依赖。

跟 `codegen-harmony` 类似，`codegen-lib-harmony` 的使用也是大致分为以下步骤：

1. 新建一个前端三方库;
2. 声明 JavaScript 接口；
3. 配置 Codegen；
4. 执行 Codegen；
5. 编写原生代码以实现 TurboModule 或 Fabric 组件的功能；
6. 在 JavaScript 侧调用
  
### 配置 Codegen

这一步是在三方库中操作，与 `codegen-harmony` 不同，这里不需要在三方库 package.json 中配置 codegenConfig 字段，或者 `harmony::codegenConfig::specPaths` 中不应包含需要单独生成胶水代码的 JavaScript 声明文件。

而这些需要单独生成胶水代码的 JavaScript 声明文件则需要通过参数传递给 `codegen-lib-harmony`, 如：

```json
"scripts": {
  "codegen-lib": "react-native codegen-lib-harmony --no-safety-check --npm-package-name codegen-lib --cpp-output-path ../harmony/codegen-lib/src/main/cpp/generated --ets-output-path ../harmony/codegen-lib/src/main/ets/generated --turbo-modules-spec-paths ./src --arkts-components-spec-paths ./src/SelectBox.ts --cpp-components-spec-paths ./src/CppSampleNativeComponent.ts"
}
...
```

> 上面的命令一般是配置三方库的 package.json 中，配置完后在该三方库的路径下执行 `npm run codegen-lib` 则可生成胶水代码。

其中 `codegen-lib-harmony` 命令支持的参数及说明如下：

- **--npm-package-name \<string\>**：三方库的包名，生成 C++ 代码时，会创建一个跟该包名同名的目录然后再将 C++ 文件放进去；

- **--turbo-modules-spec-paths [path...]**: TurboModule 的声明文件路径，可以是某个目录或者声明文件的相对路径，可以支持传入多个，用空格分隔；

- **--arkts-components-spec-paths [path...]**: ArkTS 组件的声明文件路径，可以是某个目录或者声明文件的相对路径，可以支持传入多个，用空格分隔；

- **--cpp-components-spec-paths [path...]**: **CAPI** 组件的声明文件路径，可以是某个目录或者声明文件的相对路径，可以支持传入多个，用空格分隔；

- **--cpp-output-path \<path\>**：指定 C++ 文件输出目录的相对路径；

- **--ets-output-path \<path\>**：指定 ArkTS 类型声明文件的输出目录；

- **--no-safety-check [boolean]**：是否跳过文件操作安全检查，同 `codegen-harmony`。

以上是 `codegen-lib-harmony` 命令支持所有参数，原生端 TurboModule 和自定义组件的功能实现逻辑跟 `codegen-harmony` 类似，这里不多讲。

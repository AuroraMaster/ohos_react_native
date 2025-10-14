# Codegen

On the [React Native official website](https://github.com/reactwg/react-native-new-architecture/blob/main/docs/codegen.md), `Codegen` is usually used to generate glue code when `Turbo Native Module` and `Fabric Native Component` are created.

A OpenHarmony-based Codegen is also created in RNOH, which is described below.

## Environment Setup

The `Codegen` function of RNOH is supported since `rnoh-react-native-harmony-0.72.13`. Therefore, to use `Codegen`, you must install `react-native-harmony` 0.72.13 or later.

The following two demo projects describe how to use the `Codegen`:

* [Demo Project of TuboModule](../Samples/using_TurboModule/README.md)
* [Custom Fabric Component](../Samples/FabricComponentSample/README.md)

## Usage

### Creating Turbo Native Modules and Fabric Native Components Using Codegen

For Android and iOS, the procedure of using Codegen to create Turbo Native Module and Fabric Native Component is as follows:

1. _(Optional)_ Create a frontend third-party library.
2. Declare JavaScript APIs.
3. Configure the Codegen.
4. Execute the Codegen.
5. Compile native code to implement the functions of **TurboModule** or the **Fabric** component.
6. Call **TurboModule** and the **Fabric** component on the JavaScript side.

For details, see the following documents:

* [Implementation of a Custom TurboModule](./TurboModule.md)
* [Custom Component Development Guide](./custom-components.md)
* [Usage of the C-API Component Hybrid Solution](./usage-of-c-api-component-hybrid-solution.md)

### Codegen Configuration

Codegen configuration involves the following scenarios:

#### 1. codegenConfig

For Android and iOS, Codegen generates code based on **codegenConfig** in the **package.json** file of the third-party library or frontend project. Note that the `codegenConfig` field in RNOH must be nested in `harmony`. For example:

   ```json
    ...
    "harmony": {
      "alias": "<Module name>",
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

where:

- **version**: indicates the version, which only differs from application scenarios. 
  `"version": 1`: applies to the scenario where **TurboModule** or **ArkTS** component is created. 
  `"version": 2`: applies to the scenario where the **CAPI** component is created.

- **specPaths**: path of the declaration file.

#### 2. Configuring codegen-harmony

`codegen-harmony` is a command used to generate code. Generally, it is configured in the scripts of the frontend project **package.json** and executed using `npm run xxx`. For example:

```json
"scripts": {
      ...
  "codegen": "react-native codegen-harmony --cpp-output-path ./entry/src/main/cpp/generated --rnoh-module-path ./entry/oh_modules/@rnoh/react-native-openharmony"
}
```

`codegen-harmony` is followed by some parameters of the command. The parameters are described as follows:

- **--rnoh-module-path \<path\>**: specifies the relative path (for example, `--rnoh-module-path ./entry/oh_modules/@rnoh/react-native-openharmony`) of the RNOH OHOS module. The ArkTS type declaration file is generated in this directory.

- **--cpp-output-path \<path\>**: specifies the relative path of the C++ file output directory. The default value is `./harmony/entry/src/main/cpp/generated`.

- **--no-safety-check \[boolean\]**: By default, Codegen cannot perform file operations outside the working directory. You can skip this check by configuring `--no-safety-check`. If the OpenHarmony project is not in the frontend project directory, this parameter needs to be transferred.


## Codegen File Generation and Structure

After the Codegen command is executed, the structure of the generated file is as follows:

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


> The preceding file structure is for reference only. The actual file structure depends on the declaration file and Codegen configuration.

## Codegen-lib-harmony

`codegen-harmony` traverses the Codegen configurations of all third-party libraries in the `node_module` directory and generates all glue code. If you want to write glue code for a specific third-party component due to various reasons (for example, the glue code is stored in different locations in different third-party libraries), run the `codegen-lib-harmony` command. `codegen-lib-harmony` is a new capability provided by `react-native-harmony-cli@0.0.27`. Check the dependency before using it.

Similar to `codegen-harmony`, `codegen-lib-harmony` is used in the following steps:

1. Create a frontend third-party library.
2. Declare JavaScript APIs.
3. Configure the Codegen.
4. Execute the Codegen.
5. Compile native code to implement the functions of **TurboModule** or the **Fabric** component.
6. Call **TurboModule** and the **Fabric** component on the JavaScript side.
  
### Codegen Configuration

This step is performed in the third-party library. Different from `codegen-harmony`, the **codegenConfig** field is not required in the **package.json** file of the third-party library, and JavaScript declaration files that require separate glue code generation should not be contained in the `harmony::codegenConfig::specPaths` file.

However, these JavaScript declaration files need to be passed to `codegen-lib-harmony` through parameters. For example:

```json
"scripts": {
  "codegen-lib": "react-native codegen-lib-harmony --no-safety-check --npm-package-name codegen-lib --cpp-output-path ../harmony/codegen-lib/src/main/cpp/generated --ets-output-path ../harmony/codegen-lib/src/main/ets/generated --turbo-modules-spec-paths ./src --arkts-components-spec-paths ./src/SelectBox.ts --cpp-components-spec-paths ./src/CppSampleNativeComponent.ts"
}
...
```

> Generally, the preceding command is used to configure the **package.json** file of the third-party library. After the configuration is complete, run the `npm run codegen-lib` command in the path of the third-party library to generate the glue code.

The `codegen-lib-harmony` command supports the following parameters:

- **--npm-package-name \<string\>**: bundle name of the third-party library. When the C++ code is generated, a directory with the same name as the bundle name is created and then the C++ file is stored in the directory.

- **--turbo-modules-spec-paths [path...]**: path of the TurboModule declaration file, which can be a directory or a relative path of the declaration file. Multiple paths can be passed and separated by spaces.

- **--arkts-components-spec-paths [path...]**: declaration file path of the **ArkTS** component, which can be a directory or a relative path of the declaration file. Multiple paths can be passed and separated by spaces.

- **--cpp-components-spec-paths [path...]**: declaration file path of the **CAPI** component. It can be a directory or a relative path of the declaration file. Multiple paths can be passed and separated by spaces.

- **--cpp-output-path \<path\>**: specifies the relative path of the C++ file output directory.

- **--ets-output-path \<path\>**: specifies the output directory of the ArkTS type declaration file.

- **--no-safety-check [boolean]**: indicates whether to skip file safety check. The operation is similar to that of `codegen-harmony`.

`codegen-lib-harmony` supports all the preceding parameters. The implementation logic of the native **TurboModule** and custom components is similar to that of `codegen-harmony`.

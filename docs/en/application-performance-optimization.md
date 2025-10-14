# Application Performance Optimization

Application performance optimization requires a performance version application package of the production environment. You need to configure the environment and use the profiler tool for debugging. This document describes how to obtain the application performance optimization configuration, profiler, and the latest CAPI version of a third-party library.

## 1. Release Environment Configuration on JS

The performance optimization on JS is configured by using the environment command during bundling.

In a common debug environment, you can run the following bundle command:

```cmd
npm run codegen && react-native bundle-harmony --dev --minify=false
```


- **`npm run codegen`**: runs the code generation tool, which is used to generate some necessary files and data structures.
- **`--dev`**: enables the development mode. Compression and optimization are not performed during bundling to facilitate debugging.
- **`--minify=false`**: disables code compression so that the code is more readable but larger.

You can run the following command to optimize the performance for bundling:

``````cmd
npm run codegen && react-native bundle-harmony --dev=false --minify=true
``````


- **`--dev=false`**: disables the development mode and enables the production mode. Optimization and compression are performed during bundling.
- **`--minify=true`**: enables code compression to reduce the bundle size and optimize the performance.

## 2. Native Environment Configuration

When configuring the native environment for performance optimization, you can perform the following operations:

### (1) Setting the Build Mode to Release in the IDE

In DevEco IDE, set the build mode. Click **Product** in the upper right corner, set **Build Mode** to **release**, and click **Apply**. During compilation, bundling, and running, the IDE uses the release mode to build an application so that the performance of the bundled application can be optimized.

![](./figures/application-performance-optimization-release-settings.png)

### (2) Setting LOG_VERBOSITY_LEVEL in CMakeList.txt

By default, you can set `LOG_VERBOSITY_LEVEL` in the CMakeList configuration file to `1`, which means to enable low-level log output. These logs are used to display key information or basic debugging output.

```cmake
set(LOG_VERBOSITY_LEVEL 1)
```

Specifically, `LOG_VERBOSITY_LEVEL=1` has the following functions:

#### a. **Enable basic log output.**
When `LOG_VERBOSITY_LEVEL` is set to `1`, CMake outputs additional log information that is not too detailed. It is usually used to display key information, such as key build steps, major status updates, warnings, and errors. Logs of this level are applicable to common development or debugging. They can capture important events and problems without overwhelming key information due to a large amount of detailed information.

#### b. **Filter out log information that is too detailed.**
Compared with a higher log level (such as `LOG_VERBOSITY_LEVEL=2` or higher), `LOG_VERBOSITY_LEVEL=1` filters out a large amount of low-priority information to avoid outputting debugging information that is too detailed (such as function calls and variable statuses). In this way, unnecessary log information is avoided, ensuring concise output.

#### c. **Applicable scenario**
This log level is suitable for developers who need extra context to understand problems during build or configuration, but do not need very in-depth debugging information. For common debugging and troubleshooting, this setting will suffice to provide appropriate information.

In a specific CMake project, `LOG_VERBOSITY_LEVEL=1` may display the following types of information:
- Key configuration status, such as changes of compilers and build options.
- Loading status of project dependencies.
- Warning and error information.

It does not output detailed debugging information (such as detailed compiler options and values of internal variables). The information can be viewed only when `LOG_VERBOSITY_LEVEL` is set to a larger value.

Therefore, `LOG_VERBOSITY_LEVEL=1` is a setting that balances log detail and readability and is used to obtain appropriate build feedback.

During performance optimization, excessive logs affect application performance. Therefore, you can comment out this setting in the CMakeList file or set its value to **0** to disable log printing.

```cmake
set(LOG_VERBOSITY_LEVEL 0)
```

## 3. Bytecode (HBC)

During performance optimization, you can compile the JavaScript bundle into a Hermes bytecode binary file and convert the JavaScript code into Hermes bytecode, that is, an HBC file. The Hermes binary bundle (by compiling JavaScript code into Hermes bytecode) has the following key performance advantages:

- **Faster launch**: Hermes precompiles JavaScript code into bytecode instead of dynamically parsing it at runtime. Compared with standard JavaScript engines (such as JSC or V8), Hermes can skip the parsing and compilation steps, significantly shortening the application launch time. For mobile devices, the reduced launch time improves user experience, especially when the application is loaded for the first time.
- **Less memory usage**: The bytecode generated by Hermes compilation is smaller and more efficient than that generated by the standard JavaScript engine, and occupies less memory. Hermes optimizes the bytecode representation, removing unnecessary symbols and debugging information and focusing on execution efficiency. Reduced memory usage enables applications to run more smoothly on low-memory devices.
- **More efficient garbage collection mechanism**: Hermes' garbage collector is optimized for mobile devices. It uses the generational garbage collection strategy to distinguish the memory between different generations to manage the lifecycles of objects. This mechanism can significantly reduce the pause time and memory fragments, thereby improving the response speed of applications and effectively utilizing the limited resources of mobile devices.
- **Reduced JavaScript compilation time**: Hermes moves JavaScript code compilation to the build phase and uses the `hermesc` tool to precompile JavaScript code into bytecode. Therefore, you do not need to compile JavaScript code when an application is at runtime. This reduces the CPU load at runtime and improves performance, especially in complex applications or applications with a large amount of JavaScript code.

You can run the following bundle command to convert a JavaScript bundle into an HBC bundle:

``````cmd
npm run codegen && react-native bundle-harmony --dev=false --minify=true --bundle-output /tmp/bundle.harmony.js && hermesc --emit-binary /tmp/bundle.harmony.js -out ./harmony/entry/src/main/resources/rawfile/hermes_bundle.hbc
``````

- **`hermesc`**: uses `hermesc` to compile the generated JavaScript bundle into a Hermes bytecode binary file. The Hermes engine reduces the launch time by pre-compiling the bytecode.
- **`--dev`**: enables the development mode, which may affect the performance optimization of Hermes.

## 4. Profiler

You can use the Profiler in DevEco IDE to analyze application performance. For details about how to use Profiler, see [Using DevEco Profiler](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V5/ide-profiler-introduction-V5).

For details about how to analyze the captured traces, see [Performance Optimization](./performance-optimization.md).



## 5. Latest CAPI Version of a Third-Party Library

During application development, if a third-party library is used, ensure that its latest version is used. This not only facilitates function update, but also improves performance and security. For details about how to obtain the latest CAPI version of a third-party library, refer to the [official documentation of third-party libraries](https://gitee.com/react-native-oh-library/usage-docs/tree/master/zh-cn) and related resources.

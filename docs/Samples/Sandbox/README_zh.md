# README

这个项目包含两个演示，一个演示如何在 OpenHarmony 侧将 raw 文件推送到沙箱并成功加载，另一个演示如何在 React Native 侧加载沙箱中的图片。

# 目录结构

```
├── SandBoxNativeProject
|   └── Index.ets
|       ├── downloadBundle # OpenHarmony 侧推送 raw 文件到沙箱中
|       └── new FileJSBundleProvider # OpenHarmony 侧加载沙箱中的文件
└── SandBoxRnProject
    └── example.tsx  # React Native 侧加载沙箱中的图片

```

# OpenHarmony 侧

1. **将文件推送到沙箱**: 使用 `downloadBundle` 函数的 getRawFileContent 方法将 rwaFile 中的文件推送到沙箱，以便之后的读取操作。
2. **加载文件**: 使用 `new FileJSBundleProvider` 类从沙箱中加载文件并验证其内容。

# React Native 侧

1. **加载沙箱中的图片**: 使用 `example.tsx` 中的代码，从 OpenHarmony 沙箱中加载图片，并在 React Native 应用中显示。

# 运行步骤

1. 在 `SandBoxRnProject` 目录下执行 **npm i @react-native-oh/react-native-harmony@x.x.x**或**yarn add @react-native-oh/react-native-harmony@x.x.x** 安装依赖，执行 **npm run dev** 生成 bundle；
2. 在 `entry` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
3. 检查 `SandBoxNativeProject`、`entry` 目录下是否生成 `oh-modules` 文件夹；
4. 用 DevEco Studio 打开 `SandBoxNativeProject`，执行 **Sync and Refresh Project**；
5. 点击 File > Project Structure > Signing Configs，登录并完成签名；
6. 点击右上角的 **run** 启动项目；
7. 确保鸿蒙和 rn 侧依赖下载无缺，版本配套正常；
8. 打开生成的 app 来确认文件已正确加载。

# 注意事项

- 确保 OpenHarmony 和 React Native 项目配置正确，以便文件可以在两个系统之间无缝传递与加载。
- 沙箱路径的使用和文件访问权限需要根据具体环境进行调整。

---

# 效果预览
通过代码将bundle推送到沙箱：

![沙箱加载运行效果图](./screenshots/Screenshots.jpeg)
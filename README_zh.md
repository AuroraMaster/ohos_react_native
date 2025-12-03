# React Native for OpenHarmony (RNOH)

欢迎阅读React Native for OpenHarmony文档，参与React Native for OpenHarmony开发者文档开源项目，与我们一起完善开发者文档。

## RNOH版本演进规划和分支策略
您可以在[RNOH版本演进规划和分支策略](https://gitcode.com/openharmony-sig/ohos_react_native/wiki/RNOH%E7%89%88%E6%9C%AC%E6%BC%94%E8%BF%9B%E8%A7%84%E5%88%92%E5%92%8C%E5%88%86%E6%94%AF%E7%AD%96%E7%95%A5.md)中了解更多关于我们对React Native的OpenHarmony适配版本的说明。

## 启动

### 前提条件

#### 安装了HarmonyOS NEXT的模拟器或真机

该项目需要HarmonyOS NEXT上进行测试。

#### 华为开发者账号

运行应用程序时，需要用到华为开发者账号进行签名。

#### DevEco Studio

从[官网](https://developer.huawei.com/consumer/cn/deveco-studio/)下载并安装最新版本 DevEco Studio。

### 启动项目

> 如果是编译源码，在拉取工程后需要在工程的根目录执行`git submodule update --init --recursive`命令，拉取三方库依赖。

1. 打开终端（命令行工具）并导航到react-native-harmony-cli目录，然后执行 `npm i && npm pack`。
1. 在终端中导航到react-native-harmony目录，然后执行 `npm i && npm pack`。
1. 在终端中导航到react-native-harmony-sample-package目录，然后执行 `npm i && npm pack`。
1. 导航到tester目录，然后执行 `npm run i` （不是 npm i）。
1. 在tester目录中运行 `npm start` 以启动Metro服务器。
1. 设置`RNOH_C_API_ARCH`环境变量的值为`1`，具体参考[配置CAPI版本环境变量](docs/zh-cn/环境搭建.md#set_capi_path)。
1. 在DevEco Studio中打开 `tester/harmony` 项目并等待后台同步完成。
1. 连接设备。
1. 点击 File > Project Structure > Signing Configs，登录并完成签名。
1. 选择 `entry`（右上角）。
1. 点击【Run 'entry'】按钮（右上角）运行项目。

**说明：** 该tester项目工程主要用于开发自测试各种用例场景，如果在运行tester中的用例时遇到问题，通过提issues来咨询。

## 使用说明

- React Native的使用问题可查阅[React Native 官网](https://reactnative.dev/)或[React Native 中文网](https://reactnative.cn/)。
- RNOH的使用问题可查阅[中文文档](./docs/zh-cn/README.md)或[英文文档](./docs/en/README.md)。

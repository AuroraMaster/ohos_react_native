# 说明
这是一个使用RNSurface的demo工程。

## 目录结构
SampleProject -- RN侧工程  
SampleApp -- 原生工程


## 环境搭建
1. 在 `SampleProject/MainProject` 目录下执行 **npm i @react-native-oh/react-native-harmony@x.x.x**或**yarn add @react-native-oh/react-native-harmony@x.x.x** 安装依赖，执行 **npm run dev** 生成 bundle；
2. 在 `entry` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
3. 检查 `SampleApp`、`entry` 目录下是否生成 `oh-modules` 文件夹；
4. 用 DevEco Studio 打开 `SampleApp`，执行 **Sync and Refresh Project**；
5. 点击 File > Project Structure > Signing Configs，登录并完成签名；
6. 点击右上角的 **run** 启动项目。

## 效果预览
启动后页面效果如下：

![using_RNSurface运行界面](./screenshots/Screenshot.jpeg)

# 说明

这是RNOH Autolinking的demo工程

## 目录结构

```md
AutolinkingSample
├── NativeProject harmony工程
├── react-native-oh RNOH前端及手脚架
├── ReactProject 前端工程
├── screenshots 效果图
├── third-party-library-sample RN三方件示例
└── README.md
```

## 环境搭建

1. 在 `third-party-library-sample` 中运行 **npm pack** 进行打包；
1. 在 `ReactProject` 目录下执行 **npm i @react-native-oh/react-native-harmony@x.x.x**或**yarn add @react-native-oh/react-native-harmony@x.x.x** 安装依赖;
1. 修改 `NativeProject/hvigor/hvigor-config.json5` 中 `@rnoh/hvigor-plugin` 的版本号；
1. 用 DevEco Studio 打开 `NativeProject`，执行 **Sync and Refresh Project**；
1. 点击 File > Project Structure > Signing Configs，登录并完成签名；
1. 在 `ReactProject` 目录下执行 **npm start** 启动Metro;
1. 点击 DevEco Studio 右上角的 **run** 启动项目；

## 效果预览

启动后页面效果如下：

![FabricComponentSample运行界面](./screenshots/Screenshot.jpg)

1. 点击【点击滚到顶】按钮，弹窗会滚到顶部；
1. 弹窗会滚到顶部后向下拉，会根据滚动位置决定最终停留在中间还是回到顶部；
1. 点击【选项1/2/3】，选项右侧会显示选中状态；
1. 点击【确定】按钮，会在VSCode控制台打印对应选项的value，如：`{"target": 14, "value": [1]}`；
1. 点击【现在是单选】按钮，按钮文案会改成【现在是多选】，继续点击【选项1/2/3】，可以让多个选项切换到选中状态；
1. 点击【确定】按钮，会在VSCode控制台打印对应选项的value，如：`{"target": 14, "value": [1,2]}`；
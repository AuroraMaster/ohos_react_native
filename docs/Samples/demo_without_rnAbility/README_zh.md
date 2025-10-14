# 说明
这是一个不继承RNAbilty，由原生页面跳转到RN页面的demo工程。


# 目录结构
RNProject -- RN侧工程  
NativeProject -- 原生工程


# 环境搭建
1. 在 `library` 目录下执行 **ohpm i @rnoh/react-native-openharmony@x.x.x** 安装依赖；
2. 执行完成后，检查是否在 `NativeProject`、`entry`、`library` 目录下生成 `oh-modules` 文件夹；
3. 在 `RNProject/MainProject` 中执行 **npm i @react-native-oh/react-native-harmony@x.x.x**或**yarn add @react-native-oh/react-native-harmony@x.x.x** 安装依赖；
4. 在 `RNProject/MainProject` 中执行 **npm run dev** 命令打包；
5. 用 DevEco Studio 打开 `NativeProject`，执行 **Sync and Refresh Project**；
6. 点击 File > Project Structure > Signing Configs，登录并完成签名；
7. 点击右上角的 **run** 启动项目。
   

# 效果预览
在成功运行demo工程之后，打开应用如下图所示：
![demo_without_rnAbility的应用首页](../../zh-cn//figures/demoWithoutRNAbility-App首页.jpg)

**导航到RN页面**
操作：点击首页中间的打开RN应用按钮，跳转到RN页面。
效果：如下图所示
![demo_without_rnAbility的RN页面](../../zh-cn//figures/demoWithoutRNAbility-RN页面.jpg)


# 注意事项

1. 主动调用`windowSizeChange`方法监听屏幕尺寸变化，调用`this.rnInstancesCoordinator?.onWindowSizeChange`设置正确的屏幕尺寸，否则会出现字体size不正常的问题。
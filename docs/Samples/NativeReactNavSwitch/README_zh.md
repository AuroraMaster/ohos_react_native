# 说明
这是一个原生页面和RN页面相互跳转的demo工程。


# 目录结构
NavigationProject -- RN侧工程  
NavigationApp -- 原生工程


# 环境搭建
1. 在 `NavigationProject/MainProject` 目录下执行 **npm run i**或**yarn i** 安装依赖，执行 **npm run dev** 生成 bundle；
2. 在 `entry` 目录下执行 **ohpm i @rnoh/react-native-openharmony** 安装依赖；
3. 检查 `NavigationApp`、`entry` 目录下是否生成 `oh-modules` 文件夹；
4. 用 DevEco Studio 打开 `NavigationApp`，执行 **Sync and Refresh Project**；
5. 点击 File > Project Structure > Signing Configs，登录并完成签名；
6. 点击右上角的 **run** 启动项目。


# 效果预览
在成功运行demo工程之后，打开应用如下图所示：  
![NativeReactNavSwitch的应用首页](../../zh-cn//figures/NativeReactNavSwitch-首页.png)

**导航到RN页面B**
操作：点击首页的跳转到RN页面B按钮，导航到RN页面B。
效果：如下图所示  
![NativeReactNavSwitch的RN页面B](../../zh-cn//figures/NativeReactNavSwitch-RN页面B.png)

**导航到原生页面C**
操作：点击RN页面B的跳转到原生页面C按钮，导航到原生页面C。
效果：如下图所示  
![NativeReactNavSwitch的原生页面C](../../zh-cn//figures/NativeReactNavSwitch-原生页面C.png)

**回到RN页面B**
操作：点击原生页面C的回到上一个页面按钮或侧滑返回，回到RN页面B。
效果：如下图所示  
![NativeReactNavSwitch的RN页面B](../../zh-cn//figures/NativeReactNavSwitch-RN页面B.png)

**回到原生页面A**
操作：点击RN页面B的回到上一个页面按钮或侧滑返回，回到原生页面A。
效果：如下图所示  
![NativeReactNavSwitch的原生页面A](../../zh-cn//figures/NativeReactNavSwitch-首页.png)

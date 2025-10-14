# React Native OpenHarmony

## 简介
本库是react-native在OpenHarmony上进行适配的原生侧har包。

## 下载安装
```text
ohpm install @rnoh/react-native-openharmony
```
环境配置等更多内容，请参考[环境搭建](https://gitee.com/openharmony-sig/ohos_react_native/blob/0.72.5-ohos-5.0-release/docs/zh-cn/%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA.md)

## 使用说明
执行上一步命令安装完成之后，点击顶部菜单栏的File > Sync and Refresh Project来执行ohpm install，执行完成后会在entry目录下生成oh_modules文件夹。 由于har包比较大，这一步耗时会比较长，务必保证ohpm install和IDE自发的SyncData全都完成，否则会导致编译报错。
更多详细信息参考配套文档[创建鸿蒙工程](https://gitee.com/openharmony-sig/ohos_react_native/blob/0.72.5-ohos-5.0-release/docs/zh-cn/%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA.md#%E5%88%9B%E5%BB%BA%E9%B8%BF%E8%92%99%E5%B7%A5%E7%A8%8B)

## 目录结构
```text
|---- react_native_openharmony
|     |---- src  # react_native_openharmony库文件夹
|           |---- main  # 模块代码
|                |---- cpp   # c++侧代码
|                |---- ets   # ets侧代码
                 |---- resources   # 资源文件夹
|            |---- *.json5      # 配置文件
|     |---- build-profile.json5     # 安装使用方法
|     |---- BuildProfile.ets        # 配置参数
|     |---- CHANGELOG.md            # 修改记录
|     |---- hvigorfile.ts           # hvigor配置
|     |---- LICENSE                 # 开源协议license
|     |---- oh-package.json5        # 配置文件
|     |---- README.md               # 安装使用方法

```

## 贡献代码

使用过程中发现任何问题都可以提交[Issue](https://gitee.com/openharmony-sig/ohos_react_native/issues)，当然，也非常欢迎提交[PR](https://gitee.com/openharmony-sig/ohos_react_native/pulls) 。

## 开源协议

本项目遵循 [MIT License]

# React Native 鸿蒙化版本信息
当前React Native鸿蒙版本基于社区RN 0.72.5进行适配，发布版本信息如下：
| 名称                          | 版本号                            |
| ----------------------------- | -------------------------------|
| react-native-harmony.tgz        | 0.72.46 |
| react-native-harmony-cli.tgz    | 0.0.27 |
| react_native_openharmony-5.0.0.812.har                          | 0.72.46 |
| react_native_openharmony_release-5.0.0.812.har                  | 5.0.0.812 |

配套IDE、SDK版本和手机ROM：
| 名称                          | 版本号                            |
| ----------------------------- | -------------------------------|
| DevEco Studio     | DevEco Studio 5.0.5.309 |
| HarmonyOS SDK     | HarmonyOS SDK 5.0.1.115(SP2) |
| 手机ROM           | ALN-AL00 205.0.0.115(SP6C00E115R4P22) <br> ALN-AL80 205.0.0.115(SP6C00E115R4P22) <br> BRA-AL00 205.0.0.115(SP6C00E115R4P22) |



## 特性迭代

### 支持Metro局域网内远程调试
RNOH 5.0.0.803及以上版本，支持Metro局域网内远程调试。
该功能需要允许应用读取加速度传感器的数据，可以在module.json5内配置该权限。
```json5
"requestPermissions": [
    {
        "name": "ohos.permission.ACCELEROMETER"
    },
]
```
#### 已知缺陷
release模式下可以通过摇一摇的方式打开DevMenu的弹窗进行远程调试。

想详细了解如何开启远程调试，可以参考[调试调测](../调试调测.md)。

### ContentSlot接入
根据[HarmonyOS关于XComponentType](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/ts-appendix-enums-V5#xcomponenttype10%E6%9E%9A%E4%B8%BE%E8%AF%B4%E6%98%8E)的介绍，先前在RNOH中使用的用于Native UI节点的占位容器XComponent的NODE类型不再演进，推荐使用[ContentSlot](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V5/arkts-rendering-control-contentslot-V5)占位组件管理Native API创建的组件，ContentSlot在内存和性能方面都优于NODE类型的XComponent。因此，RNOH将使用ContentSlot作为占位组件。
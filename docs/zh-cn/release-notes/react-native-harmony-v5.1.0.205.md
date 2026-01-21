# React Native 鸿蒙化版本信息

当前React Native鸿蒙版本基于社区RN 0.72.5进行适配，发布版本信息如下：

| 名称                          | 版本号                            | 是否发布npm仓 |
| ----------------------------- | -------------------------------|------------- |
| react-native-harmony.tgz        | 0.72.59 |   已发布     |
| react-native-harmony-cli.tgz    | 0.0.28 |   已发布     |
| react_native_openharmony-5.1.0.205.har                          | 0.72.59 |   已发布     |
| react_native_openharmony_release-5.1.0.205.har                  | 5.1.0.205 |   已发布     |

配套IDE、SDK版本和手机ROM：

| 名称                          | 版本号                            |
| ----------------------------- | -------------------------------|
| DevEco Studio     | DevEco Studio 5.1.0.249 |
| HarmonyOS SDK     | HarmonyOS SDK 5.1.0.52(SP3) |
| 手机ROM           | ALN-AL00 205.1.0.52(SP31DEVC00E52R4P1) <br> ALN-AL80 205.1.0.52(SP31DEVC00E52R4P1) <br> BRA-AL00 205.1.0.52(SP31DEVC00E52R4P1) |

## 新增特性

### 支持Worker线程执行自定义任务

RNOH 5.1.0.205及以上版本，支持Worker线程执行自定义任务。在 `EtsUITurboModuleContext` 和 `RNComponentContext` 中添加 `runOnWorkerThread` 方法，允许 `EtsUITurboModules` 和 `ArkTS` 组件调度WorkerTurboModule线程上的任务

具体详细请参考[API接口说明.md#runonworkerthreadtparams-tresult-trunnablerunnable-params](../API接口说明.md#runonworkerthreadtparams-tresult-trunnablerunnable-params)。

### 新增textContentType类型

RNOH 5.1.0.205及以上版本，增加TextInput/TextArea的textContentType的新增类型的支持，对应的枚举类值传入到ArkUI开放的CAPI接口：`NODE_TEXT_INPUT_CONTENT_TYPE`。

textContentType属性增加支持类型：

| enumeration type   | 输入场景      |
| ------------------ | ------------ |
| passportNumber | 护照号 |
| validity | 护照有效期 |
| issueAt | 护照签发地 |
| organization | 发票抬头名称 |
| taxId | 税号 |
| addressCityAndState | 地址城市与省份 |
| flightNumber | 航班号 |
| licenseNumber | 驾驶证号 |
| licenseFileNumber | 驾驶证档案编号 |
| licensePlate | 车牌号 |
| engineNumber | 行驶证发动机号 |
| licenseChassisNumber | 许可证编号 |

具体使用方法可以参考对应的[ArkUI contentType](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/ts-basic-components-textinput-V5#contenttype12%E6%9E%9A%E4%B8%BE%E8%AF%B4%E6%98%8E)

### 支持使用毕昇编译器构建应用

使用 DevEco Studio 5.0.3.402 及以上版本可以从 clang 编译器切换到毕昇编译器构建应用以提升应用运行性能。具体使用方法请参考[性能调优文档](../性能调优.md#3-使用毕昇进行构建)

### View组件支持focusable属性

RNOH 5.1.0.205及以上版本，增加对于View组件的focusable属性的支持，在手机外接键盘使用Tab进行走焦时，若设置focusable属性为false的时候，对应的View组件将无法获取焦点。具体使用请参看[RN官网](https://reactnative.dev/docs/0.72/view#focusable-android)对于focusable属性的说明：focusable属性。

### 支持JavaScript多虚拟机切换

RNOH 5.1.0.205及以上版本，支持JavaScript多虚拟机（Hermes、JSVM）切换。

具体详细请参考[常见开发场景.md#由Hermes 引擎切换成 JSVM 引擎](../常见开发场景.md#由hermes-引擎切换成-jsvm-引擎)。
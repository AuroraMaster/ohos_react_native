# React Native 鸿蒙化版本信息
当前React Native鸿蒙版本基于社区RN 0.72.5进行适配，发布版本信息如下：
| 名称                          | 版本号                            |
| ----------------------------- | -------------------------------|
| react-native-harmony        | 0.72.38 |
| react-native-harmony-cli    | 0.0.27 |
| rnoh                          | 5.0.0.715 |
| rnoh_release                  | 5.0.0.715 |

配套IDE、SDK版本和手机ROM：
| 名称                          | 版本号                            |
| ----------------------------- | -------------------------------|
| DevEco Studio     | DevEco Studio 5.0.3.906 |
| HarmonyOS SDK     | HarmonyOS SDK 5.0.0.71(SP1) |
| 手机ROM           | ALN-AL00 205.0.0.102(SP2C00E73R4P17) <br> ALN-AL80 205.0.0.102(SP2C00E73R4P17) <br> BRA-AL00 205.0.0.102(SP2C00E73R4P17) |

## 新增特性

### 自适应RNSurface

RNOH 5.0.0.715及以上版本，新增自适应RNSurface特性。
自适应RNSurface可以由内容决定其高度（宽度仍会尽可能地填充可用区域）。

### 字体注册与创建RNInstance解绑

RNOH 5.0.0.715及以上版本，字体注册与创建RNInstance解绑。
RNInstance新增字体注册接口registerFont，可以随时注册新的字体。

### 字体注册支持读取沙箱文件

RNOH 5.0.0.715及以上版本，两种字体注册方式fontResourceByFontFamily和registerFont均支持从沙箱路径读取字体文件。

### 多实例字体集互相独立

RNOH 5.0.0.715及以上版本，不同RNInstance不再共享字体集。

### TurboModule运行在子线程

RNOH 5.0.0.715及以上版本，新增TurboModule运行在子线程特性。
想详细了解TurboModule运行在子线程这个特性，可以参考[性能调优](../性能调优.md)。
如何设置TurboModule运行在子线程，可以参考[TurboModule](../TurboModule.md)。

### bundle内异常捕获

RNOH 5.0.0.715及以上版本，新增通过接口捕获bundle内运行异常与错误相关接口。

### RNSurface支持RootTag

RNOH 5.0.0.715及以上版本，RNSurface可通过接口获取对应RNInstance和RootTag。

### Codegen

RNOH 5.0.0.715及以上版本，Codegen代码能生成在配置文件的lib中。

## 特性迭代更新

### RNOH 5.0.0.715及以上版本，RNOHCorePackage导入方式需要开发者修改适配
如果需要继续导入RNOHCorePackage类，需要通过以下方式
```
import {RNOHCorePackage} from '@rnoh/react-native-openharmony/src/ets/RNOHCorePackage/Package'
```


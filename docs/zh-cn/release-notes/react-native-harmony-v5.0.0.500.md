# React Native 鸿蒙化版本信息
当前React Native鸿蒙版本基于社区RN 0.72.5进行适配，发布版本信息如下：
| 名称                          | 版本号                            |
| ----------------------------- | -------------------------------|
| react-native-harmony          | 0.72.29                     |
| react-native-harmony-cli                          | 0.0.26                          |
| rnoh                | 5.0.0.500                |

配套IDE、SDK版本和手机ROM：
| 名称                          | 版本号                            |
| ----------------------------- | -------------------------------|
| DevEco Studio          | DevEco Studio 5.0.3.706                     |
| HarmonyOS SDK          | HarmonyOS SDK 5.0.0.61(SP1)                         |
| 手机ROM                | ALN-AL00 205.0.0.61(SP1C00E63R4P9) <br> ALN-AL80 205.0.0.61(SP1C00E63R4P9)  <br> BRA-AL00 205.0.0.61(SP1C00E63R4P9)|

DevEco Studio 更新到5.0.3.700及以后版本，运行过程中可能会存在找不到libhermes.so的现象，请参考[FAQ：找不到libhermes.so](../FAQ.md#找不到libhermesso)解决。

## 需要开发者修改适配

### 自定义字体适配

升级到RNOH 5.0.0.500及以上版本，自定义字体的设置接口需要开发者适配：

#### 变更迭代

升级到RNOH 5.0.0.500及以上版本，编译配置了自定义字体的历史工程时，编译不通过。

#### 适配指导

原字体目录为`src\main\resources\rawfile\assets\assets\fonts\`。
```typescript
import font from '@ohos.font';
...
RNApp({
    rnInstanceConfig: {
        ...
        fontOptions: [
            {
                familyName: 'Pacifico-Regular',
                familySrc: '/assets/fonts/Pacifico-Regular.ttf'
            },
            {
                familyName: 'StintUltraCondensed-Regular',
                familySrc: '/assets/fonts/StintUltraCondensed-Regular.ttf'
            }
        ],
    },
    ...
})
```

升级到RNOH 5.0.0.500及以上版本，接口名从`fontOptions`改为`fontResourceByFontFamily`，传递值从数组改为以`familyName`为键，`Resource`为值的map。废弃原字体目录位置。
以建议的新字体目录`src\main\resources\rawfile\fonts\`为例：
```typescript
...
RNApp({
    rnInstanceConfig: {
        ...
        fontResourceByFontFamily: {
            'Pacifico-Regular': $rawfile("fonts/Pacifico-Regular.ttf"),
            "StintUltraCondensed-Regular": $rawfile('fonts/StintUltraCondensed-Regular.ttf'),
        },
    },
    ...
})
```

## 规格变更

### 主题字体与fontFamily属性的优先级变更

升级到RNOH 5.0.0.500及以上版本，主题字体与`fontFamily`属性的优先级发生变更。

#### 变更影响

当Text组件设置了与主题字体不同的`fontFamily`属性时，生效的字体发生变更。
之前的版本中，fontFamily属性优先生效。当没有设置fontFamily属性或设为undefined时，使用主题字体。
升级到RNOH 5.0.0.500及以上版本，主题字体优先生效，覆盖fontFamily属性。

### halfLeading居中特性改变

升级到RNOH 5.0.0.500及以上版本，halfLeading居中特性发生变更：

#### 变更影响

当未在metadata中配置half_leading或者配置为false时，设置了`lineHeight`属性的Text组件文字上下居中位置发生变更：
之前的版本中，`lineHeight`属性不会影响halfLeading居中特性。
升级到RNOH 5.0.0.500及以上版本，Text组件在设置lineHeight属性时，halfLeading居中特性会强制视为开启。

更多变更信息请参考[ChangeLog](https://gitee.com/openharmony-sig/ohos_react_native/blob/0.72.5-ohos-5.0-release/CHANGELOG.md)。
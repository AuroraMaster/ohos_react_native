# 规格类FAQ

### React Native 升级到 0.72.5 版本

请参考[RN升级需要开发者适配整理](../RN升级需要开发者适配整理.md)，在此文档中，梳理了 React Native 0.59 版本到 0.72.5 版本的主要修改点，您可以根据此文档修改您的代码，并升级 React Native 的版本。
### 开启Fabric以后重要变更

#### UIManager.measure 的实现不正确，获取到的布局数据与 iOS 不一致？

- **A**：`UIManager.measure`在Fabric开启时是不支持的，请使用`onLayout`或者`ref.current.measure`代替。

### 主题字体未生效/fontFamily未生效

- 主题字体优先于fontFamily生效。
- 主题字体的变更需要重启应用后才能生效。

### RN Date.parse()时间转换问题
RN本身并不直接规定日期字符串的格式，而是依赖于JavaScript的Date对象。JS支持以下三种日期格式：
- Date.parse("2024-11-20 14:00:00");//ISO 8601 格式
- Date.parse('Mon, 20 Mar 2023 14:00:00 GMT');//RFC 2822 / IETF 标准

`2024/10/24`是非标准的日期格式，可以考虑依赖[三方库Moment](https://gitee.com/react-native-oh-library/usage-docs/blob/master/zh-cn/js/moment.md)进行处理。比如：
- const date = moment('2024/10/24', 'YYYY/MM/DD').format('YYYY-MM-DD');

### RN setNativeProps只能生效一次问题
在[React Native启用Fabric渲染器](https://reactnative.cn/docs/fabric-renderer)之后，在使用`setNativeProps`时，只会在应用打开之后生效一次，剩余时间`setNativeProps`无法使用。该问题为[RN框架社区已知问题](https://github.com/facebook/react-native/issues/34391)，且[`setNativeProps`在0.72.5版本已废弃](https://github.com/Expensify/App/issues/26989)。可使用`setState`来进行状态设置。例如，在输入框内进行内容清空时，可以用`setState`来实现：

```javascript
import React, { useState } from 'react';
import {
  View,
  TextInput,
  Button,
  StyleSheet,
} from 'react-native';

const App = () => {
  const [text, setText] = useState(''); // 保存 TextInput 的值

  const clearText = () => {
    setText(''); // 清空输入框
  };

  return (
    <View style={styles.container}>
      <TextInput
        style={styles.input}
        value={text}
        onChangeText={setText} // 更新状态
        placeholder="输入一些内容"
      />
      <Button title="清空" onPress={clearText} />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    padding: 16,
  },
  input: {
    width: '100%',
    height: 40,
    borderColor: '#ccc',
    borderWidth: 1,
    borderRadius: 4,
    marginBottom: 16,
    paddingHorizontal: 8,
  },
});

export default App;
```

### zIndex 问题说明

1. **需要设置非 static 的 position 才能使 zIndex 生效**  
   在启用 Fabric 渲染器的情况下，`zIndex` 必须与 `position` 一起使用（如 `relative` 或 `absolute`），否则设置了 `zIndex` 也不会生效。这一点与 Web 行为一致，是 Fabric 渲染机制的规范化体现。

2. **元素的层级排列顺序发生了变化，zIndex 越大越靠上**  
   与旧架构（非 Fabric）不同，Fabric 启用后，`zIndex` 的作用更加明确且优先级更高。元素的层级不再依赖组件声明顺序，而是严格按照 `zIndex` 数值来决定显示的上下关系，`zIndex` 数值越大，元素越靠上。

### ScrollView组件的 velocity 字段说明

含义：velocity 代表组件的滚动速度

说明：计算值与 android 对比有一定的差异，harmony 平台采用了最小二乘法拟合出瞬时速度来计算，比 android 更加线性。

### RN鸿蒙暂不支持toLocaleLowerCase接口

- 如果不要求支持国际化，可使用toLowerCase转换。
- 如果要求支持国际化，需要对部分国际化语言字符进行自定义特殊转换。

### RN适老化如何限制字体缩放倍数问题
- API13之前，不支持该功能。
- 应用配置fontSizeMaxScale字段，限制字体缩放倍数，默认工程无配置。
- 配置后，调整系统字体大小或者显示大小，RN页面组件仅放大到当前配置的倍数，超过该倍数不再放大。

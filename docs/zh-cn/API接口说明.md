# API接口说明

## RNAbility

### 基本介绍

`RNAbility` 继承 ArkUI 的[UIAbility](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5)，封装了启动 **RN** 的必要操作，如不同生命周期状态的处理、订阅系统环境变量的变化等，应用程序开发人员应该扩展这个类，使用方法可参考如下代码：

```typescript
import {RNAbility} from '@rnoh/react-native-openharmony';

export default class EntryAbility extends RNAbility {
  getPagePath() {
    return 'pages/Index';
  }
}

```

### 方法

#### createLogger

```typescript
protected createLogger(): RNOHLogger
```

#### onPrepareRNInstance

```typescript
protected async onPrepareRNInstance(rnohCoreContext: RNOHCoreContext): void
```

如果要在 Ability 中创建 `RNInstance` 和加载包，可重写此方法。`RNAbility` 在加载内容之前等待此方法完成。这种方法减少了在隐藏启动屏幕后可能出现的闪烁，并且 **RN** 应用程序已准备就绪。

##### 参数

| 参数名          | 类型                                | 说明                |
| --------------- | ----------------------------------- | ------------------ |
| rnohCoreContext | [RNOHCoreContext](#rnohcorecontext) |                    |

#### onWindowSetup

```typescript
protected async onWindowSetup(window: window.Window): void
```

自定义屏幕方向和窗口大小。

##### 参数

| 参数名 | 类型                                                         | 说明                     |
| ------ | ------------------------------------------------------------ |  ------------------------ |
| window | [window.Window](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5) | 当前窗口实例当前窗口实例。 |

#### onWindowStageSetup

```typescript
protected async onWindowStageSetup(windowStage: window.WindowStage): void
```

为当前窗口加载具体页面内容，如果要自定义加载行为可重写该方法。

##### 参数

| 参数名      | 类型                                                         | 说明                     |
| ----------- | ------------------------------------------------------------ | ------------------------ |
| windowStage | [window.WindowStage](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5#loadcontentdeprecated ) | 当前窗口实例当前窗口实例。 |

#### getPagePath

```typescript
protected abstract getPagePath(): string
```

获取入口页面路径，该路径是 `main_pages.json` 中定义的路径。扩展类必须实现该方法。

#### defaultBackPressHandler

```typescript
protected defaultBackPressHandler(): void
```

当 React 应用程序不处理设备返回事件时调用该方法。

#### onCreate

```typescript
onCreate(want: Want): void
```

Create 状态为在应用加载过程中，`UIAbility` 实例创建完成时触发，系统会调用[onCreate()](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityoncreate)回调。可以在该回调中进行页面初始化操作，例如变量定义资源加载等，用于后续的 UI 展示。

此处实现为创建 `RNInstancesCoordinator`、`RNOHCoreContext` 实例，并通过调用[`AppStorage.setOrCreate()`](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/ts-state-management-V5#setorcreate10)将 `RNOHCoreContext` 存储到应用全局。

##### 参数

| 参数名 | 类型                                                         | 说明                       |
| ------ | ------------------------------------------------------------ | -------------------------- |
| want   | [Want](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V5/want-overview-V5) | 用于在应用组件之间传递信息。 |


#### onCreateDefaultHttpClient

```typescript
protected onCreateDefaultHttpClient(): undefined | HttpClient
```

返回 `RNInstances` 共享的 HttpClient。

#### onDestroy

```typescript
onDestroy(): void
```

Destroy 状态在 UIAbility 实例销毁时触发。可以在[onDestroy()](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityondestroy)回调中进行系统资源的释放、数据的保存等操作。

这里最终会调用所有已注册 `RNInstance`的 `onDestroy` 方法，统一销毁。

#### onWindowStageCreate

```typescript
onWindowStageCreate(windowStage: window.WindowStage): void
```

UIAbility 实例创建完成之后，在进入 Foreground 之前，系统会创建一个 WindowStage。WindowStage 创建完成后会进入[`onWindowStageCreate()`](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonwindowstagecreate)回调，可以在该回调中设置 UI 加载、设置 WindowStage 的事件订阅。

##### 参数

| 参数名      | 类型                                                         | 说明                     |
| ----------- | ------------------------------------------------------------ | ------------------------ |
| windowStage | [window.WindowStage](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5#loadcontent9 ) | 当前窗口实例当前窗口实例。 |

#### onMemoryLevel

```typescript
onMemoryLevel(level: AbilityConstant.MemoryLevel): void
```

[onMemoryLevel](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-abilitystage-V5#abilitystageonmemorylevel)是系统底层`AbilityStage`的能力，当系统调整内存时触发的事件。

##### 参数

| 参数名 | 类型                                                         | 说明                                         |
| ------ | ------------------------------------------------------------ | -------------------------------------------- |
| level  | [AbilityConstant.MemoryLevel](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-abilityconstant-V5#abilityconstantmemorylevel) | 回调返回内存微调级别，显示当前内存使用状态。 |

#### onConfigurationUpdate

```typescript
onConfigurationUpdate(config: AbilityConfiguration.Configuration): void
```

[onConfigurationUpdate](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-abilitystage-V5#abilitystageonconfigurationupdate)是系统底层`AbilityStage`的能力，环境变化通知接口，发生全局配置发生改变时回调。

##### 参数

| 参数名 | 类型                                                         | 说明                                                         |
| ------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| config  | [AbilityConfiguration.Configuration](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-configuration-V5) | 发生全局配置变更时触发回调，当前全局配置包括系统语言、深浅色模式。 |

#### onForeground

```typescript
onForeground(): void
```

[`onForeground`](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonforeground)在 `UIAbility` 实例切换至前台时触发。

此处会将 `RNInstancesCoordinator` 中的 `appState` 变量设置为 "FOREGROUND"。

#### onBackground

```typescript
onBackground(): void
```

[`onBackground`](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonbackground)在 `UIAbility` 实例切换至后台时触发。

此处会将 `RNInstancesCoordinator` 中的 `appState` 变量设置为 "BACKGROUND"。

`RNAbility` 大部分代码由 **RNOH** 实现，以提高稳定性。如果需要更自由的实现方式，可以使用 `RNInstancesCoordinator`，本节主要介绍了 `RNAbility` 的接口类型。

## RNApp

### 基本介绍

`RNApp` 是用于启动和管理 `RNInstance` 和 `RNSurface` 的模块，封装了创建与启动单个 `RNInstance` 和单个 `RNSuface` 的行为，同时您也不需要考虑在什么时间加载 JS Bundle，如果您的应用只需要单 `RNInstance`，同时只需要单 `RNSurface`，您可以通过 `RNApp` 来创建您的 **RN** 应用，从而免去一系列繁琐的操作，达到即开即用的效果。对于具有多个 Surface 的应用程序，可以考虑使用 `RNSurface`，本节主要介绍了 `RNApp` 的接口类型以及使用方式。

### 关键参数

| 参数名                          | 类型                            | 必填 | 说明                                                         |
| ------------------------------- | ------------------------------- | ---- | ------------------------------------------------------------ |
| rnInstanceConfig                | RNInstanceConfig                | 是   | 用于创建`RNInstance`的`RNInstance`或`RNInstanceOptions`。如果提供了[RNInstanceOptions](#rninstanceoptions)，则此组件负责创建和管理`RNInstance`。 |
| appKey                          | string                          | 是   | 开发者在JS侧使用`AppRegistry.registerComponent`注册的名称。   |
| jsBundleProvider                | JSBundleProvider                | 否   | 在`RNInstance`中运行JS Bundle。Bundle被加载后创建Surface(根容器)。 |
| initialProps                    | Record<string, string>          | 否   | React Native应用的初始化参数。                           |
| wrappedCustomRNComponentBuilder | WrappedCustomRNComponentBuilder | 是   | 构建自定义Fabric组件构造器。如果这一参数不设置或设为undefined会导致RNApp初始化失败，显示白屏。 |
| onSetUp                         | void                            | 否   | 在`RNInstance`创建完成且Surface可见前，调用此回调函数。        |

### 方法

#### onSetUp

```javascript
onSetUp: (rnInstance: RNInstance) => Promise<void> | void = () => Promise.resolve()
```

在 Surface 可见且 `RNInstance` 可用之前，调用 `RNInstance` 中想要执行的方法。

##### 参数

| 参数名     | 类型       | 必填 | 说明             |
| ---------- | ---------- | ---- | ---------------- |
| rnInstance | RNInstance | 是   | `RNInstance`对象。 |

### 具体使用说明

```javascript
RNApp({
  // 用于指定React Native 实例的配置
  rnInstanceConfig: {
    // 外部功能包管理（向框架提供额外的功能特性）
    createRNPackages,
    // 是否启用C++原生文本测算接口
    enableNDKTextMeasuring: true,
    // 是否启动后台线程，建议关闭后台线程
    enableBackgroundExecutor: false,
    // 是否使用CAPI架构，默认为true
    enableCAPIArchitecture: true,
    // ArkTS自定义组件的名称
    arkTsComponentNames,
  },
  // 传递初始属性给 React Native 应用
  initialProps: { "foo": "bar" } as Record<string, string>,
  appKey: "app_name",
  // 构建自定义组件
  wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
  // 在surface可见且rnInstance可用之前，调用`RNInstance`中想要执行的方法
  onSetUp: (rnInstance) => {
    rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP")
  },
  // 用于指定 JavaScript bundle 文件的提供方式
  jsBundleProvider: new TraceJSBundleProviderDecorator(
    new AnyJSBundleProvider([
    new MetroJSBundleProvider(),
    /**
     * 注意：要从`/data/app/el2/100/base/com.rnoh.tester/files/bundle.harmony.js`加载bundle包。路径不匹配是由于OpenHarmony上的应用程序沙盒
     */
    new FileJSBundleProvider('/data/storage/el2/base/files/bundle.harmony.js'),
    new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'hermes_bundle.hbc'),
    new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'bundle.harmony.js')
  ]),
  this.rnohCoreContext.logger),
})
```



## RNSurface

### 基本介绍

`RNSurface` 是 React Native 的根容器，它是一个 Native View，用于将 React Native 组件渲染到 Native UI 中。`RNSurface` 主要负责将组件树渲染到屏幕上。在基于 ArkUI C-API 的适配方案中，rootView 是一个 `NodeContent`，用于占位，并挂载 C-API 的组件。本节主要介绍了 `RNSurface` 的接口类型以及使用方式。

其中，Surface 的创建需要调用 `RNInstance` 的[createSurface](#createsurface)，并且会在 `RNInstance` 触发 `onDestory` 的时候销毁。Surface 会在 `RNSurface` 的 `onAreaChange` 中启动（执行 start ），需要注意的是，startSurface 需要在 loadJSbundle 完成后执行，否则会导致页面白屏。您可以通过 `RNInstance` 的[getBundleExecutionStatus](#getbundleexecutionstatus)获取 bundle 加载状态，确保时序的正确性。

### 关键参数

| 参数名        | 类型                              | 必填 | 说明            |
| ------------- | --------------------------------- | ---- | --------------- |
| ctx           | [RNOHContext \| RNComponentContext](#rncomponentcontext) | 是   | **RN** 组件上下文。    |
| surfaceConfig | [SurfaceConfig](#surfaceconfig)                     | 是   | surface配置参数。 |
| constraints | [Constraints \| undefined](#Constraints)                     | 否   | surface布局约束。 |

### 具体使用说明

```typescript
RNSurface({
  ctx: this.ctx, //RNApp中创建的组件上下文
  surfaceConfig: {
    initialProps: this.initialProps ?? {}, //从RNApp传递来的初始属性
    appKey: this.appKey, //从RNApp传递来的应用名称
  } as SurfaceConfig2,
})
```

`RNSurface` 默认情况下会尽可能地填充可用区域，但也可以通过一些方法使其成为自适应容器，由内容决定其高度（宽度仍会尽可能地填充可用区域）。
要使用自适应的 `RNSurface`，移除对应 JS Bundle 中根容器 AppContainer 的样式中的 flex: 1 属性即可。

#### 方法一

在打包需要使用自适应 `RNSurface` 特性的 JS Bundle 时，本地修改 `node_modules\react-native\Libraries\ReactNative\AppContainer.js` 中 AppContainer 的样式。

```diff
const styles = StyleSheet.create({
  appContainer: {
-    flex: 1,
  },
});
```

#### 方法二

直接修改 JS Bundle 中 AppContainer 的样式。

```diff
  AppContainer.getDerivedStateFromError = undefined;
  var styles = _StyleSheet.default.create({
    appContainer: {
-      flex: 1
    }
  });
  module.exports = AppContainer;
```

### SurfaceConfig

`SurfaceConfig` 拥有两个子类： `SurfaceConfig1`、 `SurfaceConfig2`，开发者在使用的时候可以根据需要分别选择不同的 config：

#### SurfaceConfig1

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| surfaceHandle | SurfaceHandle | 是 | 表示 `RNSurface` 的句柄，可由[createSurface](#createsurface)创建。 |

#### SurfaceConfig2

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| appKey | string | 是 | 开发者在 JS 侧使用 `AppRegistry.registerComponent` 注册的名称。 |
| initialProps | Record<string, any> | 是 | React Native 应用的初始化原生参数。 |

### updateViewRootTag

```javascript
 public updateViewRootTag: ((rnInstance:RNInstance, rootViewTag: number) => void) | undefined;
```
返回该 `RNSurface` 对应的 `RNInstance` 以及 rootViewTag。

### Constraints

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| minWidth | number | 否 | 表示 `RNSurface` 的布局最小宽度。 |
| minHeight | number | 否 | 表示 `RNSurface` 的布局最小高度。 |
| maxWidth | number | 否 | 表示 `RNSurface` 的布局最大宽度。 |
| maxHeight | number | 否 | 表示 `RNSurface` 的布局最大高度。 |

## RNInstance

### 基本介绍

`RNInstance` 是 React Native 的实例，在使用 **RN** 框架的时候，一定要创建和启动一个或多个`RNInstance`。`RNInstance` 负责管理 React Native 的生命周期，维护 React Native 组件树，本节主要介绍了 `RNInstance` 的接口类型以及使用方式。

### RNInstance创建、销毁流程

#### 创建

我们一般使用 `RNOHCoreContext` 中的 `createAndRegisterRNInstance` 方法来创建和注册 `RNInstance`，下面是详细步骤：

1. 应用启动执行 `RNAbility` 中的 `onCreate` 方法，创建了 `RNInstancesCoordinator` 对象实例。该类为 `RNInstance` 的协调者，引导 React Native 并将设备事件转发给 `RNInstance`，且这个类是为 `UIAbility` 的使用而创建的。

    ```javascript
    this.rnInstancesCoordinator = RNInstancesCoordinator.create({
      fontSizeScale: this.context.config.fontSizeScale,
      logger: this.createLogger(),
      uiAbilityContext: this.context,
      defaultBackPressHandler: () => {
        this.defaultBackPressHandler()
      },
    }, {
      launchURI: want.uri, onGetPackagerClientConfig: (buildMode) => buildMode === "DEBUG" ? {
        host: "localhost",
        port: 8081
      } : undefined,
      defaultHttpClient: this.onCreateDefaultHttpClient()
    })
    AppStorage.setOrCreate('RNOHCoreContext', this.rnInstancesCoordinator.getRNOHCoreContext())
    ```

    这时会把 `RNOHCoreContext` 存储到应用全局，后续可通过 `AppStorage.get` 方法获取 `RNOHCoreContext` 实例对象。

2. 在 `RNApp` 初始化时，会调用 `RNOHCoreContext` 中的 `createAndRegisterRNInstance` 方法来创建和注册 `RNInstance`。

    ```typescript
    private getOrCreateRNInstance(): Promise<RNInstance> {
      if (Object.keys(this.rnInstanceConfig).includes("rnInstance")) {
        return Promise.resolve((this.rnInstanceConfig as CustomRNInstance).rnInstance)
      } else {
        const options = this.rnInstanceConfig
        this.shouldDestroyRNInstance = true
        return this.rnohCoreContext!.createAndRegisterRNInstance(options as RNInstanceOptions)
      }
    }
    ...
    this.rnInstance = await this.getOrCreateRNInstance()
    ```

##### RNInstanceOptions

| 参数名                   | 类型    | 必填 | 说明                                       |
| ------------------------ | ------- | ---- | ------------------------------------------ |
| createRNPackages         | void    | 是   | 外部功能包管理（向框架提供额外的功能特性）。 |
| enableDebugger           | boolean | 否   | 是否启动Hermes debugger模式。                       |
| enableBackgroundExecutor | boolean | 否   | 是否启动后台线程，建议关闭后台线程。     |
| enableNDKTextMeasuring   | boolean | 否   | 是否启用C++原生文本测算接口。                    |
| enableImageLoader        | boolean | 否   | 是否启动图片缓存。                           |
| enableCAPIArchitecture   | boolean | 否   | 是否使用CAPI架构，默认开启。                 |
| assetsDest               | string  | 否   | 指定资源文件路径。                           |
| arkTsComponentNames      | Array<string>  | 是   | ArkTS自定义组件的名称。                           |
| hspModuleName      | string  | 否   | 存放图片资源的HSP模块名称。                           |
| backPressHandler         | () => void  | 否   | 当RN JS端不处理侧滑返回事件时的自定义处理逻辑。   |
| httpClient               | [HttpClient](https://developer.huawei.com/consumer/cn/doc/system-References/network-httpclient-0000001073658008)  | 否  | 指定Http客户端处理类。    |
| fontResourceByFontFamily | Record<string, Resource>  | 否   | 注册自定义字体。 |
| jsvmInitOptions | ReadonlyArray<JSVMInitOption>  | 否   | JSVM启动项 |

#### 销毁

`RNApp` 执行 `aboutToDisappear` 时，会调用 `RNOHCoreContext` 的 `destroyAndUnregisterRNInstance` 方法来销毁 `RNInstance`。

```javascript
aboutToDisappear() {
  this.cleanUp()
}

private async cleanUp() {
  const stopTracing = this.logger.clone("cleanUp").startTracing()
  this.shouldShow = false
  if (this.shouldDestroyRNInstance) {
    await this.rnohCoreContext!.destroyAndUnregisterRNInstance(this.rnInstance)
  }
  this.cleanUpCallbacks.forEach(cleanUp => cleanUp())
  stopTracing()
}
```

### 关键参数

| 参数名                   | 类型                                    | 说明                                             |
| ------------------------ | --------------------------------------- | ------------------------------------------------ |
| descriptorRegistry       | DescriptorRegistry                      | Descriptor注册表，用于管理ArkTS Fabric组件的数据。 |
| cppEventEmitter          | EventEmitter<Record<string, unknown[]>> | ArkTS侧订阅CPP侧传过来的消息，并进行处理。         |
| componentManagerRegistry | ComponentManagerRegistry                | ArkTS 组件管理注册表。                             |

### 方法

#### getLifecycleState()

获取生命周期状态。

##### 返回类型：LifecycleState

| 枚举值        | 说明   |
| ------------- | ------ |
| BEFORE_CREATE | 创建前。 |
| PAUSED        | 暂定。   |
| READY         | 就绪。   |

#### subscribeToLifecycleEvents

```javascript
subscribeToLifecycleEvents: <TEventName extends keyof LifecycleEventArgsByEventName>(
  eventName: TEventName,
  listener: (...args: LifecycleEventArgsByEventName[TEventName]) => void
) => () => void;
```

订阅生命周期事件。

##### 参数

| 参数名    | 类型       | 必填 | 说明         |
| --------- | ---------- | ---- | ------------ |
| eventName | TEventName | 是   | 事件名称。     |
| listener  | void       | 是   | 监听回调方法。 |

#### callRNFunction

```typescript
callRNFunction(moduleName: string, functionName: string, args: unknown[]): void
```

回调 React js 端的函数。

###### 参数

| 参数名       | 类型      | 必填 | 说明                                   |
| ------------ | --------- | ---- | -------------------------------------- |
| moduleName   | string    | 是   | JavaScript文件的名称。                   |
| functionName | string    | 是   | 在module中定义的函数名，在JS文件中导出。 |
| args         | unknown[] | 是   | 函数应该接收的参数。                     |

#### emitDeviceEvent

```typescript
emitDeviceEvent(eventName: string, payload: any): void
```

向 React js 端的 `RCTDeviceEventEmitter` 模块的 emit 发送事件件。

##### 参数

| 参数名    | 类型   | 必填 | 说明       |
| --------- | ------ | ---- | ---------- |
| eventName | string | 是   | 事件名称。   |
| payload   | any    | 是   | 发送的数据。 |

#### emitComponentEvent

```typescript
emitComponentEvent(tag: Tag, eventName: string, payload: any): void
```

发送组件事件，该事件由 C++ 端的 `EventEmitRequestHandler` 接收。该处理程序调用组件的 EventEmitter 上的适当方法。

##### 参数

| 参数名    | 类型   | 必填 | 说明          |
| --------- | ------ | ---- | ------------- |
| tag       | Tag    | 是   | 组件对应的Tag。 |
| eventName | string | 是   | 事件名称。      |
| payload   | any    | 是   | 发送的数据。    |

#### getBundleExecutionStatus

```typescript
getBundleExecutionStatus(bundleURL: string) : BundleExecutionStatus | undefined
```

获取 bundle 执行状态，`RNApp` 用来避免两次加载相同的包。

##### 参数

| 参数名    | 类型   | 必填 | 说明        |
| --------- | ------ | ---- | ----------- |
| bundleURL | string | 是   | bundle 地址。 |

##### 返回类型：BundleExecutionStatus | undefined

BundleExecutionStatus 枚举类型。

| 枚举值  | 说明   |
| ------- | ------ |
| RUNNING | 运行中。 |
| DONE    | 完成。   |

#### enableFeatureFlag

```typescript
enableFeatureFlag(featureFlagName: FeatureFlagName): void
```

启用特性标志。它可能会在将来被删除，因为通常在创建 `RNInstance` 时需要提供特性标志。

##### 参数

| 参数名          | 类型            | 必填 | 说明           |
| --------------- | --------------- | ---- | -------------- |
| featureFlagName | FeatureFlagName | 是   | 特性标识枚举值。 |

##### FeatureFlagName枚举类型

| 枚举值                      | 说明                 |
| --------------------------- | -------------------- |
| ENABLE_RN_INSTANCE_CLEAN_UP | 启动`RNInstance`清理。 |
| NDK_TEXT_MEASUREMENTS       | 使用C++原生测算接口。  |
| IMAGE_LOADER                | 使用图片缓存。         |
| C_API_ARCH                  | 使用CAPI架构。         |

#### isFeatureFlagEnabled

```typescript
isFeatureFlagEnabled(featureFlagName: FeatureFlagName): boolean
```

判断给定的特性标识是否开启。

##### 参数

| 参数名          | 类型                                        | 必填 | 说明           |
| --------------- | ------------------------------------------- | ---- | -------------- |
| featureFlagName | [FeatureFlagName](#featureflagname枚举类型) | 是   | 特性标识枚举值。 |

#### runJSBundle

```typescript
runJSBundle(jsBundleProvider: JSBundleProvider): Promise<void>;
```

读取 JS Bundle 并执行加载流程。

##### 参数

| 参数名           | 类型             | 必填 | 说明            |
| ---------------- | ---------------- | ---- | --------------- |
| jsBundleProvider | JSBundleProvider | 是   | JS Bundle提供者。 |

#### getTurboModule

```typescript
getTurboModule<T extends TurboModule>(name: string): T
```

获取对应 TurboModule 实例。

##### 参数

| 参数名 | 类型   | 必填 | 说明                |
| ------ | ------ | ---- | ------------------- |
| name   | string | 是   | TurboModule对应名称。 |

#### createTurboModulesFactory(ctx)<sup>(deprecated)<sup> 

```typescript
createTurboModulesFactory(ctx: UITurboModuleContext): TurboModulesFactory
```

创建TurboModule工厂，从 RNOHVersion 0.72.38 开始废弃，使用[createUITurboModuleFactory](#createuiturbomodulefactorydeprecated)代替。

##### 参数

| 参数名 | 类型                  | 必填 | 说明                |
| ------ | -------------------- | ---- | ------------------- |
|  ctx   | UITurboModuleContext | 是   | UITurboModule上下文对象 |

#### createUITurboModuleFactory<sup>(deprecated)<sup>

```typescript
createUITurboModuleFactory(ctx: UITurboModuleContext): UITurboModuleFactory
```

创建TurboModule工厂，从 RNOHVersion 0.72.38 开始废弃，使用[getUITurboModuleFactoryByNameMap](#getuiturbomodulefactorybynamemap) 和 [createEagerUITurboModuleByNameMap](#createeageruiturbomodulebynamemap)代替。

##### 参数

| 参数名 | 类型                  | 必填 | 说明                |
| ------ | -------------------- | ---- | ------------------- |
|  ctx   | UITurboModuleContext | 是   | UITurboModule上下文对象 |

#### createWorkerTurboModuleFactory<sup>(deprecated)<sup>

```typescript
createWorkerTurboModuleFactory(ctx: WorkerTurboModuleContext): WorkerTurboModuleFactory | null
```

创建[worker线程](架构介绍.md#worker线程)TurboModule工厂，从 RNOHVersion 0.72.38 开始废弃，使用[getAnyThreadTurboModuleFactoryByNameMap](#getanythreadturbomodulefactorybynamemap) 和 [createEagerAnyThreadTurboModuleByNameMap](#createeageranythreadturbomodulebynamemap)代替。

##### 参数

| 参数名 | 类型                      | 必填 | 说明                     |
| ------ | -------------------------| ---- | ------------------------- |
|  ctx   | WorkerTurboModuleContext | 是   | WorkerTurboModule上下文对象 |

#### getUITurboModuleFactoryByNameMap

```typescript
getUITurboModuleFactoryByNameMap(): Map<string, (ctx: UITurboModuleContext) => UITurboModule | null>
```

通过此方法获取Factory工厂类，返回一个模块工厂映射表，此方法在[UI线程](架构介绍.md#mainui线程)中使用。

#### getAnyThreadTurboModuleFactoryByNameMap

```typescript
getAnyThreadTurboModuleFactoryByNameMap(): Map<string, (ctx: AnyThreadTurboModuleContext) => AnyThreadTurboModule | null>
```

通过此方法获取Factory工厂类，返回一个模块工厂映射表，此方法在[worker线程](架构介绍.md#worker线程)中使用。

#### createEagerUITurboModuleByNameMap

```typescript
createEagerUITurboModuleByNameMap(ctx: UITurboModuleContext): Promise<Map<string, UITurboModule>>
```

创建TurboModule 映射表，注册需要在应用启动时立即创建的[UI线程](架构介绍.md#mainui线程)TurboModule。

##### 参数

| 参数名 | 类型                  | 必填 | 说明                |
| ------ | -------------------- | ---- | ------------------- |
|  ctx   | UITurboModuleContext | 是   | UITurboModule上下文对象 |

#### createEagerAnyThreadTurboModuleByNameMap

```typescript
createEagerAnyThreadTurboModuleByNameMap(ctx: AnyThreadTurboModuleContext): Promise<Map<string, AnyThreadTurboModule>>
```

创建TurboModule 映射表，注册需要在应用启动时立即创建的[worker线程](架构介绍.md#worker线程)TurboModule。

##### 参数

| 参数名 | 类型    | 必填  | 说明                |
| ------| ------ | ---- | ----------------------------- |
| ctx   | string | 是   | AnyThreadTurboModule上下文对象 |

#### createSurface

```typescript
createSurface(appKey: string): SurfaceHandle
```

由 `RNSurface` 使用，会在 React Native 中创建一个 Surface。

##### 参数

| 参数名 | 类型   | 必填 | 说明     |
| ------ | ------ | ---- | -------- |
| appKey | string | 是   | 开发者在JS侧使用`AppRegistry.registerComponent`注册的名称。 |

#### updateState

```typescript
updateState(componentName: string, tag: Tag, state: unknown): void
```

向 ComponentNapiBinder.h::updateState 发送状态更新请求，更新 C++ 端状态。

##### 参数

| 参数名        | 类型    | 必填 | 说明         |
| ------------- | ------- | ---- | ------------ |
| componentName | string  | 是   | 组件名称。     |
| tag           | Tag     | 是   | 组件Tag。      |
| state         | unknown | 是   | 要更新的状态。 |

#### bindComponentNameToDescriptorType

```typescript
bindComponentNameToDescriptorType(componentName: string, descriptorType: string)
```

将 Descriptor 类型和组件名进行关联，用于将核心组件替换为自定义组件。

##### 参数

| 参数名         | 类型   | 必填 | 说明                                                         |
| -------------- | ------ | ---- | ------------------------------------------------------------ |
| componentName  | string | 是   | 组件名称。                                                     |
| descriptorType | string | 是   | 为给定的`descriptorType`在`ComponentBuilderContext`中提供的值。 |

#### getComponentNameFromDescriptorType

```typescript
getComponentNameFromDescriptorType(descriptorType: string): string
```

根据 Descriptor 类型获取组件名称。

##### 参数

| 参数名         | 类型   | 必填 | 说明           |
| -------------- | ------ | ---- | -------------- |
| descriptorType | string | 是   | Descriptor类型。 |

#### blockComponentsGestures

```javascript
blockComponentsGestures(targetComponentTag: Tag): (() => void)
```

阻止 targetComponent 及其祖先组件中的手势。由 react-native-gesture-handler 在 `RNScrollView` 或其他可滚动组件中平移时使用。

##### 参数

| 参数名             | 类型 | 必填 | 说明          |
| ------------------ | ---- | ---- | ------------- |
| targetComponentTag | Tag  | 是   | 目标组件的Tag。 |

##### 返回

返回一个可以抵消这个效应的方法。

#### getInitialBundleUrl

```typescript
getInitialBundleUrl(): string | undefined
```

返回第一个加载的 JS Bundle URL。SourceCodeTurboModule 使用这个方法来生成正确的堆栈跟踪。有些应用将它们捆绑包分开来提高启动性能，因此有了“初始”这个词。

#### getAssetsDest

```typescript
getAssetsDest(): string
```

返回 assets 资源的路径。

#### postMessageToCpp

```typescript
postMessageToCpp(name: string, payload: any)
```

ArkTS 向 C++ 端发送消息。由 `ArkTSMessageHub:Observer` 或 `ArkTSMessageHandler` 处理。

##### 参数

| 参数名  | 类型   | 必填 | 说明       |
| ------- | ------ | ---- | ---------- |
| name    | string | 是   | 消息名称。   |
| payload | any    | 是   | 发送的数据。 |

#### registerFont

```typescript
registerFont(fontFamily: string, fontResource: Resource | string): void
```

注册字体。

###### 参数

| 参数名       | 类型      | 必填 | 说明                                   |
| ------------ | --------- | ---- | -------------------------------------- |
| fontFamily   | string    | 是   | 字体名。                   |
| fontResource | Resource / string    | 是   | 字体资源对象，或字体资源文件的沙箱路径。 |

## RNOHCoreContext

### 基本介绍

`RNOHCoreContext` 提供可跨 `RNInstances` 共享的依赖项和实用程序。还包括创建和销毁`RNInstance` 的方法，对于特定于 `RNInstance` 的方法。本节主要介绍了 `RNOHCoreContext` 的接口类型。

### 关键参数

| 参数名                 | 类型                    | 说明                                                         |
| ---------------------- | ----------------------- | ------------------------------------------------------------ |
| uiAbilityContext       | common.UIAbilityContext | `UIAbility`的上下文。                                          |
| safeAreaInsetsProvider | SafeAreaInsetsProvider  | SafeAreaInsets 提供者，SafeAreaInsets 提供了与屏幕边缘的距离，可以用来避免在状态栏、缺口或瀑布边缘下面呈现的内容。 |
| isDebugModeEnabled     | boolean                 | React Native 是否使用 Debug 模式。                                |
| launchUri              | string \| undefined     | ArkUI::Want::uri，应用自己的uri。                              |

### 关键方法

#### createAndRegisterRNInstance

```javascript
createAndRegisterRNInstance: (options: RNInstanceOptions, frameNodeFactory?: FrameNodeFactory) => Promise<RNInstance>
```

创建 `RNInstance` 并将其注册到 **RNOH** 的内部注册表中。

##### 参数

| 参数名           | 类型              | 必填 | 说明                        |
| ---------------- | ----------------- | ---- | --------------------------- |
| options          | RNInstanceOptions | 是   | `RNInstance` 的参数选项。      |
| frameNodeFactory | FrameNodeFactory  | 是   | 由 `RNSurface` 实例化的内部类。 |

#### destroyAndUnregisterRNInstance

```javascript
destroyAndUnregisterRNInstance: (rnInstance: RNInstance) => void
```

销毁 `RNInstance` 并从 **RNOH** 的内部注册表中注销。

##### 参数

| 参数名     | 类型       | 必填 | 说明                 |
| ---------- | ---------- | ---- | -------------------- |
| rnInstance | RNInstance | 是   | 要销毁的 `RNInstance`。 |

#### getDisplayMetrics

```javascript
getDisplayMetrics: () => DisplayMetrics
```

返回当前屏幕信息。

#### getUIAbilityState

```javascript
getUIAbilityState: () => UIAbilityState
```

返回应用程序的当前状态。

#### dispatchBackPress

```javascript
dispatchBackPress: () => void
```
用 root ArkUI 组件的 `onBackPress` 调用，目前这是 BackPress 的唯一响应方式。

#### reportRNOHError

| 参数名     | 类型       | 必填 | 说明                 |
| ---------- | ---------- | ---- | -------------------- |
| rnohError | RNOHError | 是   | 上报的错误 |

上报 **RNOH** 错误用于监听。

## RNComponentContext

`RNComponentContext` 是 React Native for OpenHarmony 构造组件时使用的上下文信息，是 `RNOHContext` 的子类。

### constructor

| 参数名  | 类型     | 必填 | 说明     |
| ------- | -------- | ---- | -------- |
| rnohContext | RNOHContext | 是 | 用于组件或TurboModule使用的上下文信息。 |
| wrappedCustomRNComponentBuilder | WrappedCustomRNComponentBuilder | 是 | 用于构造ArkTS侧自定义组件的构造器。 |
| wrappedRNComponentBuilder | WrappedRNComponentBuilder | 是 | 用于构造 ArkTS 侧 **RN** 子组件的构造器。 |
| rnComponentDataSourceFactoriesByDescriptorType | Map<string, RNComponentDataSourceFactory> | 是 | 用来存储 LazyForEach 的 DataSource 对象。 |

### 方法

#### runOnWorkerThread<TParams, TResult, TRunnable>(runnable, params)

| 泛型类型     | 继承父类型       | 必填 | 说明                 |
| ---------- | ---------- | ---- | -------------------- |
| TParams | SupportedTaskParams | 是  | 参数类型, 受 **WORKER** 线程通信机制限制，必须为可序列化类型。 |
| TResult | lang.ISendable \| void | 是 | 返回值类型, 受 **WORKER** 线程通信机制限制，必须为可序列化类型。 |
| TRunnable | WorkerTaskRunnable<TParams, TResult> | 是 | 可执行任务对象类型。 |

| 参数名     | 类型       | 必填 | 说明                 |
| ---------- | ---------- | ---- | -------------------- |
| runnable | TRunnable | 是 | 可执行任务对象。 |
| params  | TParams | 是 | 可执行任务参数。 |

以`params`作为参数，在**WORKER** 线程并行执行`runnable.run()`，返回一个`Promise<TResult>`对象，可用于读取`runnable.run()`的返回值。

使用本特性前需启用**WORKER** 线程特性。如何启用**WORKER** 线程特性详见[架构介绍.md#worker线程](./架构介绍.md#worker线程)。

使用示例详见[常见开发场景.md#如何在worker-线程并行执行自定义任务](./常见开发场景.md#如何在worker-线程并行执行自定义任务)。

## EtsUITurboModuleContext

`EtsUITurboModuleContext` 是 React Native for OpenHarmony 构造`EtsUITurboModule`时使用的上下文信息，是 `RNOHContext` 的子类。

### 方法

#### runOnWorkerThread<TParams, TResult, TRunnable>(runnable, params)

同上文[API接口说明.md#runonworkerthreadtparams-tresult-trunnablerunnable-params](./API接口说明.md#runonworkerthreadtparams-tresult-trunnablerunnable-params)。

## JSBundleProvider

### 基础介绍

JS Bundle 提供者，用于初始化 bundle 信息，获取 bundle 具体内容。本节主要介绍了 `RNOHCoreContext` 的接口类型。

### 类型

#### AnyJSBundleProvider

所有 bundle 的加载器，从小到大依次加载。

##### 参数

| 参数名            | 类型               | 必填 | 说明                                                 |
| ----------------- | ------------------ | ---- | ---------------------------------------------------- |
| jsBundleProviders | JSBundleProvider[] | 是   | JSBundleProvider 数组，可以传入多个 JS Bundle Provider。 |

#### MetroJSBundleProvider

使用 Metro 服务加载 bundle。

#### FileJSBundleProvider

从沙箱目录下加载 bundle。

##### 参数

| 参数名  | 类型     | 必填 | 说明     |
| ------- | -------- | ---- | -------- |
| path    | string   | 是   | 文件地址。 |
| appKeys | string[] | 否   | 开发者在JS侧使用`AppRegistry.registerComponent`注册的名称数组。 |

#### ResourceJSBundleProvider

从资源文件中加载 bundle，专指 `resources/rawfile` 下的 bundle 文件。

| 参数名          | 类型                                                         | 必填 | 说明         |
| --------------- | ------------------------------------------------------------ | ---- | ------------ |
| resourceManager | [resmgr.ResourceManager](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/resourcemanager-V5) | 是   | 资源管理对象。 |
| path            | string                                                       | 是   | bundle文件名。 |
| appKeys         | string[]                                                     | 否   | 开发者在 JS 侧使用 `AppRegistry.registerComponent` 注册的名称。     |

#### 

## RNInstancesCoordinator

### 基本介绍

`RNAbility` 大部分代码由 **RNOH** 实现，以提高稳定性。这类项目不使用 `RNAbility` 和 `RNApp`。为此，React Native for OpenHarmony 提供了 `RNInstancesCoordinator` 给开发者，旨在更加灵活的控制 **RN** 的启动，

### 方法

#### create

```typescript
static create(dependencies: RNInstancesCoordinatorDependencies, options?: RNInstancesCoordinatorOptions)
```

根据给定的 `RNInstancesCoordinatorDependencies` 与 `RNInstancesCoordinatorOptions` 返回 `RNInstancesCoordinator` 的对象。

##### 参数

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| dependencies | [RNInstancesCoordinatorDependencies](#rninstancescoordinatordependencies) | 是 | 用于构建 `RNInstancesCoordinator` 时使用的依赖。 |
| options | [RNInstancesCoordinatorOptions](#rninstancescoordinatoroptions) | 否 | 构建 `RNInstancesCoordinator` 时使用的选项。 |

#### onDestroy

```typescript
rnInstancesCoordinator?.onDestroy(): void
```

`onDestroy` 方法在 UIAbility 实例销毁时调用，进入 **RN**。可以在[onDestroy()](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityondestroy)回调中进行系统资源的释放、数据的保存等操作。

#### getBuildMode

```typescript
rnInstancesCoordinator?.getBuildMode(): BuildMode
```
获取项目的构建方式: 'RELEASE' 或 'DEBUG'；


#### getRNOHCoreContext


```typescript
rnInstancesCoordinator?.getRNOHCoreContext(): RNOHCoreContext
```
获取 **RNInstancesCoordinator** 实例的 **RNOHCoreContext**；

#### onForeground

```typescript
rnInstancesCoordinator?.onForeground(): void
```

开发者应在[`onForeground`](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonforeground)在 `UIAbility` 实例切换至前台时调用该方法，向 React 侧通知应用已经从后台切换至前台。

#### onBackground

```typescript
rnInstancesCoordinator?.onBackground(): void
```
开发者应在[`onBackground`](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonbackground)在`UIAbility`实例切换至后台时调用该方法，向 React 侧通知应用已经从前台切换至后台。

#### onWindowStageChange

```typescript
rnInstancesCoordinator?.onWindowStageChange(windowStageEvent: window.WindowStageEventType): void
```
开发者需要监听[windowStageEvent](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5#onwindowstageevent9)事件，并调用该方法，向 React 侧通知 WindowStage 生命周期变化。

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| windowStageEvent | [WindowStageEventType](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5#windowstageeventtype9) | 是 | WindowStage 类型。 |

#### onWindowSizeChange

```typescript
rnInstancesCoordinator?.onWindowSizeChange(windowSize: window.Size): void
```

开发者需要监听[windowSizeChange](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5#onwindowsizechange7)事件，并调用该方法，向 React 侧通知窗口尺寸变化。

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| windowSize | [Size](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5#size7) | 是 | 更改后的窗口大小。 |

#### onConfigurationUpdate

```typescript
rnInstancesCoordinator?.onConfigurationUpdate(systemConfiguration: AbilityConfiguration.Configuration): void
```

开发者应在系统配置发生变化时调用该方法，通知 React Native for openHarmony 系统相关设置已经发生了变化。

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| systemConfiguration | [Configuration](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-application-configuration-V5) | 是 | 当系统配置发生变化，向 React Native for OpenHarmony 通知。 |

#### onMemoryLevel

```typescript
rnInstancesCoordinator?.onMemoryLevel(memoryLevel: number): void
```
开发者应在内存到达不同级别时，调用该方法，向 React Native 通知。
| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| memoryLevel | [AbilityConstant.MemoryLevel](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-abilityconstant-V5#abilityconstantmemorylevel) | 是 | 通知 React Native 内存级别发生改变。 |

#### onNewWant

```typescript
rnInstancesCoordinator?.onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void
```
开发者需要在 UIAbility 实例已经启动并在前台运行过，由于某些原因切换到后台，再次启动该 UIAbility 实例时会回调执行的 UIAbility.onNewWant 中调用该方法。

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| want | [Want](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-want-V5) | 是 | Want 类型参数，如 ability 名称，包名等。 | 
| launchParam | [AbilityConstant.LaunchParam](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-abilityconstant-V5#abilityconstantlaunchparam) | 是 | UIAbility 启动的原因、上次异常退出的原因信息。 |


#### RNInstancesCoordinatorDependencies

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| fontSizeScale | number \| undefined | 是 | 字体大小缩放比例，值为 1 的时候为正常大小字体，小于1缩小，大于 1 放大。 |
| logger | RNOHLogger | 是 | 用于打印日志。 |
| uiAbilityContext | [UIAbilityContext](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-inner-application-uiabilitycontext-V5) | 是 | 使用 React Native 的 Ability 的上下文信息。 |
| rnohWorkerScriptUrl | string | 否 | 部分TurboModule需要启动Worker线程才可以正常启用，需要设置该属性，启用对应 name 的 `RNInstance` 的 Worker 线程。 |
| defaultBackPressHandler | Function | 是 | React 侧执行的 `exitApp` 的原生侧实现。 |

#### RNInstancesCoordinatorOptions

| 参数名 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| launchURI | string | 否 | `Linking.getInitialURL()`获取到的初始URL。 |
| onGetPackagerClientConfig | (buildMode: BuildMode) => JSPackagerClientConfig \| undefined | 否 | 返回Metro配置的函数。 | 
| defaultHttpClient | [HttpClient](https://developer.huawei.com/consumer/cn/doc/system-References/network-httpclient-0000001073658008) | 否 | 用于在 **RN** 与原生混合的应用中，重用 Cookies 或打印日志。 |
| disableCleaningRNInstances | boolean | 否 | 是否启用 CleanUP 线程，清理 **RN** 实例。 |

# API Description
> WARNING: This document may become outdated. Documentation comments directly in the code are more likely to be up-to-date

## RNAbility

### Introduction

The `RNAbility` class inherits the [UIAbility](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5) class of ArkUI and encapsulates necessary operations for starting React Native (RN), such as processing different lifecycle states and subscribing to changes of system environment variables. Application developers need to extend this class. For details about how to use the class, see the following code:

```ts
import {RNAbility} from '@rnoh/react-native-openharmony';

export default class EntryAbility extends RNAbility {
  getPagePath() {
    return 'pages/Index';
  }
}

```

### Methods

#### createLogger

```ts
protected createLogger(): RNOHLogger
```

#### onPrepareRNInstance

```typescript
protected async onPrepareRNInstance(rnohCoreContext: RNOHCoreContext): void
```

Creates an `RNInstance` and loads a package in the ability. `RNAbility` waits for this method to complete before loading the content. This approach reduces the flicker that may occur after the splash screen is hidden, and the RN application is ready.

##### Parameter

| Parameter         | Type                               | Description               |
| --------------- | ----------------------------------- | ------------------ |
| rnohCoreContext | [RNOHCoreContext](#rnohcorecontext) |                    |

#### onWindowSetup

```typescript
protected async onWindowSetup(window: window.Window): void
```

Customizes the screen orientation and window size.

##### Parameter

| Parameter| Type                                                        | Description                    |
| ------ | ------------------------------------------------------------ |  ------------------------ |
| window | [window.Window](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5) | Current window instance.|

#### onWindowStageSetup

```typescript
protected async onWindowStageSetup(windowStage: window.WindowStage): void
```

Loads specific page content for the current window.

##### Parameter

| Parameter     | Type                                                        | Description                    |
| ----------- | ------------------------------------------------------------ | ------------------------ |
| windowStage | [window.WindowStage](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-window-V5#loadcontentdeprecated)| Current window instance.|

#### getPagePath

```typescript
protected abstract getPagePath(): string
```

Gets the path of the entry page, which is defined in **module.json5**. This method is required for the extension class.

#### defaultBackPressHandler

```typescript
protected defaultBackPressHandler(): void
```

Handles the back press event. This method is called when the React application does not handle the back press event.

#### onCreate

```typescript
onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void
```

Creates a UIAbility instance. The system calls the [onCreate()](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityoncreate) callback. In this callback, you can perform page initialization operations, for example, defining variables or loading resources.

In this implementation, the `RNInstancesCoordinator` and `RNOHCoreContext` instances are created, and [`AppStorage.setOrCreate()`](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/ts-state-management-V5#setorcreate10) is called to store `RNOHCoreContext` to the global application.

##### Parameter

| Parameter| Type                                                        | Description                      |
| ------ | ------------------------------------------------------------ | -------------------------- |
| want   | [Want](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V5/want-overview-V5) | Used to transfer information between application components.|
| launchParam   | [AbilityConstant.LaunchParam](https://developer.huawei.com/consumer/cn/doc/harmonyos-references/js-apis-app-ability-abilityconstant#launchparam) | Parameters for application launch, including the reason for application launch and the reason for the last application exit.|


#### onCreateDefaultHttpClient

```ts
protected onCreateDefaultHttpClient(): undefined | HttpClient
```

Returns the HttpClient shared by RNInstances.

#### onDestroy

```typescript
onDestroy(): void
```

Destroys a UIAbility instance. The [onDestroy()](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityondestroy) callback can be used to release system resources and save data.

All registered RNInstances with the **onDestroy** method will be destroyed.

#### onWindowStageCreate

```typescript
onWindowStageCreate(windowStage: window.WindowStage): void
```

Creates a WindowStage after a UIAbility instance is created but before it switches to the foreground. After a WindowStage is created, the [onWindowStageCreate()](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonwindowstagecreate) callback can be used to set UI loading and WindowStage event subscription.

##### Parameter

| Parameter     | Type                                                        | Description                    |
| ----------- | ------------------------------------------------------------ | ------------------------ |
| windowStage | [window.WindowStage](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-window-V5#loadcontent9)| Current window instance.|

#### onMemoryLevel

```typescript
onMemoryLevel(level: AbilityConstant.MemoryLevel): void
```

Adjusts the memory level. [onMemoryLevel](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-abilitystage-V5#abilitystageonmemorylevel) is the AbilityStage capability at the bottom layer of the system.

##### Parameter

| Parameter| Type                                                        | Description                                        |
| ------ | ------------------------------------------------------------ | -------------------------------------------- |
| level  | [AbilityConstant.MemoryLevel](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-abilityconstant-V5#abilityconstantmemorylevel) | Memory level, indicating the current memory usage.|

#### onConfigurationUpdate

```typescript
onConfigurationUpdate(config: AbilityConfiguration.Configuration): void
```

Updates the global configuration. [onConfigurationUpdate](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-abilitystage-V5#abilitystageonconfigurationupdate) is the AbilityStage capability at the bottom layer of the system. It is used to notify global configuration changes.

##### Parameter

| Parameter| Type                                                        | Description                                                        |
| ------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| config  | [AbilityConfiguration.Configuration](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-configuration-V5) | Update of global configuration, including the system language and color mode.|

#### onForeground

```typescript
onForeground(): void
```

Switches the UIAbility instance to the foreground. [onForeground](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonforeground) is triggered.

The `appState` variable in `RNInstancesCoordinator` is set to **FOREGROUND**.

#### onBackground

```typescript
onBackground(): void
```

Switches the UIAbility instance to the background. [onBackground](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonbackground) is triggered.

The `appState` variable in `RNInstancesCoordinator` is set to **BACKGROUND**.

`RNAbility` aims to move most of the code to the RNOH side to improve stability. If a more flexible implementation mode is required, you can use `RNInstancesCoordinator`. This section describes the API types of `RNAbility`.

## RNApp

### Introduction

`RNApp` is a module used to start and manage `RNInstance` and `RNSurface`. It encapsulates the behavior of creating and starting a single `RNInstance` and a single `RNSuface`. You do not need to consider when to load the JS bundle. If your application requires only a single `RNInstance` and a single `RNSurface`, you can use `RNApp`s to create an RN application, freeing you from complex operations. For applications with multiple surfaces, you can use `RNSurface`. This section describes the API types and usage of `RNApp`.

### Key Parameters

| Parameter                         | Type                           | Required| Description                                                        |
| ------------------------------- | ------------------------------- | ---- | ------------------------------------------------------------ |
| rnInstanceConfig                | RNInstanceConfig                | Yes  | Used to create an `RNInstance` or `RNInstanceOptions`. If [RNInstanceOptions](#rninstanceoptions) is provided, this component creates and manages `RNInstance`.|
| appKey                          | string                          | Yes  | Name registered by using `AppRegistry.registerComponent` on the JS side.   |
| jsBundleProvider                | JSBundleProvider                | No  | Provider for running JS Bundle in `RNInstance`. A surface and a container are created after a bundle is loaded.|
| initialProps                    | Record<string, string>          | No  | Native initial properties of a React Native application.                          |
| wrappedCustomRNComponentBuilder | WrappedCustomRNComponentBuilder | Yes  | Builder for a custom Fabric component.                                  |
| onSetUp                         | void                            | No  | Called before an `RNInstance` is created and a surface is visible.       |

### Methods

#### onSetUp

```javascript
onSetUp: (rnInstance: RNInstance) => Promise<void> | void = () => Promise.resolve()
```

Calls the method to be executed in `RNInstance`. This callback is called before an `RNInstance` is created and a surface is visible.

##### Parameter

| Parameter    | Type      | Required| Description            |
| ---------- | ---------- | ---- | ---------------- |
| rnInstance | RNInstance | Yes  | `RNInstance` object.|

### API Usage Description

```javascript
RNApp({
  // Used to specify the configuration of the React Native instance.
  rnInstanceConfig: {
    // External function package management (providing additional functions and features for the framework)
    createRNPackages,
    // Whether to enable the C++ native text measuring API.
    enableNDKTextMeasuring: true,
    // Whether to enable the background executor.
    enableBackgroundExecutor: false,
    // Whether to use the CAPI architecture. Defaults to true.
    enableCAPIArchitecture: true,
    // Name of the custom ArkTS component
    arkTsComponentNames,
  },
  // Pass the initial properties to the React Native application.
  initialProps: { "foo": "bar" } as Record<string, string>,
  appKey: "app_name",
  // Custom component builder
  wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
  // Call the method to be executed in 'RNInstance'. This callback is called before an rnInstance is created and a surface is visible.
  onSetUp: (rnInstance) => {
    rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP")
  },
  // Used to specify the mode of providing the JavaScript bundle file.
  jsBundleProvider: new TraceJSBundleProviderDecorator(
    new AnyJSBundleProvider([
    new MetroJSBundleProvider(),
    /**
     * Note: Load a bundle from '/data/app/el2/100/base/com.rnoh.tester/files/bundle.harmony.js'. Path mismatch is caused by the application sandbox on OpenHarmony.
     */
    new FileJSBundleProvider('/data/storage/el2/base/files/bundle.harmony.js'),
    new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'hermes_bundle.hbc'),
    new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'bundle.harmony.js')
  ]),
  this.rnohCoreContext.logger),
})
```



## RNSurface

### Introduction

`RNSurface` is the root container of React Native. It is a native view used to render React Native components to the native UI. `RNSurface` is used to render the component tree to the screen. In the ArkUI C-API adaptation solution, rootView is a `XComponent`, which is used as a placeholder for mounting C-API components. This section describes the API types and usage of `RNSurface`.

To create a surface, you need to call [createSurface](#createsurface) of `RNInstance`. It is destroyed when `RNInstance` triggers `onDestory`. A surface is started in `onAreaChange` of RNSurface (by running the **start** command). Note that startSurface must be executed after loadJSbundle is complete. Otherwise, a white screen is displayed. You can use [getBundleExecutionStatus](#getbundleexecutionstatus) of `RNInstance` to obtain the bundle loading status and ensure that the time sequence is correct.

### Key Parameters

| Parameter       | Type                             | Required| Description           |
| ------------- | --------------------------------- | ---- | --------------- |
| ctx           | [RNOHContext \| RNComponentContext](#rncomponentcontext) | Yes  | RN component context.   |
| surfaceConfig | [SurfaceConfig](#surfaceconfig)                     | Yes  | Surface configuration.|

### API Usage Description

```typescript
RNSurface({
  ctx: this.ctx, // Component context created in RNApp.
  surfaceConfig: {
    initialProps: this.initialProps ?? {}, // Initial properties passed from RNApp.
    appKey: this.appKey, // App name passed from RNApp.
  } as SurfaceConfig2,
})
```
### SurfaceConfig

`SurfaceConfig` has two child classes: `SurfaceConfig1` and `SurfaceConfig2`, which can be selected as required.

#### SurfaceConfig1

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| surfaceHandle | SurfaceHandle | Yes| Handle to the RNSurface, which can be created by [createSurface](#createsurface).|

#### SurfaceConfig2

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| appKey | string | Yes| Name registered by using `AppRegistry.registerComponent` on the JS side.|
| initialProps | Record<string, any> | Yes| Native initial properties of a React Native application.|


## RNInstance

### Introduction

`RNInstance` is an instance of React Native. When using the RN framework, you must create and start one or more `RNInstance`s. An `RNInstance` manages the lifecycle callback of React Native and maintains the React Native component tree. This section describes the API types and usage of `RNInstance`.

### RNInstance Creation and Destruction

#### Creation

Generally, the `createAndRegisterRNInstance` method in `RNOHCoreContext` is used to create and register `RNInstance`. The procedure is as follows:

1. The application executes the `onCreate` method in `RNAbility` to create a `RNInstancesCoordinator` object instance. This class is the coordinator of `RNInstance` and is created for the use of `UIAbility`. It guides React Native and forwards device events to `RNInstance`.

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

    In this case, `RNOHCoreContext` is stored to the global application. You can use the `AppStorage.get` method to obtain the `RNOHCoreContext` instance object.

2. During `RNApp` initialization, the `createAndRegisterRNInstance` method in `RNOHCoreContext` is called to create and register `RNInstance`.

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

| Parameter                  | Type   | Required| Description                                      |
| ------------------------ | ------- | ---- | ------------------------------------------ |
| createRNPackages         | void    | Yes  | External function package management (providing additional functions and features for the framework).|
| enableDebugger           | boolean | No  | Whether to enable the Hermes debugger mode.                      |
| enableBackgroundExecutor | boolean | No  | Whether to start the background executor.    |
| enableNDKTextMeasuring   | boolean | No  | Whether to enable the C++ native text measuring API.                   |
| enableImageLoader        | boolean | No  | Whether to enable the image loader.                          |
| enableCAPIArchitecture   | boolean | No  | Whether to enable the CAPI architecture. It is enabled by default.                |
| assetsDest               | string  | No  | Path of the destination assets file.                          |
| arkTsComponentNames      | Array<string>  | Yes  | Name of a ArkTS custom component.                          |
| backPressHandler         | () => void  | No  | Custom handler logic when RN JS does not handle the back press event.  |
| httpClient               | [HttpClient](https://developer.huawei.com/consumer/en/doc/system-References/network-httpclient-0000001073658008) | No | HTTP client class.  
| caPathProvider | (url: string) => string | No | Specifies the CA certificate path rule |
| fontResourceByFontFamily | Record<string, Resource>  | No  | Registration of a custom font.|

#### Destruction

When `aboutToDisappear` is executed for `RNApp`, the `destroyAndUnregisterRNInstance` method of `RNOHCoreContext` is called to destroy `RNInstance`.

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

### Key Parameters

| Parameter                  | Type                                   | Description                                            |
| ------------------------ | --------------------------------------- | ------------------------------------------------ |
| descriptorRegistry       | DescriptorRegistry                      | Descriptor registry, which is used to manage the data of ArkTS Fabric component.|
| cppEventEmitter          | EventEmitter<Record<string, unknown[]>> | Subscription and handling of CPP events on the ArkTS side.        |
| componentManagerRegistry | ComponentManagerRegistry                | ArkTS component manager registry.                            |

### Methods

#### getLifecycleState()

Gets the lifecycle state.

##### Return type: LifecycleState

| Value       | Description  |
| ------------- | ------ |
| BEFORE_CREATE | Before creation.|
| PAUSED        | Paused.  |
| READY         | Ready.  |

#### subscribeToLifecycleEvents

```typescript
subscribeToLifecycleEvents: <TEventName extends keyof LifecycleEventArgsByEventName>(
  eventName: TEventName,
  listener: (...args: LifecycleEventArgsByEventName[TEventName]) => void
) => () => void;
```

Subscribes to lifecycle events.

##### Parameter

| Parameter   | Type      | Required| Description        |
| --------- | ---------- | ---- | ------------ |
| eventName | TEventName | Yes  | Event name.    |
| listener  | void       | Yes  | Listener callback method.|

#### callRNFunction

```typescript
callRNFunction(moduleName: string, functionName: string, args: unknown[]): void
```

Calls back the function of React JS.

###### Parameter

| Parameter      | Type     | Required| Description                                  |
| ------------ | --------- | ---- | -------------------------------------- |
| moduleName   | string    | Yes  | Name of the JavaScript file.                  |
| functionName | string    | Yes  | Function name defined in the module, which is exported from the JS file.|
| args         | unknown[] | Yes  | Arguments of the function.                    |

#### emitDeviceEvent

```typescript
emitDeviceEvent(eventName: string, payload: any): void
```

Emits an event to the RCTDeviceEventEmitter module on React JS.

##### Parameter

| Parameter   | Type  | Required| Description      |
| --------- | ------ | ---- | ---------- |
| eventName | string | Yes  | Event name.  |
| payload   | any    | Yes  | Sent data.|

#### emitComponentEvent

```typescript
emitComponentEvent(tag: Tag, eventName: string, payload: any): void
```

Emits a component event, which is handled by `EventEmitRequestHandler` of C++. The handler calls the appropriate method on `EventEmitter` of the component.

##### Parameter

| Parameter   | Type  | Required| Description         |
| --------- | ------ | ---- | ------------- |
| tag       | Tag    | Yes  | Component tag.|
| eventName | string | Yes  | Event name.     |
| payload   | any    | Yes  | Sent data.   |

#### getBundleExecutionStatus

```typescript
getBundleExecutionStatus(bundleURL: string) : BundleExecutionStatus | undefined
```

Gets the bundle execution status. `RNApp` is used to prevent the same package from being loaded twice.

##### Parameter

| Parameter   | Type  | Required| Description       |
| --------- | ------ | ---- | ----------- |
| bundleURL | string | Yes  | Bundle address.|

##### Return type: BundleExecutionStatus| undefined

BundleExecutionStatusmei enumeration types

| Value | Description  |
| ------- | ------ |
| RUNNING | Running.|
| DONE    | Done.  |

#### enableFeatureFlag

```typescript
enableFeatureFlag(featureFlagName: FeatureFlagName): void
```

Enables the feature flag. It may be deleted in the future, because the feature flag is usually required when creating an `RNInstance`.

##### Parameter

| Parameter         | Type           | Required| Description          |
| --------------- | --------------- | ---- | -------------- |
| featureFlagName | FeatureFlagName | Yes  | Feature flag name.|

##### FeatureFlagName enumeration types

| Value                     | Description                |
| --------------------------- | -------------------- |
| ENABLE_RN_INSTANCE_CLEAN_UP | Enables `RNInstance` cleanup.|
| NDK_TEXT_MEASUREMENTS       | Uses the Native C++ text measurement API. |
| IMAGE_LOADER                | Uses the image loader.        |
| C_API_ARCH                  | Uses the CAPI architecture.        |

#### isFeatureFlagEnabled

```typescript
isFeatureFlagEnabled(featureFlagName: FeatureFlagName): boolean
```

Enables or disables a feature flag.

##### Parameter

| Parameter         | Type                                       | Required| Description          |
| --------------- | ------------------------------------------- | ---- | -------------- |
| featureFlagName | [FeatureFlagName](#featureflagname-enumeration-types)| Yes  | Feature flag name.|

#### runJSBundle

```typescript
runJSBundle(jsBundleProvider: JSBundleProvider): Promise<void>;
```

Reads and runs the JS bundle.

##### Parameter

| Parameter          | Type            | Required| Description           |
| ---------------- | ---------------- | ---- | --------------- |
| jsBundleProvider | JSBundleProvider | Yes  | JS Bundle provider.|

#### getTurboModule

```typescript
getTurboModule<T extends TurboModule>(name: string): T
```

Gets a TurboModule instance.

##### Parameter

| Parameter| Type  | Required| Description               |
| ------ | ------ | ---- | ------------------- |
| name   | string | Yes  | TurboModule name.|

#### createSurface

```typescript
createSurface(appKey: string): SurfaceHandle
```

Creates a surface in React Native. This method is used by `RNSurface`.

##### Parameter

| Parameter| Type  | Required| Description    |
| ------ | ------ | ---- | -------- |
| appKey | string | Yes  | Name registered by using `AppRegistry.registerComponent` on the JS side.|

#### updateState

```typescript
updateState(componentName: string, tag: Tag, state: unknown): void
```

Updates a state to `ComponentNapiBinder.h::updateState`. The C++ status is updated.

##### Parameter

| Parameter       | Type   | Required| Description        |
| ------------- | ------- | ---- | ------------ |
| componentName | string  | Yes  | Component name.    |
| tag           | Tag     | Yes  | Component tag.     |
| state         | unknown | Yes  | State to be updated.|

#### bindComponentNameToDescriptorType

```typescript
bindComponentNameToDescriptorType(componentName: string, descriptorType: string)
```

Binds the descriptor type with the component name to replace the core component with a custom component.

##### Parameter

| Parameter        | Type  | Required| Description                                                        |
| -------------- | ------ | ---- | ------------------------------------------------------------ |
| componentName  | string | Yes  | Component name.                                                    |
| descriptorType | string | Yes  | Value provided in `ComponentBuilderContext` for the given `descriptorType`.|

#### getComponentNameFromDescriptorType

```typescript
getComponentNameFromDescriptorType(descriptorType: string): string
```

Gets the component name based on the descriptor type.

##### Parameter

| Parameter        | Type  | Required| Description          |
| -------------- | ------ | ---- | -------------- |
| descriptorType | string | Yes  | Descriptor type.|

#### blockComponentsGestures

```typescript
blockComponentsGestures(targetComponentTag: Tag): (() => void)
```

Blocks gestures in targetComponent and its ancestors. This method is used by react-native-gesture-handler to translate in `RNScrollView` or other scrollable components.

##### Parameter

| Parameter            | Type| Required| Description         |
| ------------------ | ---- | ---- | ------------- |
| targetComponentTag | Tag  | Yes  | Tag of the target component.|

##### Return

Returns a method that can offset this effect.

#### getInitialBundleUrl

```typescript
getInitialBundleUrl(): string | undefined
```

Gets the initial JS Bundle URL. This method is used by SourceCodeTurboModule to generate the correct stack trace. Some applications separate their bundles to improve startup performance. That is why the word "initial" is used.

#### getAssetsDest

```typescript
getAssetsDest(): string
```

Gets the assets path.

#### postMessageToCpp

```typescript
postMessageToCpp(name: string, payload: any)
```

Sends messages from ArkTS to C++. It is handled by `ArkTSMessageHub:Observer` or `ArkTSMessageHandler`.

##### Parameter

| Parameter | Type  | Required| Description      |
| ------- | ------ | ---- | ---------- |
| name    | string | Yes  | Message name.  |
| payload | any    | Yes  | Sent data.|

#### setAttachedLabel

```typescript
setAttachedLabel(attachedLabel: string): void
```

Attaches a developer-defined string label to the current RNInstance.
Subsequent errors reported via reportRNOHError from this instance will automatically include this label (to quickly correlate a specific bundle/scene), making it easier to locate the source in logs and subscriber callbacks.

##### Parameter

| Parameter | Type  | Required| Description      |
| ------- | ------ | ---- | ---------- |
| attachedLabel | string | Yes  | The custom label to bind to this instance; values longer than 256 characters emit a warning and are truncated. |

#### getAttachedLabel

```typescript
getAttachedLabel(): string
```

Returns the custom label previously set on this RNInstance via setAttachedLabel method.

## RNOHCoreContext

### Introduction

`RNOHCoreContext` provides dependencies and utilities that can be shared across `RNInstances`s. It also includes specific methods for creating and destroying an `RNInstance`. This section describes the API types of `RNOHCoreContext`.

### Key Parameters

| Parameter                | Type                   | Description                                                        |
| ---------------------- | ----------------------- | ------------------------------------------------------------ |
| uiAbilityContext       | common.UIAbilityContext | Context of `UIAbility`.                                         |
| safeAreaInsetsProvider | SafeAreaInsetsProvider  | SafeAreaInsets provider. SafeAreaInsets provides an area from the edge of the screen, which can be used to avoid content display below the status bar, notch, or waterfall edge.|
| isDebugModeEnabled     | boolean                 | Whether React Native uses the debug mode.                               |
| launchUri              | string \| undefined     | ArkUI::Want::uri, application URI.                             |

### Key Methods

#### createAndRegisterRNInstance

```typescript
createAndRegisterRNInstance: (options: RNInstanceOptions, frameNodeFactory?: FrameNodeFactory) => Promise<RNInstance>
```

Creates an RNInstance and registers it with the internal registry of RNOH.

##### Parameter

| Parameter          | Type             | Required| Description                       |
| ---------------- | ----------------- | ---- | --------------------------- |
| options          | RNInstanceOptions | Yes  | `RNInstance` options.     |
| frameNodeFactory | FrameNodeFactory  | Yes  | Inner class instantiated by `RNSurface`.|

#### destroyAndUnregisterRNInstance

```typescript
destroyAndUnregisterRNInstance: (rnInstance: RNInstance) => void
```

Destroys `RNInstance` and unregisters it from the internal registry of RNOH.

##### Parameter

| Parameter    | Type      | Required| Description                |
| ---------- | ---------- | ---- | -------------------- |
| rnInstance | RNInstance | Yes  | `RNInstance` to be destroyed.|

#### getDisplayMetrics

```typescript
getDisplayMetrics: () => DisplayMetrics
```

Gets the current display information.

#### getUIAbilityState

```typescript
getUIAbilityState: () => UIAbilityState
```

Gets the current state of the application.

#### dispatchBackPress

```typescript
dispatchBackPress: () => void
```

onBackPress of the root ArkUI component is used. Currently, this is the only response mode of BackPress.

## RNComponentContext

`RNComponentContext` defines the context used by React Native for OpenHarmony to construct components. It is a child class of `RNOHContext`.

### Constructor

| Parameter | Type    | Required| Description    |
| ------- | -------- | ---- | -------- |
| rnohContext | RNOHContext | Yes| Context used by the component or TurboModule.|
| wrappedCustomRNComponentBuilder | WrappedCustomRNComponentBuilder | Yes| Builder for a custom component on the ArkTS side.|
| wrappedRNComponentBuilder | WrappedRNComponentBuilder | Yes| Builder for an RN subcomponent on the ArkTS side.|
| rnComponentDataSourceFactoriesByDescriptorType | Map<string, RNComponentDataSourceFactory> | Yes| DataSource object for storing LazyForEach.|

## JSBundleProvider

### Introduction

JS bundle provider, which is used to initialize bundle information and obtain bundle content. This section describes the API types of `RNOHCoreContext`.

### Types

#### AnyJSBundleProvider

Loads all bundles, in ascending order.

##### Parameter

| Parameter           | Type              | Required| Description                                                |
| ----------------- | ------------------ | ---- | ---------------------------------------------------- |
| jsBundleProviders | JSBundleProvider[] | Yes  | JSBundleProvider array. Multiple JS Bundle Providers can be passed in.|

#### MetroJSBundleProvider

Loads a bundle by using Metro.

#### FileJSBundleProvider

Loads a bundle from the sandbox directory.

##### Parameter

| Parameter | Type    | Required| Description    |
| ------- | -------- | ---- | -------- |
| path    | string   | Yes  | File path.|
| appKeys | string[] | No  | Name array registered by using `AppRegistry.registerComponent` on the JS side.|

#### ResourceJSBundleProvider

Loads a bundle from a resource file, that is, the bundle in **resources/rawfile**.

| Parameter         | Type                                                        | Required| Description        |
| --------------- | ------------------------------------------------------------ | ---- | ------------ |
| resourceManager | [resmgr.ResourceManager](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/resourcemanager-V5) | Yes  | Resource manager object.|
| path            | string                                                       | Yes  | Bundle file name.|
| appKeys         | string[]                                                     | No  | Name registered by using `AppRegistry.registerComponent` on the JS side.    |

#### 

## RNInstancesCoordinator

### Introduction

`RNAbility` aims to move most of the code to the RNOH side to improve stability. However, this class of projects does not use `RNAbility` and `RNApp`. To address this issue, React Native for OpenHarmony provides `RNInstancesCoordinator` for you to flexibly control the startup of RN.

### Methods

#### create

```TypeScript
static create(dependencies: RNInstancesCoordinatorDependencies, options?: RNInstancesCoordinatorOptions)
```

Creates an `RNInstancesCoordinator` object based on the specified `RNInstancesCoordinatorDependencies` and `RNInstancesCoordinatorOptions`.

##### Parameter

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| dependencies | [RNInstancesCoordinatorDependencies](#rninstancescoordinatordependencies) | Yes| Dependencies used for building RNInstancesCoordinator.|
| options | [RNInstancesCoordinatorOptions](#rninstancescoordinatoroptions) | No| Options used for building RNInstancesCoordinator.|

#### onDestroy

```typescript
rnInstancesCoordinator?.onDestroy(): void
```

Destroys a UIAbility instance. The [onDestroy()](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityondestroy) callback can be used to release system resources and save data.

#### getBuildMode

```typescript
rnInstancesCoordinator?.getBuildMode(): BuildMode
```
Gets the build mode, which can be **RELEASE** or **DEBUG**.


#### getRNOHCoreContext


```typescript
rnInstancesCoordinator?.getRNOHCoreContext(): RNOHCoreContext
```
Gets the RNOHCoreContext of the RNInstancesCoordinator instance.

#### onForeground

```typescript
rnInstancesCoordinator?.onForeground(): void
```

Switches the UIAbility instance from the background to the foreground. When [onForeground](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonforeground) is called, React is notified of the change.

#### onBackground

```typescript
rnInstancesCoordinator?.onBackground(): void
```
Switches the UIAbility instance from the foreground to the background. When [onBackground](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-app-ability-uiability-V5#uiabilityonbackground) is called, React is notified of the change.

#### onWindowStageChange

```typescript
rnInstancesCoordinator?.onWindowStageChange(windowStageEvent: window.WindowStageEventType): void
```
Notifies React of the WindowStage lifecycle change. [windowStageEvent](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-window-V5#onwindowstageevent9) needs to be listened on.

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| windowStageEvent | [WindowStageEventType](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5#windowstageeventtype9) | Yes| WindowStage type.|

#### onWindowSizeChange

```typescript
rnInstancesCoordinator?.onWindowSizeChange(windowSize: window.Size): void
```

Notifies React of the window size change. [windowSizeChange](https://developer.huawei.com/consumer/en/doc/harmonyos-references-V5/js-apis-window-V5#onwindowsizechange7) needs to be listened on.

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| windowSize | [Size](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-window-V5#size7) | Yes| Changed window size.|

#### onConfigurationUpdate

```typescript
rnInstancesCoordinator?.onConfigurationUpdate(systemConfiguration: AbilityConfiguration.Configuration): void
```

Notifies the configuration update to React Native for OpenHarmony.

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| systemConfiguration | [Configuration](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-application-configuration-V5) | Yes| Notifies the configuration update to React Native for OpenHarmony.|

#### onMemoryLevel

```typescript
rnInstancesCoordinator?.onMemoryLevel(memoryLevel: number): void
```
Adjusts the memory level. When this method is called, React Native is notified of the change.
| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| memoryLevel | [AbilityConstant.MemoryLevel](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-abilityconstant-V5#abilityconstantmemorylevel) | Yes| Notifies React Native of the memory level change.|

#### onNewWant

```typescript
rnInstancesCoordinator?.onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void
```
Starts a new UIAbility instance after it has been started and running in the foreground and switched to the background.

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| want | [Want](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-want-V5) | Yes| Want information, such as the ability name and bundle name.| 
| launchParam | [AbilityConstant.LaunchParam](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-app-ability-abilityconstant-V5#abilityconstantlaunchparam) | Yes| Reason for the UIAbility startup and the last abnormal exit.|


#### RNInstancesCoordinatorDependencies

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| fontSizeScale | number \| undefined | Yes| Font size scaling ratio.|
| logger | RNOHLogger | Yes| Log printing.|
| uiAbilityContext | [UIAbilityContext](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V5/js-apis-inner-application-uiabilitycontext-V5) | Yes | Context required for integrating with compatible frameworks such as React Native. |
| rnohWorkerScriptUrl | string | No| Attribute to enable the worker thread of RNInstance with the corresponding name. Some TurboModules can be enabled only after the worker thread is started.|
| defaultBackPressHandler | Function | Yes| Native implementation of `exitApp` on the React side.|

#### RNInstancesCoordinatorOptions

| Parameter| Type| Required| Description|
| --- | --- | --- | --- |
| launchURI | string | No| Initial URL obtained by `Linking.getInitialURL()`.|
| onGetPackagerClientConfig | (buildMode: BuildMode) => JSPackagerClientConfig \| undefined | No| Function with Metro configuration.| 
| defaultHttpClient | [HttpClient](https://developer.huawei.com/consumer/en/doc/system-References/network-httpclient-0000001073658008)| No| Used to reuse cookies or print logs in RN and native applications.|
| disableCleaningRNInstances | boolean | No| Whether to enable the CleanUP thread to clear RN instances.| 

#### setupRNOHWorker

##### Basic Introduction

`TurboModule` can be configured to run on `Worker` threads. This configuration is implemented by calling the `setupRNOHWorker` method in the `RRNOHWorker.ets` file. For detailed operation steps, please refer to the section "Configuring Custom TurboModules to Run on `Worker` Threads" in the `TurboModule.md` document.

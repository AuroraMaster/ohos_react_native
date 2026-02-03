# 使用类FAQ

### 加载沙盒中的bundle时RN界面图片显示问题
 
- 现象
 
  鸿蒙项目中加载本地 rawfile 中的 bundle 时 **RN** 界面Image组件可以正常显示本地图片，从沙盒中加载 bundle 时 **RN** 界面 Image 组件图片显示不出来。
 
- 原因
 
  对于从沙盒中加载图片，放资源文件，需要按照 bundles 本身文件的解压位置来确定。
 
- 解决
 
  读取沙箱中的图片资源请参考[文档](../常见开发场景.md#如何加载沙箱路径bundle和图片)。

  对于从沙盒中加载图片，放资源文件，需要按照 bundles 本身文件的解压位置来确定，可参考以下：
- 打包场景
  - 打包路径
    
    假设为打包路径为根路径：/，**RN**代码路径：/aaa/bbb/c.tsx，
 
    图片1所在路径：/aaa/bbb/d.png **RN**使用写法：require('./d.png')，
 
    图片2所在路径：/eee/f.png **RN**使用写法：require('../../eee/f.png')，
 
    图片3所在路径：/aaa/ggg/h.png **RN**使用写法：require('../ggg/h.png')。
 
  - 图片路径前缀
 
    图片1生成的路径前缀：/aaa/bbb/
 
    图片2生成的路径前缀：/eee/
    
    图片3生成的路径前缀：/aaa/ggg/
 
  - 资源打包结果
 
    assets 是在 `package.json` 里面指定的目录,开发者可以在 assets 下任意设置目录。
    
    参考事例，用户可以自定义 aaa, bbb , ggg , eee 等子目录：
 
    assets/aaa/bbb/d.png ，assets/aaa/ggg/h.png ，assets/eee/f.png。
 
- 沙箱场景图片路径
 
  在沙箱场景下假设 bundle 所在路径 `/data/storage/base/files/dir1/bundle.harmony.js`。
 
  - 图片1沙箱场景实际路径： `/data/storage/base/files/dir1/aaa/bbb/d.png`。
  - 图片2沙箱场景实际路径： `/data/storage/base/files/dir1/eee/f.png`。
  - 图片3沙箱场景实际路径： `/data/storage/base/files/dir1/aaa/ggg/h.png`。
 
  **重点**：沙箱场景下图片的路径是根据 bundle 路径作为前缀，图片与工程打包路径的相对路径为后缀，合成完整的路径。这里没有 assets 目录。
 
  **路径示例：**
 
  ```CMAKE
  <Image source={{ uri: 'file:///data/storage/el2/base/files/10.png' }}
            style={{ width: 200, height: 200 }} />
  ```
 
- RAWFILE场景图片路径
 
  **RAWFILE** 场景下假设 bundle 所在路径 `resource://RAWFILE/dir1/bundle.harmony.js`。
 
  - 图片1rawfile场景实际路径： `resource://RAWFILE/assets/aaa/bbb/d.png`。
  - 图片2rawfile场景实际路径： `resource://RAWFILE/assets/eee/f.png`。
  - 图片3rawfile场景实际路径： `resource://RAWFILE/assets/aaa/ggg/h.png`。
  **重点**：**RAWFILE** 场景下图片路径的前缀指定为 `resource://RAWFILE/assets/` ，图片与工程打包路径的相对路径为后缀，合成完整的路径。这里有 assets 目录。

### modal弹框，进行路由跳转后modal 仍然显示在最上层

- 现象
	
    modal 弹框，进行路由跳转后 modal 仍然显示在最上层。

- 原因

    这个是规格；鸿蒙的 modal 对应的是 Arkui 的 Dialog ，当前 Dialog 的 **UI** 规范是在 window 最上层，所以才会有这个问题。

- 解决

    有以下两种规避方案：
    - 跳转时主动关闭 modal。
    - 不使用 modal，通过view实现 modal 的 ui 效果。

- 参考

    鸿蒙[规格文档](https://gitee.com/openharmony/docs/blob/master/zh-cn/release-notes/changelogs/OpenHarmony_5.0.0.17/changelogs-arkui.md#clarkui2--dialog%E5%9C%A8%E9%A1%B5%E9%9D%A2%E8%B7%AF%E7%94%B1%E8%B7%B3%E8%BD%AC%E6%97%B6%E5%85%B3%E9%97%AD%E8%A1%8C%E4%B8%BA%E5%8F%98%E6%9B%B4)。
   
### FlashList 列表右侧滚动条如何隐藏

- 现象
	
    想隐藏 FlashList 列表右侧滚动条。

- 原因

    无。

- 解决

    - 隐藏垂直的滚动条

        showsVerticalScrollIndicator={false}//隐藏垂直滚动条

    - 隐藏水平滚动条

        showsHorizontalScrollIndicator={false}//隐藏水平滚动条


### Animated动画组件设置useNativeDriver为false卡顿问题

- 现象

   使用 RNAnimated 动画组件设置 useNativeDriver 为`false`时，会出现卡顿现象（因 useNativeDriver 不支持 Animated 中 `translateX` 属性，无法设置 useNativeDriver 为`true`），也会导致一些其他性能问题。
   
   使用 `react-native-page-view` 库时，在 `onPageScroll` 配置 useNativeDriver 的方法也会有此问题。

- 原因

    非 bug ,使用方式不当导致。

- 解决

   用 `transform` 代替 `top` ，即用 `transform` 的 `scaleX` 和 `scaleY` 对宽度和高度进行变换。

   （1） 原问题代码示例：

    ```typescript 
    <Animated.View
        style={{
          width: 300,
          height: 204,
          position: 'absolute',
          top:macTop,
          left:macTop,
        }}>
        <Image ref="image" style={{ width: 375, height: 242 }}
          source={require('./keli.png')}>
        </Image>
      </Animated.View>
    ```

   （2） 解决方案代码示例：

    ```typescript 
    // 初始化动画值
    const scaleX = useRef(new Animated.Value(1)).current;
      const scaleY = useRef(new Animated.Value(1)).current;

      return (
         <View style={styles.container}>
            <Animated.View
              style={[
                styles.box,
                {
                  transform: [
                  { scaleX: scaleX }, // 变换宽度
                  { scaleY: scaleY }, // 变换高度
                  ],
                },
              ]}
            />
            <Button title="Start Animation" onPress={startScaleAnimation} />
         </View>
      );
    ```

### 5.0.0.500 版本显示字体变小的问题

- 现象

  - 现象1：同一个 bundle 包，在自定义的 `UIAbility` 场景下，可能存在显示字体明显变小。
  - 现象2：使用了 RNAbility 的项目，在使用 Metro 服务加载 **RN** 页面时字体变小；加载本地 bundle 时，字体显示正常。

- 原因

    这个版本 **RNOH** 的 `RNInstancesCoordinator` 新增了 `fontSizeScale` 参数，对应原生中的 `fontSizeScale` ，表示字体大小缩放比例，取值范围：0~3.2，默认值为 1。

    最近遇到的两个关于 **RN** 页面字体变小的案例，分析原因后发现都是因为加载 bundle 太早或者说创建 RNInstancesCoordinator 太晚导致的。CPP 侧拿到 `fontScale` 值是通过 RNInstancesCoordinator 的 `fontSizeScale` 传递的，如果RNInstancesCoordinator 创建过晚，提前加载了 **RN** 页面，CPP 侧渲染时的 `fontScale` 将为 0，而正常大小值是 1，所以导致了显示明显变小。

    以上两种场景具体原因分别是：

      - 现象 1 原因：使用自定义的 `UIAbility` 时，在创建 RNInstancesCoordinator 并打开 **RN** 页面的时候，没有提前初始化 RNInstancesCoordinator 导致 `fontSizeScale` 未成功传递给 CPP 侧；
      - 现象 2 原因：代码中 `onWindowStageCreate` 执行了预加载 Metro bundle 的操作，加载 Metro 的时候，**RNOH** 的 CPP 侧还未拿到 RNInstancesCoordinator 传递的 `fontScale` 默认值，所以出现了 Metro 服务加载的 **RN** 页面字体变小。

- 解决

  - 现象1解决方案：在 `Ability.onWindowStageCreate()` 生命周期里提交初始化 `RNInstancesCoordinator`。
  - 现象2解决方案：将预加载 Metro bundle 的代码延迟执行。
    
### 当一个页面的TABHost嵌套一个RN页面时，水平滑动会很容易触发RN页面中的点击事件

- 现象

    当一个页面的TABHost嵌套一个 **RN** 页面时，水平滑动会高概率触发 **RN** 页面中的点击事件。

- 原因

    水平滑动造成原生层面手势冲突，需要在原生滑动时调用对应方法取消点击事件。

- 解决

    在原生滑动时调用`this.rnohCoreContext?.cancelTouches()`方法，具体代码如下：

    ```javascript
    private rnContext:RNOHCoreContext|undefined = AppStorage.get('RNOHCoreContext'); //RN-500版本
      .onGestureSwipe((index: number, event: TabsAnimationEvent) => {
      if(this.rnContext) {
      this.rnContext.cancelTouches();
      }
      })
    ```

### hermes 编译教程

- 现象

    如果业务场景有诉求，需要自定义编译hermes，可参考下面步骤。

- 原因

    无。

- 解决

    > 如非必要，不推荐自行编译hermes。

    - 资源准备

      - 编译脚本: 可以通过[RN鸿蒙化仓库](https://gitee.com/openharmony-sig/ohos_react_native/blob/0.72.5-ohos-5.0-release/react-native-harmony/scripts/build-hermes.sh)获取；

      - hermes源码及相关依赖：编译需要[hermes源码](https://github.com/facebook/hermes/tree/hermes-2023-08-07-RNv0.72.4-813b2def12bc9df02654b3e3653ae4a68d0572e0)和JSI源码（可通过解压rnoh的源码包获得，如：react_native_openharmony-5.0.0.601.har）；

      - SDK：直接使用IDE自带的SDK；

      - 操作系统：MacOS 或 Linux。

    - 编译流程

      1. 解压har包：找一个空目录（比如temp），将har包复制到改目录，然后解压，解压后将看到一个 `package` 文件夹；
      2. 下载hermes源码：通过[hermes源码](https://github.com/facebook/hermes/tree/hermes-2023-08-07-RNv0.72.4-813b2def12bc9df02654b3e3653ae4a68d0572e0)的链接下载hermes源码，然后替换掉 `package/src/main/cpp/third-party/hermes` 文件夹；
      3. 下载编译脚本：通过上述链接下载编译脚本，然后将其拷贝到 `package/src/main/cpp/third-party/scripts` 路径下（没有scripts目录就自己建）；
      4. 配置 SDK 环境变量：将 IDE 中 `sdk/default/openharmony` 的路径配置在系统的环境变量中，环境变量名为 `OHOS_SDK`，或者也可以将这个路径写在编译脚本中；
      5. 根据下方代码：调整编译脚本：
          ```diff
          #!/bin/bash

          + OHOS_SDK=你的SDK路径 # 若做了第3步，这里可以忽略
          ...
          - THIRD_PARTY_DIR=$SCRIPT_DIR/../harmony/cpp/third-party
          + THIRD_PARTY_DIR=../
          ...
          - OHOS_SDK_NATIVE_DIR=$OHOS_SDK/10/native
          + OHOS_SDK_NATIVE_DIR=$OHOS_SDK/native
          ...
          - JSI_DIR=$THIRD_PARTY_DIR/rn/ReactCommon/jsi
          + JSI_DIR=jsi 的绝对路径
          ...
          - -DJSI_DIR=$THIRD_PARTY_DIR/rn/ReactCommon/jsi \
          + -DJSI_DIR=$JSI_DIR \
          ```
      6. 调整 `hermes/CMakeLists.txt`：注释掉 619 和 632 行，编译时也会有提示；
      7. 编译：在scripts目录下打开命令行工具，然后执行 `./build-hermes.sh`；
      8. 编译完成，编译好的so会覆盖掉原先 `prebuilt` 目录中的文件。

### 原生页面切换到RN页面字体偏小问题

- 现象  
采用非直板机设备(折叠屏，平板)，在开启capi架构，从原生页面切换到RN页面字体会出现偏小问题。
- 原因  
1.不使用rnability
2.从原生页面切换到RN页面不会触发onWindiwSizeChange，导致DisplayMetricsManager的displayMetrics默认的scale是1，与预期不符。
- 解决  
需要手动执行下`this.rnInstancesCoordinator.onWindowSizeChange(windowSize)`来触发displayMetrics更新。

### 使用KeyboardAvoidingView组件后页面被异常抬高的问题

- 现象

  使用KeyboardAvoidingView包裹TextInput设置一个底部弹窗，键盘弹出的时候，弹窗底部没有与键盘顶部对齐，中间间隔一段空白。

- 原因

  当TextInput聚焦，键盘弹出的时候，TextInput本身会自动避让键盘抬高组件高度。而当TextInput被KeyboardAvoidingView包裹时，不仅TextInput高度被抬高，其所在的页面会为了躲避键盘而重新计算页面高度，页面整体会被抬升一个键盘的高度。故出现了键盘顶部和弹窗底部中间有一段空白，没有对齐。

- 解决

  在KeyboardAvoidingView所在页面的原生容器中，设置键盘安全区域属性 `.expandSafeArea([SafeAreaType.KEYBOARD])` ，如下示例：

  ```typescript
  build() {
    Stack() {
      if (this.shouldShow) {
        RNSurface({
          ctx: this.ctx,
          surfaceConfig: {
            initialProps: this.initialProps ?? {},
            appKey: this.appKey,
          } as SurfaceConfig2,
        })
      }
      if (this.rnohCoreContext!.isDebugModeEnabled) {
        RNDevLoadingView({ useSafeAreaInsets: true, ctx: this.rnohCoreContext }).position({ x: 0, y: 0 })
      }
    }
    .expandSafeArea([SafeAreaType.KEYBOARD])
    .width("100%")
    .height("100%")
  }
  ```

- 参考

  鸿蒙[规格文档](https://developer.huawei.com/consumer/cn/doc/best-practices-V5/bpta-keyboard-layout-adapt-V5)。      

### 加载多个实例后，仅进行一次手势侧滑返回，页面却执行了多次返回动作的问题

- 现象

  加载多个实例后，仅进行一次手势侧滑返回，页面却执行了多次返回动作。

- 原因

  1. 在原生端的@Entry页面中，`onBackPress`方法拦截返回动作，定义如下：
      ```typescript
        onBackPress(): boolean | undefined {
          if (this.rnohCoreContext) {
            this.rnohCoreContext!.dispatchBackPress()
          }
          return true
        }
      ```
  2. 创建实例时，自定义返回拦截处理方法`backPressHandler`如下：
      ```typescript
        const rnInstance: RNInstance = await this.rnohCoreContext.createAndRegisterRNInstance({
          createRNPackages: createRNPackages,
          ...
          backPressHandler: () => {
            router.back()
          }
        }
      ```
  3. 假设开发者在首页中依次打开两个页面，每个页面都通过各自的实例`rnInstance`进行加载，当在第二个页面中侧滑返回时，应该返回到第一个页面，然而实际却执行了两次返回动作，直接回到了首页。

- 解决

  有以下两种办法可以选择：
  - 在`backPressHandler`方法中根据实际情况增加判断逻辑，比如通过路由获取当前页面的路径名称，与创建当前页面实例时所定义的名称对比，如果两者一致，则执行页面返回逻辑，反之则不作任何处理。
  - 去除实例中的`backPressHandler`拦截方法，且跳过`onBackPress`中定义的`dispatchBackPress`方法，直接执行返回逻辑，比如下面示例的方法：
    ```typescript 
      onBackPress(): boolean | undefined {
        if (this.rnohCoreContext) {
          router.back()
        }
        return true
      }
    ``` 

### 原生组件嵌套RNSurface，滚动时容易触发RN页面的点击事件

- 现象

    原生Scroll组件嵌套RNSurface时，在滚动原生组件时极易触发RN的点击事件。

- 原因

    触摸滑动原生Scroll时，RN监听到了触摸事件从而触发了点击事件。

- 解决

    在滚动事件中调用`this.rnohCoreContext?.cancelTouches()`主动阻止RN的触摸事件。
    ```typescript
    Scroll() {
      RNApp({
        rnInstanceConfig: {
          createRNPackages,
          enableNDKTextMeasuring: true,
          enableBackgroundExecutor: true,
          enableCAPIArchitecture: true,
          enablePartialSyncOfDescriptorRegistryInCAPI: true,
        },
        initialProps: { "foo": "bar" } as Record<string, string>,
        appKey: "app_name",
        wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
        onSetUp: (rnInstance) => {
          rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP")
        },
        jsBundleProvider: new TraceJSBundleProviderDecorator(
          new AnyJSBundleProvider([
            new MetroJSBundleProvider(),
            new FileJSBundleProvider('/data/storage/el2/base/files/bundle.harmony.js'),
            new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'hermes_bundle.hbc'),
            new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, 'bundle.harmony.js')
          ]),
          this.rnohCoreContext.logger),
      })
    }.height(2000).backgroundColor("red").onScroll(() => {
      this.rnohCoreContext?.cancelTouches()
    })
    ```

### RN中StatusBar作用域问题

- 现象

    由原生页面进入RN页面后设置StatusBar的样式，再退到原生页面，原生页面的StatusBar样式没有还原，变成在RN页面中设置的样式了。

- 原因

    ArkUI中设置StatusBar对整个窗口生效，不是对页面生效。

- 解决

    由RN页面退到原生页面后重新设置StatusBar的样式。

### release包相关问题

- 现象

  - 滚动条滚动缓慢、闪烁问题。
  - 两个scrollView联动滚动的时候会有卡顿、延迟。
  - RN中动画卡顿。

- 原因

  debug包性能有一定的瓶颈。

- 方案

  用release版本的RN包编译release版本的包，性能会好很多。

### 如何提高编译效率

- 现象
    
  用 release 包的时候，由于没有 x86_64 架构，就编译不到 Windows 的模拟器上；用 Windows 或者 intel 芯片的 mac的同事，都需要用源码的版本编译到模拟器上，如果有c++的改动，一次编译都得在30分钟左右。

- 原因

  主要是c++编译较慢

- 解决

一、创建rnoh静态模块
  - [release包使用](../环境搭建.md)  
  1. ⽤DevEco打开鸿蒙⼯程，右键->New->Module->Static Library,填写模块名。
  2. 解压华为公司提供的react_native_openharmony-x.x.x.xxx.har。
  3. 将解压后的src/main/cpp⽂件夹、ets.ets⽂件、ts.ts⽂件复制粘贴到新模块。
  4. ⽤解压后的src/main/ets⽂件夹替换掉新模块的src/main/ets⽂件夹。
  5. ⽤解压后的index.ets⽂件替换掉新模块的index.ets⽂件。
  6. 新模块的build-profile.json5如下：
    ```json5
    {
        "apiType": "stageMode",
        "targets": [
            {
                "name": "default",
                "runtimeOS": "HarmonyOS",
            }
        ]
    }
    ```
  7. 新模块的oh-package.json5如下：
    ```json5
    {
        license: 'ISC',
        types: '',
        devDependencies: {},
        name: '@rnoh/react-native-openharmony',
        description: 'React Native for OpenHarmony',
        ohos: {
            org: '',
        },
        type: 'module',
        version: '', # 版本号到解压后的oh-package.json5⽂件中查看
        dependencies: {},
        main: 'index.ets',
    }
    ```
  8. 新模块的src/main/module.json5内容如下：
    ```json5
    {
        "module": {
            "name": "react_native_openharmony",
            "type": "har",
            "deviceTypes": [
                "default"
            ],
            "requestPermissions": [
                {"name": "ohos.permission.INTERNET"},
                {"name": "ohos.permission.VIBRATE"}
            ]
        }
    }
    ```
  9. 删除新模块的混淆配置⽂件consumer-rules.txt和obfuscation-rules.txt。

二、编译rnoh release

  1. 解压华为公司提供的react_native_openharmony_release-x.x.x.xxx.har。
  2. 将解压后的src/main/include⽂件夹复制粘贴到新模块。
  3. 修改新模块的src/main/cpp/CMakeLists.txt。
  4. 修改新模块的build-profile.json5。
  
  ```json
    {
        "apiType": "stageMode",
      + "buildOption": {
        + "externalNativeOptions": {
        + "path": "./src/main/cpp/CMakeLists.txt",
        + "arguments": "",
        + "cppFlags": ""
        + }
      + },
        "targets": [
            {
                "name": "default",
                "runtimeOS": "HarmonyOS",
            }
        ]
    }
  ```
  5. 切换成release模式构建 entry旁边左边有个按钮，product，build Mode选择release。
  6. 编译，选中新建的模块（rnoh）-> Build -> Make Module xxx，等待编译完成，编译好后的⽂件位于`build/default/outputs/default`, ⼤⼩约35M。

三、使⽤release包

使⽤release版本的har包需要使⽤release版本的[CMakeLists.txt⽂件](../../Zips/MyApplicationReplace/entry/src/main/cpp/CMakeLists%20-%20release.txt)，将该⽂件内容复制粘贴到⾃⼰鸿蒙⼯程的CMakeLists.txt中，并做对应的调整。  

### Keyboard下的监听事件未响应的问题

- 现象

    Keyboard下的监听事件未响应。

- 原因

    RNOH只监听了最上层子窗口的键盘事件，如果遇到上述情况，可能原因就是当前应用中RN并不是在最上层子窗口中展示。

- 解决

    根据上面的分析，这个问题可以有2种解决方案：
    1. 调整RN显示的窗口，让其显示在 `lastWindow`；
    2. 修改RNOH的源码，让RNOH监听对应窗口（如：`MainWindow`）上的键盘事件。  
  
    <br>RNOH中，键盘事件监听的代码位于：  
    `oh_modules/@rnoh/react-native-openharmony/src/main/ets/RNOHCorePackage/turboModules/KeyboardObserverTurboModule.ts`  
    具体功能实现可以参考[窗口](https://developer.huawei.com/consumer/cn/doc/harmonyos-references-V13/js-apis-window-V13#windowgetlastwindow9)。

### Linking.canOpenURL()一直返回false的问题

- 现象

  使用Linking.canOpenURL()拉起其他应用时一直返回false

- 原因

  指定应用的scheme需要在module.json5文件中配置，否则查询不到

- 解决

  在`module.json5`文件内的[querySchemes](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V13/canopenlink-V13#%E6%93%8D%E4%BD%9C%E6%AD%A5%E9%AA%A4)字段配上想要拉起的应用的scheme

  例如：想要拉起浏览器访问网页，则需要配http或https
  ```json5
    "querySchemes": ["http", "https"],
  ```
  ### RNApp、RNSurface作为子组件嵌入ArkTs原生Scroll组件时，滑动会触发子组件的点击事件

- 现象

  在用户滚动过程中，当触碰到React Native组件时，滚动事件和点击事件会同时触发，导致滚动过程中意外触发了子组件的点击。

- 原因

  在ArkTs原生Scroll组件（或者类似有滚动功能的ArkTs原生组件）的滚动事件里，未对 React Native 框架的点击事件透传进行取消操作。

- 方案

  当遇到类似问题时，可以在原生滚动组件上设置 `onScroll` 事件。在该事件的处理逻辑中，调被封装的 React Native 组件 `rnInstance` 或者 `rnohCoreContext` 的 `cancelTouches` 方法，以阻止滑动过程中 React Native 内部触摸的触发。

- 示例代码  

```typescript
    Scroll() {
        RNApp({...})
    }
    .onScroll(() => {
        this.rnInstance!.cancelTouches();
    })  
``` 

### 开启语音播报后为什么空白区域也能选中 

- 现象
	
    开启语音播报功能后开白位置也能被选中。

- 原因

    主要原因是因为 RNOH 中所有的 CustomNode（View 组件对应的 ArkUI 节点） 和 StackNode （RootView 组件对应的 ArkUI 节点） 都默认监听了 onClick 事件，因此系统便会认为该节点可以点击，开启语音播报后该位置自然也能响应点击。

- 解决

    最佳的解决方案应该是将 RNOH 中默认的 onClick 事件去掉。但考虑到有三方库或者某些伙伴的自定义组件可能会用到这个onClick事件，因此从 0.72.95 版本开始我们增加了一个编译选项 `ALL_CONTAINERS_CLICKABLE`，在模块的 `CMakeLists.txt` 文件中添加一行 `set(ALL_CONTAINERS_CLICKABLE OFF)` 即可取消掉默认的 onClick 监听。

    > 需要注意的是，当将 `ALL_CONTAINERS_CLICKABLE` 设置为 `OFF` 后，RNOH 框架不再默认监听 onClick 事件，因此设置之前需要检查所用到的三方库或者自定义组件是否依赖 onClick （对应 CAPI 属性为 NODE_ON_CLICK） 事件，以免影响其他业务。如果发现有组件依赖到 onClick 事件，则需要自行注册事件监听器来处理 onClick 事件（可参考 ArkUI 的文档 [addnodeeventreceiver](https://developer.huawei.com/consumer/cn/doc/harmonyos-references/capi-arkui-nativemodule-arkui-nativenodeapi-1#addnodeeventreceiver)）。

### 使用 RN StatusBar 在 HarmonyOS 上可能出现状态不符合预期的原因说明

* 现象

  在 HarmonyOS 平台上使用 React Native 的 `StatusBar` 相关接口时，部分场景下可能出现状态栏样式与当前页面预期不一致的情况，例如：

  * 页面切换后，状态栏样式未如预期发生变化
  * 返回某个页面时，状态栏样式未恢复为该页面期望的状态
  * RN 页面与原生页面切换后，状态栏表现不符合页面自身配置

  上述现象通常表现为：**状态栏未严格随页面切换而变化**。

* 原因

  该问题的根本原因在于 HarmonyOS 与 React Native 对 StatusBar 的能力模型不同。

  **平台能力模型差异：**

  * 在 Android / iOS 平台中，状态栏能力与页面级容器（Activity / ViewController）生命周期强绑定
    页面创建与销毁天然对应状态栏样式的生效与回退
  * 在 HarmonyOS 中，StatusBar 属于**系统级 UI 能力**
    状态栏配置由系统统一管理，不与单个页面生命周期绑定

  在此基础上，以下两类场景更容易暴露差异：

  1. **系统行为介入的场景**

     在应用运行过程中，系统可能因应用状态变化或系统 UI 行为对状态栏进行调整。
     此类调整并不一定伴随 RN 页面重新渲染或重新设置状态栏，因此可能导致当前页面的状态栏样式未按预期生效。

  2. **页面切换相关场景**

     在页面跳转或页面栈变化过程中，不同页面可能对状态栏有不同期望。
     由于 HarmonyOS 上状态栏不具备页面级隔离能力，页面切换并不等价于状态栏状态的自动切换或回滚，从而可能出现样式残留或覆盖的情况。

* 说明

  上述现象并非 RNOH 框架或 RN `StatusBar` 接口的实现缺陷，而是由 HarmonyOS 平台系统能力边界所决定。

  在 HarmonyOS 上，**页面生命周期与状态栏状态并非一一对应关系**，因此无法保证状态栏行为始终与页面切换严格同步。

* 使用建议

  * HarmonyOS 场景下，**优先使用系统默认状态栏策略**
  * 避免在多个页面中频繁或动态控制 `StatusBar`
  * 若业务确有特殊需求（如沉浸式页面），建议：

    * 将 `StatusBar` 的使用范围限制在封闭页面内
    * 明确页面进入与退出时的状态栏预期
    * 不依赖页面卸载自动恢复状态栏样式

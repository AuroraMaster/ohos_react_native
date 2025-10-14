# Usage FAQs

### Improper Display of the RN Image When the Bundle in the Sandbox is Loaded
 
- Symptom
 
    The local image on the RN page is properly displayed when a bundle in the local rawfile is loaded in the OpenHarmony project. While the image is not properly displayed when the bundle is loaded from sandbox.
 
- Cause
 
    Loading images from the sandbox and storing resource files are not based on the file decompression location of bundles.
 
- Solution
 
    For details about how to read image resources in the sandbox, see [Document](../common-development-scenarios.md#how-do-i-load-a-bundle-or-image-from-the-sandbox-directory?).
 
    Loading images from the sandbox and storing resource files based on the file decompression location of bundles.
- Packing
  - Packing path
    
    Assume that the packing path is the root path: /; RN format: /aaa/bbb/c.tsx
 
    Path of image 1: /aaa/bbb/d.png; RN format: require('./d.png')
 
    Path of image 2: /eee/f.png; RN format: require('../../eee/f.png')
 
    Path of image 3: /aaa/ggg/h.png; RN format: require('../ggg/h.png')
 
  - Image path prefix
 
    Path prefix of image 1: /aaa/bbb/
 
    Path prefix of image 2: /eee/
    
    Path prefix of image 3: /aaa/ggg/
 
  - Resource packing result
 
    You can set any directory under **assets**, which is the directory specified in `package.json`.
    
    A user can customize subdirectories such as aaa, bbb, ggg, eee as the following:
 
    assets/aaa/bbb/d.png, assets/aaa/ggg/h.png, assets/eee/f.png.
 
- Sandbox image path
 
  In the sandbox scenario, assume that the bundle path is `/data/storage/base/files/dir1/bundle.harmony.js`.
 
  - Actual path of image 1: `/data/storage/base/files/dir1/aaa/bbb/d.png`
  - Actual path of image 2: `/data/storage/base/files/dir1/eee/f.png`
  - Actual path of image 3: `/data/storage/base/files/dir1/aaa/ggg/h.png`
 
  Note: In the sandbox scenario, the bundle path is used as the prefix of the image path, and the relative path of the image and project packing path is used as the suffix to form a complete path. The **assets** directory does not exist.
 
  Path example:
 
  ```CMAKE
  <Image source={{ uri: 'file:///data/storage/el2/base/files/10.png' }}
            style={{ width: 200, height: 200 }} />
  ```
 
- Image path in RAWFILE
 
  In the RAWFILE scenario, assume that the bundle path is `resource://RAWFILE/dir1/bundle.harmony.js`.
 
  - Actual path of image 1: `resource://RAWFILE/assets/aaa/bbb/d.png`
  - Actual path of image 2: `resource://RAWFILE/assets/eee/f.png`
  - Actual path of image 3: `resource://RAWFILE/assets/aaa/ggg/h.png`
  Note: In the RAWFILE scenario, the prefix of the image path is specified as `resource://RAWFILE/assets/`, and the relative path of the image and project packing path is the suffix to form a complete path. The **assets** directory exists.

### Modal at the Top Layer After Route Redirection

- Symptom
	
    The modal is still displayed at the top layer after route redirection.

- Cause

    According to different specifications, the modal of OpenHarmony is implemented through ArkUI, while the UI dialog is at the top layer of the window.

- Solutions

    There are two solutions:
    - Disable modal automatically during redirection.
    - Use view instead of modal to implement the UI effect of modal.

- Reference

    OpenHarmony [specifications](https://gitee.com/openharmony/docs/blob/master/en/release-notes/changelogs/OpenHarmony_5.0.0.17/changelogs-arkui.md#clarkui2--dialog%E5%9C%A8%E9%A1%B5%E9%9D%A2%E8%B7%AF%E7%94%B1%E8%B7%B3%E8%BD%AC%E6%97%B6%E5%85%B3%E9%97%AD%E8%A1%8C%E4%B8%BA%E5%8F%98%E6%9B%B4)
   
### Hiding the Scrollbar on the Right of FlashList

- Symptom
	
    There is no idea to hide the scrollbar on the right of FlashList.

- Cause

    None.

- Solutions

    - Hide the vertical scrollbar.

        showsVerticalScrollIndicator={false}// Hide the vertical scroll bar.

    - Hide the horizontal scrollbar.

        showsHorizontalScrollIndicator={false}// Hide the horizontal scrollbar.


### Frame Freezing Caused When useNativeDriver is Set to false for the Animated Component

- Symptom

   When the RN **Animated** component is used to set **useNativeDriver** to `false`, frame freezing occurs (**useNativeDriver** does not support the `translateX` attribute in **Animated** and cannot be set to `true`.), causing other performance problems.
   
   When the `react-native-page-view` library is used, such problem also occurs when the **useNativeDriver** method is configured in `onPageScroll`.

- Cause

    Improper use instead of a bug.

- Solutions

   Replace `top` with `transform`. That is, use `scaleX` and `scaleY` of `transform` to convert the width and height.

   (1) Example of the original code:

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

   (2) Example of a correct code:

    ```typescript 
    // Initialize the animated value.
    const scaleX = useRef(new Animated.Value(1)).current;
      const scaleY = useRef(new Animated.Value(1)).current;

      return (
         <View style={styles.container}>
            <Animated.View
              style={[
                styles.box,
                {
                  transform: [
                  { scaleX: scaleX }, // Convert the width.
                  { scaleX: scaleX }, // Convert the height.
                  ],
                },
              ]}
            />
            <Button title="Start Animation" onPress={startScaleAnimation} />
         </View>
      );
    ```

### Small Font Size in Version 5.0.0.500

- Symptom

  - Symptom 1: In the custom `UIAbility` scenario of the same bundle, the font size may be obviously smaller.
  - Symptom 2: For a project that uses RNAbility, the font size becomes small when Metro is used to load the RN page. When the local bundle is loaded, the font size is normal.

- Causes

    The `fontSizeScale` parameter is added to `RNInstancesCoordinator` of RNOH, which corresponds to the native `fontSizeScale`, indicating the font size scale ratio. The value ranges from 0 to 3.2. The default value is **1**.

    In recent two cases, the reason why the font size on the RN page becomes small is that a bundle is loaded too early or RNInstancesCoordinator is created too late. The `fontScale` value obtained by CPP is transferred through the `fontSizeScale` of RNInstancesCoordinator. If the RNInstancesCoordinator is created too late, the RN page will be loaded in advance, and the `fontScale` value will be **0** during rendering on CPP, which is smaller compared with the normal value **1**, therefore, the display becomes small.

    The causes for the preceding two scenarios are as follows:

      - Cause of symptom 1: When the custom `UIAbility` is used, RNInstancesCoordinator is not initialized in advance when it is created and the RN page is opened. As a result, `fontSizeScale` fails to be transferred to the CPP.
      - Cause of symptom 2: In the code, `onWindowStageCreate` pre-loads the Metro bundle. When the Metro is loaded, the CPP side of RNOH has not obtained the default value of `fontScale` transferred by RNInstancesCoordinator. As a result, the font size of the RN page loaded by Metro becomes small.

- Solutions

  - Solution to symptom 1: Submit the initialized `RNInstancesCoordinator` in the `Ability.onWindowStageCreate()` lifecycle.
  - Solution to symptom 2: Delay the execution of the code for pre-loading the Metro bundle.
    
### Touch Event Triggered by Horizontal Scrolling When TABHost is Nested With a RN Page

- Symptom

    when TABHost is nested with an RN page, the touch event on the RN page may be triggered when a user scrolls the page horizontally.

- Cause

    Horizontal scrolling causes gesture conflicts at the native layer. The corresponding method needs to be called to cancel the touch event during native scrolling.

- Solution

    Call the `this.rnohCoreContext?.cancelTouches()` method during native scrolling. The code is as follows:

    ```javascript
    private rnContext:RNOHCoreContext|undefined = AppStorage.get('RNOHCoreContext'); // RN-500 version
      .onGestureSwipe((index: number, event: TabsAnimationEvent) => {
      if(this.rnContext) {
      this.rnContext.cancelTouches();
      }
      })
    ```

# Runtime FAQs

## Crash Error, Indicating That RNOH_C_API_ARCH Is Not Set
- Viewing the Crash Log 
The application is compiled and installed on the phone, but it crashes once it is run. View the crash log in **DevEco Studio** > **Log** > **FaultLog**.
- Error screenshot 
![faq-RNOH_C_API_ARCH](../figures/faq-RNOH_C_API_ARCH.png)
- Solutions
1. This error is caused by the **C API** version. You need to set `RNOH_C_API_ARCH=1` in the environment variable, restart DevEco Studio, choose **Build** > **Clean Project**, and recompile the code.
2. If the preceding settings do not take effect, delete the `.cxx` directory and recompile and run the code.
3. If the problem still persists, set the following parameter in `CMakeLists.txt`:

    ```CMAKE
    set(RNOH_C_API_ARCH, 1)
    ```
4. For details about how to analyze other crash problems, see [Troubleshooting Guidelines for CppCrash](https://developer.huawei.com/consumer/en/doc/best-practices-V5/bpta-cppcrash-guidance-V5).

### "insertChild" Error for Crash in the Hybrid Solution
- Error screenshot 
![faq-CustomRNComponentFrameNodeFactory](../figures/faq-CustomRNComponentFrameNodeFactory.png)
- Crash 
When the hybrid solution is used, if a crash occurs and the `ComponentInstance::insertChild` call stack exists, check whether `CustomRNComponentFrameNodeFactory` is correctly created when `RNInstance` is created. Create the factory method correctly or upgrade the **RNOH** version to solve the problem.

### "Cannot read property tag of undefined" Error for Crash in the Hybrid Solution
- Error screenshot 
    ![faq-arkTSComponentNames](../figures/faq-arkTSComponentNames.png)
- Crash
When the hybrid solution is used, if an unexpected exit occurs and `CustomRNComponentFrameNodeFactory` is displayed at the bottom of the call stack, check whether the component name is placed in the array and transferred as the `arkTsComponentNames` parameter when the `RNApp` or `RNInstance` is created.

### "Couldn't create bindings between ETS and CPP. libRNOHApp is undefined." Error After Running  
 
- Symptom
    
    An RN application crashes at runtime. `Error message: Couldn't create bindings between ETS and CPP. libRNOHApp is undefined` is displayed in **FaultLog**.
 
- Causes
 
    The `libRNOHApp` library corresponds to the `librnoh_app.so` dynamic library, and is a dynamic library product compiled by C++. The possible causes are as follows:
        
    - Cause 1: `librnoh_app.so` does not exist.
    - Cause 2: If `librnoh_app.so` exists, locate other causes.
 
- Solutions
 
    Check whether `librnoh_app.so` exists. You can check whether the corresponding .so file exists in `\entry\build\default\intermediates\libs\default\arm64-v8a\` or `libs/arm64-v8a` under the root directory of the OpenHarmony project.
 
    - Solution to cause 1: Edit the configuration file `entry/build-profile.json5` and add the `externalNativeOptions` option under **buildOptions**.
 
        ```json5
        "buildOption": {
            "externalNativeOptions": { // Native compilation options
                "path": "./src/main/cpp/CMakeLists.txt",  // CMakeLists.txt at the native layer
                "arguments": "",
                "cppFlags": "",            
            }                
        }
        ```
    - Solution to cause 2:
        
        (1) Check the Hilog to locate the fault. One possible cause is that `libhermes.so` is not packed into ***XXX*.hap**. (The Hilog reports `can't find library libhermes.so in namespace: moduleNs_default`.)
        
        (2) Alternatively, enable the error enhancement function of IDE, as shown in the following figure.
            ![faq-IDE-enabling-error-enhancement](../figures/faq-IDE-enable-error-enhancement.png)

        (3) There is an incompatible change in IDE upgrade. For details, see [Document](https://developer.huawei.com/consumer/en/doc/harmonyos-releases-V5/ide-changelogs-db5-V5). Simply speaking, there are two HARs, and the .so file of HAR A depends on the .so file of HAR B. Before change, the .so file of HAR B is packed into HAR A.
 
        (4) Solution: This problem is automatically solved when IDE is upgraded to the latest version.
 
        Add the following information to `/build-profile.json5` of the HAR module:
        ```json5
        (type=har in modules.json5)
            "nativeLib": { // Same level as apiType
            "excludeFromHar": false
            }    
        ```
        For details about the parameters, see [Document](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V5/ide-hvigor-build-profile-V5).
 
        To solve the problem, you can also copy the content in the `oh_modules/@rnoh/react-native-openharmony/src/main/cpp/third-party/prebuilt` directory of the RN source code to the `oh_modules/@rnoh/react-native-openharmony/libs` directory.

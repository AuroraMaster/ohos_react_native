# React Native for OpenHarmony Release Notes
The current React Native for OpenHarmony version is adapted based on the community RN 0.72.5. The release notes are as follows.
| Name                         | Version Number                           |
| ----------------------------- | -------------------------------|
| react-native-harmony        | 0.72.38 |
| react-native-harmony-cli    | 0.0.27 |
| rnoh                          | 5.0.0.715 |
| rnoh_release                  | 5.0.0.715 |

Matching IDE, SDK, and mobile phone ROM
| Name                         | Version Number                           |
| ----------------------------- | -------------------------------|
| DevEco Studio     | DevEco Studio 5.0.3.906 |
| HarmonyOS SDK     | HarmonyOS SDK 5.0.0.71 (SP1)|
| Mobile phone ROM          | ALN-AL00 205.0.0.102 (SP2C00E73R4P17)<br> ALN-AL80 205.0.0.102 (SP2C00E73R4P17)<br> BRA-AL00 205.0.0.102 (SP2C00E73R4P17)|

## New Features

### Adaptive RNSurface

For RNOH 5.0.0.715 or later, the adaptive RNSurface feature is added.
The height of the adaptive RNSurface can be determined by the content (the width still fills the available area as much as possible).

### Unbinding Font Registration From RNInstance Creation

For RNOH 5.0.0.715 or later, font registration is unbound from RNInstance creation.
The font registration API **registerFont** is added to RNInstance, so that new fonts can be registered at any time.

### Reading Sandbox Files During Font Registration

For RNOH 5.0.0.715 or later, both fontResourceByFontFamily and registerFont can read font files from the sandbox path.

### Independent Multi-Instance Font Sets

For RNOH 5.0.0.715 or later, different RNInstances do not share the font set.

### TurboModule Running in a Child Thread

For RNOH 5.0.0.715 or later, TurboModule can run in a child thread.
For details about this feature, see [Performance Optimization](../performance-optimization.md).
For details about how to set this feature, see [TurboModule](../TurboModule.md).

### Exception Capture in a Bundle

For RNOH 5.0.0.715 or later, APIs for capturing running exceptions and errors in a bundle are added.

### RNSurface Supporting RootTag

For RNOH 5.0.0.715 or later, RNSurface can obtain the corresponding RNInstance and RootTag through APIs.

### Codegen

For RNOH 5.0.0.715 or later, the Codegen code can be generated in the library of the configuration file.

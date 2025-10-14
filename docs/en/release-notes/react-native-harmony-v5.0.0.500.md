# React Native for OpenHarmony Release Notes
The current React Native for OpenHarmony version is adapted based on the community RN 0.72.5. The release notes are as follows:
| Name                         | Version Number                           |
| ----------------------------- | -------------------------------|
| react-native-harmony          | 0.72.29                     |
| react-native-harmony-cli                          | 0.0.26                          |
| rnoh                | 5.0.0.500                |

Matching IDE, SDK, and mobile phone ROM:
| Name                         | Version Number                           |
| ----------------------------- | -------------------------------|
| DevEco Studio          | DevEco Studio 5.0.3.706                     |
| HarmonyOS SDK          | HarmonyOS SDK 5.0.0.61 (SP1)                         |
| Mobile phone ROM               | ALN-AL00 205.0.0.61 (SP1C00E63R4P9) <br> ALN-AL80 205.0.0.61 (SP1C00E63R4P9)  <br> BRA-AL00 205.0.0.61 (SP1C00E63R4P9)|

After DevEco Studio is updated to 5.0.3.700 or a later version, the **libhermes.so** file may not be found. For details, see the related section in [FAQs : Cannot find libhermes.so](../../zh-cn/FAQ.md).

## Incompatible Changes

### Custom Font Configuration

After the system is updated to RNOH 5.0.0.500 or a later version, the API for configuring the custom font is changed.

#### Impact

After the system is updated to RNOH 5.0.0.500 or a later version, a historical project configured with a custom font fails to be compiled.

#### Adaptation Guide

The original font directory is `src\main\resources\rawfile\assets\assets\fonts\`.
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

After the system is updated to RNOH 5.0.0.500 or a later version, the API name is changed from `fontOptions` to `fontResourceByFontFamily`, and the passed argument is changed from an array to a map with `familyName` as the key and `Resource` as the value. The original font directory is deprecated.
Assume that the new font directory is `src\main\resources\rawfile\fonts\`:
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

## Specification Changes

### Priority of the Theme Font and fontFamily Attribute

After the system is updated to RNOH 5.0.0.500 or a later version, the priority of the theme font and `fontFamily` attribute is changed.

#### Impact

If the `fontFamily` attribute of the Text component is set to a font different from the theme font, the font that takes effect is different from the earlier setting.
In earlier versions, the `fontFamily` attribute takes effect preferentially. The theme font is used only when the `fontFamily` attribute is not set or is set to `undefined`.
After the system is updated to RNOH 5.0.0.500 or a later version, the theme font takes effect preferentially and overrides the `fontFamily` attribute.

### halfLeading Feature

After the system is updated to RNOH 5.0.0.500 or a later version, the `halfLeading` feature is changed.

#### Impact

When `half_leading` is not configured in metadata or is set to `false`, the text center position of the Text component with `lineHeight` is changed.
In earlier versions, the `lineHeight` attribute does not affect the `halfLeading` feature.
After the system is updated to RNOH 5.0.0.500 or a later version, the `halfLeading` feature is forcibly enabled when the `lineHeight` attribute of the Text component is set.

For more changes, see [ChangeLog](https://gitee.com/openharmony-sig/ohos_react_native/blob/0.72.5-ohos-5.0-release/CHANGELOG.md).

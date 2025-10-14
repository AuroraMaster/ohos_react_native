# Description
This is a demo project for creating custom **Fabric** components (including **CAPI** and **ArkTS** components).


# Directory Structure

```md
FabricComponentSample
├── ReactProject Front-end project
├── fabric-component-sample-package RN third-party component (custom Fabric component)
├── NativeProject OpenHarmony project
├── react-native-harmony RN OpenHarmony bundle
├── react-native-harmony-cli RN OpenHarmony scaffolding
└── README.md
```


# Environment Setup
1. Save `rnoh-react-native-harmony-xx.tgz` to the `react-native-harmony` folder and modify the `react-native-harmony` dependency path of `ReactProject/package.json`.
2. Save `rnoh-react-native-harmony-cli-xx.tgz` to the `react-native-harmony-cli` folder.
3. Run **npm pack** in `fabric-component-sample-package` to generate a .tgz file.
4. Save `react_native_openharmony-xxx.har` to the `NativeProject/libs` folder and modify the dependency paths of `NativeProject/oh-package.json5` and `NativeProject/entry/oh-package.json5`.
5. Open `NativeProject` in DevEco Studio and run **Sync and Refresh Project**.
6. Run **npm i** in `ReactProject` to install the dependency, run **npm run dev** to generate the bundle file, and run **npm start** to start Metro.
7. Go back to DevEco Studio and click File > Project Structure > Signing Configs, log in and complete the signing.
8. Click **run** in the upper right corner to start the project.
9. If Metro is started, run `hdc rport tcp:8081 tcp:8081` to forward port 8081, and then restart the application on the mobile phone.

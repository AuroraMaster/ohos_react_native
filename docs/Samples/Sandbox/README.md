### README

This project includes two demos. One demonstrates how to push raw files to the sandbox and load the files on OpenHarmony. The other demonstrates how to load images in the sandbox on the React Native side.

#### Directory Structure

```
├── SandBoxNativeProject
|   └── Index.ets
|       ├── downloadBundle # Push raw files to the sandbox on OpenHarmony.
|       └── new FileJSBundleProvider # Load the files in the sandbox on OpenHarmony.
└── SandBoxRnProject
    └── example.tsx  # Load images in the sandbox on the React Native side.

```

#### On OpenHarmony

1. **Push files to the sandbox**: Use the **getRawFileContent** method of the `downloadBundle` function to push raw files to the sandbox for subsequent read operations.
2. **Load files**: Use the `new FileJSBundleProvider` class to load the files from the sandbox and verify the file content.

#### On React Native

1. **Load images from the sandbox**: Use the code in `example.tsx` to load images from the OpenHarmony sandbox and display them in the React Native app.

#### Procedure

1. In the `SandBoxRnProject` directory, run **npm i @react-native-oh/react-native-harmony@x.x.x** to install the dependency and run **npm run dev** to generate the bundle.
2. In the `entry` directory, run **ohpm i @rnoh/react-native-openharmony@x.x.x** to install the dependency.
3. Check whether the `oh-modules` folder is generated in the `SandBoxNativeProject` and `entry` directories.
4. Open `SandBoxNativeProject` in DevEco Studio and run **Sync and Refresh Project**.
5. Click File > Project Structure > Signing Configs, log in and complete the signing.
6. Click **run** in the upper right corner to start the project.
7. Ensure that the dependency downloaded on the Harmony and RN sides is complete and the version mapping is normal.
8. Open the generated app and check whether the file is correctly loaded.

#### Notes

- Ensure that the OpenHarmony and React Native projects are correctly configured so that files can be seamlessly transferred and loaded between the two systems.
- The sandbox path usage and file access permission need to be adjusted based on the actual environment.

---

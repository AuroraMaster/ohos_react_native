# Description

This is a demo project that uses FrameNode to preload bundles.

# Directory Structure

GenerateBundle -- RN project 
FrameNodeSampleApp -- Native project

# Environment Setup

1. In the `GenerateBundle` directory, run **npm i @react-native-oh/react-native-harmony@x.x.x** to install the dependency and run **npm run dev** to generate the bundle.
2. In the `entry` directory, run **ohpm i @rnoh/react-native-openharmony@x.x.x** to install the dependency.
3. Check whether the `oh-modules` folder is generated in the `FrameNodeSampleApp` and `entry` directories.
4. Open `FrameNodeSampleApp` in DevEco Studio and run **Sync and Refresh Project**.
5. Click File > Project Structure > Signing Configs, log in and complete the signing.
6. Click **run** in the upper right corner to start the project.

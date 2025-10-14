# Description
This is a demo project with multiple JSBundles and RNInstances.


# Directory Structure

```md
MutilBundleSample
├── FlightRN Flight front-end project
├── HotelRN Hotel front-end project
├── NativeProject OpenHarmony project
└── README.md
```


# Environment Setup
1. Run **npm i @react-native-oh/react-native-harmony@x.x.x** in `FlightRN` to install the dependency and run **npm run dev:all** to generate the JS bundle of the flight.
2. Run **npm i @react-native-oh/react-native-harmony@x.x.x** in `HotelRN` to install the dependency and run **npm run dev:all** to generate the JS bundle of the hotel.
3. In the `entry` directory, run **ohpm i @rnoh/react-native-openharmony@x.x.x** to install the dependency.
4. Check whether the `oh-modules` folder is generated in the `NativeProject` and `entry` directories.
5. Open `NativeProject` in DevEco Studio and run **Sync and Refresh Project**.
6. Click File > Project Structure > Signing Configs, log in and complete the signing.
7. Click **run** in the upper right corner to start the project.

project(rnapp)
cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(RNOH_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(OH_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules")

set(RNOH_CPP_DIR "${OH_MODULES}/@rnoh/react-native-openharmony/src/main/cpp")
set(CMAKE_ASM_FLAGS "-Wno-error=unused-command-line-argument -Qunused-arguments")
set(CMAKE_CXX_FLAGS "-fstack-protector-strong -Wl,-z,relro,-z,now,-z,noexecstack -s -fPIE -pie")
add_compile_definitions(WITH_HITRACE_SYSTRACE)
set(WITH_HITRACE_SYSTRACE 1) # for other CMakeLists.txt files to use

add_subdirectory("${RNOH_CPP_DIR}" ./rn)

# RNOH_BEGIN: add_package_subdirectories
add_subdirectory("${OH_MODULES}/@react-native-oh-tpl/react-native-video/src/main/cpp" ./video)
add_subdirectory("${OH_MODULES}/@react-native-oh-tpl/react-native-gesture-handler/src/main/cpp" ./gesture-handler)
add_subdirectory("${OH_MODULES}/@react-native-oh-tpl/react-native-safe-area-context/src/main/cpp" ./safe-area)
add_subdirectory("${OH_MODULES}/@react-native-oh-tpl/flash-list/src/main/cpp" ./flash-list)
# RNOH_END: add_package_subdirectories

file(GLOB GENERATED_CPP_FILES "./generated/*.cpp")
add_library(rnoh_app SHARED
    ${GENERATED_CPP_FILES}
    "./PackageProvider.cpp"
    "${RNOH_CPP_DIR}/RNOHAppNapiBridge.cpp"
)

target_link_libraries(rnoh_app PUBLIC rnoh)

# RNOH_BEGIN: link_packages
#target_link_libraries(rnoh_app PUBLIC rnoh_sample_package)
target_link_libraries(rnoh_app PUBLIC rnoh_video)
target_link_libraries(rnoh_app PUBLIC rnoh_gesture_handler)
target_link_libraries(rnoh_app PUBLIC rnoh_safe_area)
target_link_libraries(rnoh_app PUBLIC rnoh_flash_list)
# RNOH_END: link_packages

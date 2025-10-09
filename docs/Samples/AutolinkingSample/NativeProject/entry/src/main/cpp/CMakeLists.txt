# Copyright (c) 2025 Huawei Technologies Co., Ltd.
#
# This source code is licensed under the MIT license found in the
# LICENSE-MIT file in the root directory of this source tree.

project(rnapp)
cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(RNOH_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(OH_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../oh_modules")
set(RNOH_CPP_DIR "${OH_MODULES_DIR}/@rnoh/react-native-openharmony/src/main/cpp")
file(REAL_PATH "${RNOH_CPP_DIR}" RNOH_CPP_DIR)
set(RNOH_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")
set(LOG_VERBOSITY_LEVEL 1)
set(CMAKE_ASM_FLAGS "-Wno-error=unused-command-line-argument -Qunused-arguments")
set(CMAKE_CXX_FLAGS "-fstack-protector-strong -Wl,-z,relro,-z,now,-z,noexecstack -fPIE -pie")
set(WITH_HITRACE_SYSTRACE 1) # for other CMakeLists.txt files to use
add_compile_definitions(WITH_HITRACE_SYSTRACE)

include("${CMAKE_CURRENT_SOURCE_DIR}/autolinking.cmake")
add_subdirectory("${RNOH_CPP_DIR}" ./rn)

# RNOH_BEGIN: manual_package_linking_1
# RNOH_END: manual_package_linking_1

file(GLOB GENERATED_CPP_FILES "./generated/*.cpp")

add_library(rnoh_app SHARED
    ${GENERATED_CPP_FILES}
    "./PackageProvider.cpp"
    "${RNOH_CPP_DIR}/RNOHAppNapiBridge.cpp"
)

target_link_libraries(rnoh_app PUBLIC rnoh)

autolink_libraries(rnoh_app)

# RNOH_BEGIN: manual_package_linking_2
# RNOH_END: manual_package_linking_2

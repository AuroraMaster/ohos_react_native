# Copyright (c) 2024 Huawei Technologies Co., Ltd.
#
# This source code is licensed under the MIT license found in the
# LICENSE-MIT file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

# There are two codegen modes: app build time, library publish time.
# Generated code in this directory is generated at the app build time...
set(rnoh_sample_package_build_time_generated_dir "${RNOH_GENERATED_DIR}/react_native_harmony_sample_package")

# ...and in this directory contains a code generated at the library publish time, and it should be shipped with library.
set(rnoh_sample_package_lib_generated_dir "./generated")

# Then, all cpp files needs to be found...
file(GLOB_RECURSE rnoh_sample_package_build_time_generated_SRC "${rnoh_sample_package_build_time_generated_dir}/**/*.cpp")
file(GLOB_RECURSE rnoh_sample_package_lib_generated_SRC "${rnoh_sample_package_lib_generated_dir}/**/*.cpp")
file(GLOB rnoh_sample_package_SRC CONFIGURE_DEPENDS *.cpp)

# ...and added to the library. Otherwise, "undefined reference to *" errors will occur.
add_library(rnoh_sample_package SHARED ${rnoh_sample_package_SRC} ${rnoh_sample_package_build_time_generated_SRC} ${rnoh_sample_package_lib_generated_SRC})

# The library needs to know where to look for header files to make #includes work.
target_include_directories(rnoh_sample_package PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${rnoh_sample_package_build_time_generated_dir} ${rnoh_sample_package_lib_generated_dir})

# The library depends on the rnoh library.
target_link_libraries(rnoh_sample_package PUBLIC rnoh)

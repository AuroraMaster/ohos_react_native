# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.
cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

add_compile_options(
    -fexceptions
    -frtti
    -std=c++17
    -Wall
    -Wpedantic
    -Wno-gnu-zero-variadic-macro-arguments
    -DLOG_TAG=\"Fabric\")
# RNOH patch begin
file(GLOB react_render_uimanager_SRC CONFIGURE_DEPENDS
    *.cpp
    ${REACT_COMMON_DIR}/react/renderer/uimanager/*.cpp)
list(REMOVE_ITEM react_render_uimanager_SRC
    ${REACT_COMMON_DIR}/react/renderer/uimanager/UIManagerBinding.cpp
    ${REACT_COMMON_DIR}/react/renderer/uimanager/UIManager.cpp)
# RNOH patch end
add_library(react_render_uimanager SHARED ${react_render_uimanager_SRC})
# RNOH patch begin
target_include_directories(react_render_uimanager
    PUBLIC .
    ${REACT_COMMON_DIR}/react/renderer/uimanager)
# RNOH patch end
target_link_libraries(react_render_uimanager
    glog
    folly_runtime
    jsi
    react_debug
    react_render_componentregistry
    react_render_core
    react_render_debug
    react_render_graphics
    react_render_leakchecker
    react_render_runtimescheduler
    react_render_mounting
    react_config
    rrc_root
    rrc_view
    runtimeexecutor
    # RNOH patch begin
    ffrt::ffrt_cpp
    libffrt.z.so
    # RNOH patch end
)
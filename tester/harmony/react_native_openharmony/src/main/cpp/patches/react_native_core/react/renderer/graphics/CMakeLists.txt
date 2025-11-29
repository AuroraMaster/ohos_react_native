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
file(GLOB react_render_graphics_SRC CONFIGURE_DEPENDS
    *.cpp
    ${REACT_COMMON_DIR}/react/renderer/graphics/*.cpp)
list(REMOVE_ITEM react_render_graphics_SRC
    ${REACT_COMMON_DIR}/react/renderer/graphics/Transform.cpp)
# RNOH patch end

add_library(react_render_graphics SHARED ${react_render_graphics_SRC})

# RNOH patch begin
target_include_directories(react_render_graphics
    PUBLIC .
    ${REACT_COMMON_DIR}/react/renderer/graphics)
# RNOH patch end

target_include_directories(react_render_graphics
        PUBLIC
          ${REACT_COMMON_DIR}
          ${REACT_COMMON_DIR}/react/renderer/graphics/platform/cxx/ # RNOH: patch
        )

target_link_libraries(react_render_graphics glog fb fbjni folly_runtime react_debug)


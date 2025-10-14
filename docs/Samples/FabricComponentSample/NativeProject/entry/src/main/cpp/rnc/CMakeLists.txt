# Copyright (c) 2024 Huawei Technologies Co., Ltd.
#
# This source code is licensed under the MIT license found in the
# LICENSE-MIT file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)

set(fabric_component_sample_package "${RNOH_GENERATED_DIR}/fabric_component_sample_package")
file(GLOB_RECURSE fabric_component_sample_generated_SRC "${fabric_component_sample_package}/**/*.cpp")

file(GLOB fabric_component_sample_SRC CONFIGURE_DEPENDS *.cpp)

add_library(fabric_component SHARED ${fabric_component_sample_SRC} ${fabric_component_sample_generated_SRC})
target_include_directories(fabric_component PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${fabric_component_sample_package})
target_link_libraries(fabric_component PUBLIC rnoh)

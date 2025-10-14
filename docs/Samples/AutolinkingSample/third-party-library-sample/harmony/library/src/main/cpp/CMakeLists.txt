# Copyright (c) 2025 Huawei Technologies Co., Ltd.
#
# This source code is licensed under the MIT license found in the
# LICENSE-MIT file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)

set(third_party_library_sample_generated_dir "${CMAKE_CURRENT_SOURCE_DIR}/generated")
file(GLOB_RECURSE third_party_library_sample_generated_SRC "${third_party_library_sample_generated_dir}/**/*.cpp")

file(GLOB third_party_library_sample_SRC CONFIGURE_DEPENDS *.cpp)

add_library(third_party_library SHARED ${third_party_library_sample_SRC} ${third_party_library_sample_generated_SRC})
target_include_directories(third_party_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${third_party_library_sample_generated_dir})
target_link_libraries(third_party_library PUBLIC rnoh)

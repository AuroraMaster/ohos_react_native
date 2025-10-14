# Copyright (c) 2024 Huawei Technologies Co., Ltd.
#
# This source code is licensed under the MIT license found in the
# LICENSE-MIT file in the root directory of this source tree.
set(CMAKE_CXX_STANDARD 17)

# HERMES
add_library(hermes-engine::libhermes SHARED IMPORTED)
add_compile_options(
    # TODO: check if building in debug mode
    -DHERMES_ENABLE_DEBUGGER
)
set(hermes_include_dirs
    "${third_party_dir}/hermes/API"
    "${third_party_dir}/hermes/public"
)
set_property(TARGET hermes-engine::libhermes PROPERTY
                IMPORTED_LOCATION "${third_party_dir}/prebuilt/${OHOS_ARCH}/libhermes.so")

# HERMES INSPECTOR
add_subdirectory("${REACT_COMMON_DIR}/hermes/inspector" ./hermes_inspector)
target_include_directories(hermes_inspector PRIVATE
    "${folly_include_dir}"
    "${boost_include_dirs}"
    "${hermes_include_dirs}"
    "${libevent_include_dirs}"
)
target_compile_options(hermes_inspector PRIVATE ${folly_compile_options})

# HERMES EXECUTOR COMMON
add_subdirectory(${REACT_COMMON_DIR}/hermes/executor ./hermes_executor_common)
target_include_directories(hermes_executor_common PUBLIC
    "${folly_include_dir}"
    "${boost_include_dirs}"
    "${hermes_include_dirs}"
)

# ----------------------------------------------------------------------------------------------------------------------

add_subdirectory("${RNOH_CPP_DIR}/RNOH/react-native-jsvm" ./jsvm)

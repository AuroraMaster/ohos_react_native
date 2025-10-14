# Copyright (c) 2024 Huawei Technologies Co., Ltd.
#
# This source code is licensed under the MIT license found in the
# LICENSE-MIT file in the root directory of this source tree.

set(REACT_NATIVE_JSVM_DIR "${RNOH_CPP_DIR}/RNOH/react-native-jsvm")
set(JSVM_EXECUTOR_INCLUDE_DIR "${REACT_NATIVE_JSVM_DIR}/include")
set(JSVM_EXECUTOR_SRC
    "${REACT_NATIVE_JSVM_DIR}/src/JSVMRuntime.cpp"
    "${REACT_NATIVE_JSVM_DIR}/src/JSVMConverter.cpp"
    "${REACT_NATIVE_JSVM_DIR}/src/hostProxy.cpp"
    "${REACT_NATIVE_JSVM_DIR}/src/JSVMExecutorFactory.cpp"
)

set (CMAKE_CXX_STANDARD 17)

add_library(jsvm_executor_common
    STATIC
    ${JSVM_EXECUTOR_SRC}
)

target_include_directories(jsvm_executor_common PUBLIC
    "${RNOH_CPP_DIR}"
    "${REACT_COMMON_DIR}"
    "${REACT_COMMON_DIR}/jsiexecutor"
    "${folly_include_dir}"
    "${JSVM_EXECUTOR_INCLUDE_DIR}"
    "${RNOH_APP_DIR}"
)

target_link_libraries(jsvm_executor_common
        jsireact
        libjsvm.so
        jsi
)
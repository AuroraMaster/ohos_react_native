/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
#pragma once

namespace rnoh {

enum ScenarioID {
    SHADOW_TREE_PARALLELIZATION = 0,
    PRECREATED_COMPONENTS_PARALLELIZATION = 1,
    MUTATION_PARALLELIZATION = 2,
};

constexpr int MAX_THREAD_NUM_SHADOW_TREE = 2;
constexpr int MAX_THREAD_NUM_RECREATED_COMPONENTS = 2;
constexpr int MAX_THREAD_NUM_MUTATION_CREATE = 4;
constexpr int THREAD_PRIORITY_LEVEL_5 = 5;

} // namespace rnoh
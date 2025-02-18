// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "snippets/two_inputs_and_outputs.hpp"
#include "common_test_utils/test_constants.hpp"

namespace ov {
namespace test {
namespace snippets {
namespace {

const std::vector<std::vector<ov::Shape>> input_shapes = {
        { {5, 5, 256, 1}, {5, 5, 256, 1} },
        { {5, 5, 16, 35}, {5, 5, 16, 35} },
        { {5, 5, 256, 1}, {5, 5, 256, 35} },
        { {5, 5, 256, 1}, {5, 5, 1, 1} },

        { {5, 5, 16, 35}, {5, 5, 1, 1} },
        { {5, 5, 16, 35}, {5, 5, 16, 1} },
        { {5, 5, 5, 35}, {5, 5, 1, 35} },
        { {5, 5, 16, 1}, {5, 5, 1, 35} },

        { {5, 5, 35, 16}, {5, 5, 35, 16} },
        { {5, 5, 35, 16}, {5, 5, 1, 16} },

        { {5, 5, 35, 17}, {5, 5, 35, 17} },
        { {5, 5, 35, 17}, {5, 5, 1, 17} },

        { {5, 5, 35, 18}, {5, 5, 35, 18} },
        { {5, 5, 35, 18}, {5, 5, 1, 18} },
};

INSTANTIATE_TEST_SUITE_P(smoke_Snippets_Eltwise, TwoInputsAndOutputs,
                         ::testing::Combine(
                             ::testing::ValuesIn(input_shapes),
                             ::testing::Values(4),
                             ::testing::Values(1),
                             ::testing::Values(CommonTestUtils::DEVICE_CPU)),
                         TwoInputsAndOutputs::getTestCaseName);

}  // namespace
} // namespace snippets
} // namespace test
} // namespace ov
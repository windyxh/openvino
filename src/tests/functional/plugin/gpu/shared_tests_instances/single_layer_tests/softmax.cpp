// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <vector>

#include "single_layer_tests/softmax.hpp"
#include "common_test_utils/test_constants.hpp"

using namespace ov::test::subgraph;

namespace {

const std::vector<ov::test::ElementType> netPrecisions = {
    ov::element::f16,
    ov::element::f32,
};

const std::vector<ov::Shape> inputShapes2D = {
    {1, 100},
    {100, 1},
    {10, 10},
    {100, 10}
};

const std::vector<int64_t> axis2D = {
    -2, -1, 0, 1
};

const auto params2D = testing::Combine(
    testing::ValuesIn(netPrecisions),
    ::testing::Values(ov::element::undefined),
    ::testing::Values(ov::element::undefined),
    testing::ValuesIn(ov::test::static_shapes_to_test_representation(inputShapes2D)),
    testing::ValuesIn(axis2D),
    testing::Values(CommonTestUtils::DEVICE_GPU),
    testing::Values(ov::AnyMap())
);

INSTANTIATE_TEST_SUITE_P(
        smoke_SoftMax2D,
        SoftMax8LayerTest,
        params2D,
        SoftMax8LayerTest::getTestCaseName
);

const std::vector<ov::Shape> inputShapes3D = {
    {16, 64, 64},
};

const std::vector<int64_t> axis3D = {
    -1
};

const auto params3D = testing::Combine(
    testing::ValuesIn(netPrecisions),
    ::testing::Values(ov::element::undefined),
    ::testing::Values(ov::element::undefined),
    testing::ValuesIn(ov::test::static_shapes_to_test_representation(inputShapes3D)),
    testing::ValuesIn(axis3D),
    testing::Values(CommonTestUtils::DEVICE_GPU),
    testing::Values(ov::AnyMap())
);

INSTANTIATE_TEST_SUITE_P(
        smoke_SoftMax3D,
        SoftMax8LayerTest,
        params3D,
        SoftMax8LayerTest::getTestCaseName
);

const std::vector<ov::Shape> inputShapes4D = {
    {1, 100, 1, 1},
    {1, 3, 4, 3},
    {2, 3, 4, 5},
};

const std::vector<int64_t> axis4D = {-4, -3, -2, -1, 0, 1, 2, 3};

const auto params4D = testing::Combine(
    testing::ValuesIn(netPrecisions),
    ::testing::Values(ov::element::undefined),
    ::testing::Values(ov::element::undefined),
    testing::ValuesIn(ov::test::static_shapes_to_test_representation(inputShapes4D)),
    testing::ValuesIn(axis4D),
    testing::Values(CommonTestUtils::DEVICE_GPU),
    testing::Values(ov::AnyMap())
);

INSTANTIATE_TEST_SUITE_P(
        smoke_SoftMax4D,
        SoftMax8LayerTest,
        params4D,
        SoftMax8LayerTest::getTestCaseName
);

const std::vector<ov::Shape> inputShapes5D = {
    {1, 100, 1, 1, 1},
    {1, 3, 4, 3, 4},
    {2, 3, 4, 5, 6},
};

const std::vector<int64_t> axis5D = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4};

const auto params5D = testing::Combine(
    testing::ValuesIn(netPrecisions),
    ::testing::Values(ov::element::undefined),
    ::testing::Values(ov::element::undefined),
    testing::ValuesIn(ov::test::static_shapes_to_test_representation(inputShapes5D)),
    testing::ValuesIn(axis5D),
    testing::Values(CommonTestUtils::DEVICE_GPU),
    testing::Values(ov::AnyMap())
);

INSTANTIATE_TEST_SUITE_P(
        smoke_SoftMax5D,
        SoftMax8LayerTest,
        params5D,
        SoftMax8LayerTest::getTestCaseName
);

}  // namespace

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "single_layer_tests/strided_slice.hpp"
#include "common_test_utils/test_constants.hpp"
#include "common/myriad_common_test_utils.hpp"

#include "vpu/private_plugin_config.hpp"

#include <vector>

using namespace LayerTestsDefinitions;

namespace {

typedef std::map<std::string, std::string> Config;

std::vector<StridedSliceSpecificParams> testCases = {
    { { 128, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, { 0, 1, 1 }, { 0, 1, 1 },  { 1, 0, 0 },  { 0, 0, 0 },  { 0, 0, 0 } },
    { { 128, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1},  { 1, 0, 1 }, { 1, 0, 1 },  { 0, 1, 0 },  { 0, 0, 0 },  { 0, 0, 0 } },
    { { 1, 12, 100 }, { 0, 9, 0 }, { 0, 11, 0 }, { 1, 1, 1 }, { 1, 0, 1 }, { 1, 0, 1 },  { 0, 0, 0 },  { 0, 0, 0 },  { 0, 0, 0 } },
    { { 1, 12, 100 }, { 0, 9 }, { 0, 11 }, { 1, 1, 1 }, { 1, 0, 1 }, { 1, 0, 1 },  { 0, 0, 0 },  { 0, 0, 0 },  { 0, 0, 0 } },
    { { 1, 12, 100 }, { 0, 9 }, { 0, 11 }, { 1, 1 }, { 1, 0 }, { 1, 0 },  { 0, 0 },  { 0, 0 },  { 0, 0 } },
    { { 1, 12, 100 }, { 0, 1, 0 }, { 0, -1, 0 }, { 1, 1, 1 }, { 1, 0, 1 }, { 1, 0, 1 },  { 0, 0, 0 },  { 0, 0, 0 },  { 0, 0, 0 } },
    { { 1, 12, 100 }, { 0, 8, 0 }, { 0, 9, 0 }, { 1, 1, 1 }, { 1, 0, 1 }, { 1, 0, 1 },  {},  {},  {} },
    { { 1, 12, 100 }, { 0, 4, 0 }, { 0, 9, 0 }, { 1, 2, 1 }, { 1, 0, 1 }, { 1, 0, 1 },  { 0, 0, 0 },  { 0, 0, 0 },  { 0, 0, 0 } },
    { { 1, 12, 100 }, { 0, 0, 0 }, { 0, 11, 0 }, { 1, 2, 1 }, { 1, 0, 1 }, { 1, 0, 1 },  { 0, 0, 0 },  { 0, 0, 0 },  { 0, 0, 0 } },
    { { 1, 12, 100 }, { 0, -8, 0 }, { 0, -6, 0 }, { 1, 2, 1 }, { 1, 0, 1 }, { 1, 0, 1 },  {},  {},  {} },
    { { 1, 12, 100 }, { 0, -1, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, { 1, 0, 1 }, { 1, 0, 1 },  { 0, 0, 0 },  { 0, 1, 0 },  { 0, 0, 0 } },
    { { 1, 2, 2, 2 }, { 0, 0, 0, 0 }, { 1, 2, 2, 2 }, { 1, 1, 1, 1 }, {1, 1, 1, 1}, {1, 1, 1, 1},  {},  {},  {} },
    { { 1, 2, 2, 2 }, { 0, 1, 1, 1 }, { 1, 2, 2, 2 }, { 1, 1, 1, 1 }, {0, 0, 0, 0}, {1, 1, 1, 1},  {},  {},  {} },
    { { 1, 2, 2, 2 }, { 0, 1, 1, 1 }, { 1, 2, 2, 2 }, { 1, 1, 1, 1 }, {0, 0, 0, 0}, {0, 0, 0, 0},  {},  {},  {} },
    { { 1, 2, 4, 3 }, { 0, 0, 0, 0 }, { 1, 2, 4, 3 }, { 1, 1, 2, 1 }, {1, 1, 1, 1}, {1, 1, 1, 1},  {},  {},  {} },
    { { 1, 2, 4, 2 }, { 0, 0, 0, 1 }, { 1, 2, 4, 2 }, { 1, 1, 2, 1 }, {0, 1, 1, 0}, {1, 1, 0, 0},  {},  {},  {} },
    { { 1, 2, 4, 2 }, { 0, 0, 0, 0 }, { 1, 2, 4, 2 }, { 1, 1, 2, 1 }, {1, 1, 1, 1}, {1, 1, 1, 1},  {},  {},  {} },
    { { 1, 2, 4, 2 }, { 0, 0, 0, 0 }, { 1, 2, 4, 2 }, { 1, 1, 2, 1 }, {0, 1, 1, 1}, {1, 1, 1, 1},  {},  {},  {} },
    { { 1, 3, 4, 5, 6 }, { 0, 1, 0, 0, 0 }, { 1, 3, 4, 5, 6 }, { 1, 1, 1, 1, 1 }, {1, 0, 1, 1, 1}, {1, 0, 1, 1, 1},  {},  {},  {} },
    { { 2, 3, 4, 5, 6 }, { 0, 1, 0, 0, 0 }, { 2, 3, 4, 5, 6 }, { 1, 1, 1, 1, 1 }, {1, 0, 1, 1, 1}, {1, 0, 1, 1, 1},  {},  {0, 1, 0, 0, 0},  {} },
    { { 2, 3, 4, 5, 6 }, { 0, 1, 0, 0, 0 }, { 2, 3, 4, 5, 6 }, { 1, 1, 1, 1, 1 }, {1, 0, 1, 1, 1}, {1, 0, 1, 1, 1},  {},  {},  {0, 1, 0, 0, 0} },
    { { 10 }, { -1 }, { -11 }, { -1 }, { 0 }, { 0 }, {}, {}, {} },
    { { 10, 10 }, { -1, 0 }, { -11, 0 }, { -1, 1 }, { 0, 1 }, { 0, 1 }, {}, {}, {} },
    { { 10, 10 }, { -1, -1 }, { -11, -11 }, { -1, -1 }, { 0, 0 }, { 0, 0 }, {}, {}, {} },
    { { 1, 52, 52, 3, 2 }, { 0, 0, 0, 0, -1 }, { 0, 0, 0, 0, -2 }, { 1, 1, 1, 1, -1 }, { 1, 1, 1, 1, 0 }, { 1, 1, 1, 1, 0 }, {}, {}, {} },
    { { 1, 52, 52, 3, 2 }, { 0, 0, 0, 0, -2 }, { 0, 0, 0, 0, -9223372036854775807 }, { 1, 1, 1, 1, -1 }, { 1, 1, 1, 1, 0 }, { 1, 1, 1, 1, 0 }, {}, {}, {} },
    { { 1, 52, 52, 3, 2 }, { 0, 0, 0, 0, -1 }, { 0, 0, 0, 0, -9223372036854775807 }, { 1, 1, 1, 1, -1 }, { 1, 1, 1, 1, 0 }, { 1, 1, 1, 1, 0 }, {}, {}, {} },
};

std::vector<InferenceEngine::Precision> netPrecisions = {
        InferenceEngine::Precision::FP32,
        InferenceEngine::Precision::I32
};

Config getConfig() {
    return Config{ { InferenceEngine::MYRIAD_DETECT_NETWORK_BATCH, CONFIG_VALUE(NO) } };
}

INSTANTIATE_TEST_SUITE_P(smoke_StridedSlice_tests, StridedSliceLayerTest,
                        ::testing::Combine(
                            ::testing::ValuesIn(testCases),
                            ::testing::ValuesIn(netPrecisions),
                            ::testing::Values(InferenceEngine::Precision::UNSPECIFIED),
                            ::testing::Values(InferenceEngine::Precision::UNSPECIFIED),
                            ::testing::Values(InferenceEngine::Layout::ANY),
                            ::testing::Values(InferenceEngine::Layout::ANY),
                            ::testing::Values(CommonTestUtils::DEVICE_MYRIAD),
                            ::testing::Values(getConfig())),
                        StridedSliceLayerTest::getTestCaseName);

INSTANTIATE_TEST_SUITE_P(smoke_StridedSlice_testsU32, StridedSliceLayerTest,
                        ::testing::Combine(
                            ::testing::ValuesIn(testCases),
                            ::testing::Values(InferenceEngine::Precision::U32),
                            ::testing::Values(InferenceEngine::Precision::U32),
                            ::testing::Values(InferenceEngine::Precision::U32),
                            ::testing::Values(InferenceEngine::Layout::ANY),
                            ::testing::Values(InferenceEngine::Layout::ANY),
                            ::testing::Values(CommonTestUtils::DEVICE_MYRIAD),
                            ::testing::Values(getConfig())),
                        StridedSliceLayerTest::getTestCaseName);

INSTANTIATE_TEST_SUITE_P(smoke_StridedSlice_testsI64, StridedSliceLayerTest,
                        ::testing::Combine(
                            ::testing::ValuesIn(testCases),
                            ::testing::Values(InferenceEngine::Precision::I64),
                            ::testing::Values(InferenceEngine::Precision::I64),
                            ::testing::Values(InferenceEngine::Precision::I64),
                            ::testing::Values(InferenceEngine::Layout::ANY),
                            ::testing::Values(InferenceEngine::Layout::ANY),
                            ::testing::Values(CommonTestUtils::DEVICE_MYRIAD),
                            ::testing::Values(getConfig())),
                        StridedSliceLayerTest::getTestCaseName);

INSTANTIATE_TEST_SUITE_P(smoke_StridedSlice_testsU64, StridedSliceLayerTest,
                        ::testing::Combine(
                            ::testing::ValuesIn(testCases),
                            ::testing::Values(InferenceEngine::Precision::U64),
                            ::testing::Values(InferenceEngine::Precision::U64),
                            ::testing::Values(InferenceEngine::Precision::U64),
                            ::testing::Values(InferenceEngine::Layout::ANY),
                            ::testing::Values(InferenceEngine::Layout::ANY),
                            ::testing::Values(CommonTestUtils::DEVICE_MYRIAD),
                            ::testing::Values(getConfig())),
                        StridedSliceLayerTest::getTestCaseName);

}  // namespace

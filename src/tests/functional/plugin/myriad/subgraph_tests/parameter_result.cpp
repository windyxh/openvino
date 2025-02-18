// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "subgraph_tests/parameter_result.hpp"

using namespace SubgraphTestsDefinitions;

namespace {

INSTANTIATE_TEST_SUITE_P(smoke_Check, ParameterResultSubgraphTestLegacyApi,
                            ::testing::Combine(
                                ::testing::Values(ov::test::InputShape{{1, 3, 10, 10}, {}}),
                                ::testing::Values(CommonTestUtils::DEVICE_MYRIAD)),
                            ParameterResultSubgraphTestBase::getTestCaseName);

INSTANTIATE_TEST_SUITE_P(smoke_Check, ParameterResultSubgraphTest,
                            ::testing::Combine(
                                ::testing::Values(ov::test::InputShape{{1, 3, 10, 10}, {{1, 3, 10, 10}}}),
                                ::testing::Values(CommonTestUtils::DEVICE_MYRIAD)),
                            ParameterResultSubgraphTestBase::getTestCaseName);

}  // namespace

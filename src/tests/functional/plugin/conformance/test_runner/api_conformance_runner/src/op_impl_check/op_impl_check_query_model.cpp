// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "single_layer_tests/op_impl_check/op_impl_check_query_model.hpp"
#include "single_layer_tests/op_impl_check/single_op_graph.hpp"
#include "conformance.hpp"

namespace ov {
namespace test {
namespace conformance {
namespace op {

using namespace ov::test::subgraph;

namespace {
INSTANTIATE_TEST_SUITE_P(conformance_query_model,
                         OpImplCheckTest,
                         ::testing::Combine(
                                 ::testing::ValuesIn(createFunctions()),
                                 ::testing::Values(targetDevice),
                                 ::testing::Values(pluginConfig)),
                         OpImplCheckTest::getTestCaseName);
} // namespace

}  // namespace op
}  // namespace conformance
}  // namespace test
}  // namespace ov
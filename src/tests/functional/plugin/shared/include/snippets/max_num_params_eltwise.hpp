// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "shared_test_classes/base/snippets_test_utils.hpp"

namespace ov {
namespace test {
namespace snippets {

typedef std::tuple<
        ov::Shape,                   // Input Shape All shapes are replicated
        size_t,                      // Expected num nodes
        size_t,                      // Expected num subgraphs
        std::string                  // Target Device
> MaxNumParamsEltwiseParams;

class MaxNumParamsEltwiseSinh : public testing::WithParamInterface<ov::test::snippets::MaxNumParamsEltwiseParams>,
                   virtual public ov::test::SnippetsTestsCommon {
public:
    static std::string getTestCaseName(testing::TestParamInfo<ov::test::snippets::MaxNumParamsEltwiseParams> obj);

protected:
    void SetUp() override;
};

} // namespace snippets
} // namespace test
} // namespace ov

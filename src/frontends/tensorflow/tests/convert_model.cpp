// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "convert_model.hpp"

#include "tf_utils.hpp"

using namespace ngraph;
using namespace ov::frontend;

using TFConvertModelTest = FrontEndConvertModelTest;

static const std::vector<std::string> models{
    std::string("2in_2out/2in_2out.pb"),
    std::string("forward_edge_model/forward_edge_model.pb"),
    std::string("forward_edge_model2/forward_edge_model2.pb"),
};

INSTANTIATE_TEST_SUITE_P(TFConvertModelTest,
                         FrontEndConvertModelTest,
                         ::testing::Combine(::testing::Values(TF_FE),
                                            ::testing::Values(std::string(TEST_TENSORFLOW_MODELS_DIRNAME)),
                                            ::testing::ValuesIn(models)),
                         FrontEndConvertModelTest::getTestCaseName);

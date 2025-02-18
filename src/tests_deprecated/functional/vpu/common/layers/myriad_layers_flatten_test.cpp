// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "myriad_layers_flatten_test.hpp"

INSTANTIATE_TEST_SUITE_P(accuracy, myriadLayersTestsFlatten_smoke,
    ::testing::Combine(
        ::testing::ValuesIn(s_flattenTensors),
        ::testing::ValuesIn(s_flattenAxis)
    )
);

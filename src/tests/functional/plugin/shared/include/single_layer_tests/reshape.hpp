// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "shared_test_classes/single_layer/reshape.hpp"

namespace LayerTestsDefinitions {

TEST_P(ReshapeLayerTest, CompareWithRefsDynamicBath) {
    Run();
}

}  // namespace LayerTestsDefinitions
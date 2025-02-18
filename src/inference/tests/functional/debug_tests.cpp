// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <gtest/gtest.h>
#include "debug.h"
#include <string>

using DebugTests = ::testing::Test;

TEST_F(DebugTests, tolowerWorksWithEmptyString) {
    std::string str = "";
    ASSERT_STREQ("", InferenceEngine::details::tolower(str).c_str());
}

TEST_F(DebugTests, shouldConvertToLowerCase) {
    std::string str = "Hello, World!1";
    ASSERT_STREQ("hello, world!1", InferenceEngine::details::tolower(str).c_str());
}

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once
#include <gtest/gtest.h>
#include <ie_core.hpp>

#include "myriad_devices.hpp"
#include <behavior_test_plugin.h>
#include <mvnc.h>

using namespace InferenceEngine;
using ExeNetwork = InferenceEngine::ExecutableNetwork;

//------------------------------------------------------------------------------
// class MyriadLoadNetworkTestCase
//------------------------------------------------------------------------------

class MyriadLoadNetworkTestCase : public testing::Test,
                                        public MyriadDevicesInfo
{
protected:
    // Operations
    void SetUp() override;
    void LoadNetwork();

    bool IsDeviceAvailable(std::string device_name);

    // Data section
    InferenceEngine::CNNNetwork cnnNetwork;
    std::shared_ptr<InferenceEngine::Core> ie;
};

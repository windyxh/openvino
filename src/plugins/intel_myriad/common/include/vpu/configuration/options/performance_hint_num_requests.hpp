// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstddef>
#include <string>

#include "vpu/configuration/as_parameter_enabler.hpp"
#include "openvino/runtime/properties.hpp"

namespace vpu {

namespace details {

enum class Access;
enum class Category;

}  // namespace details

class PluginConfiguration;

struct PerformanceHintNumRequestsOption : public AsParameterEnabler {
    using value_type = int;

    static std::string key();
    static void validate(const std::string&);
    static void validate(const PluginConfiguration&);
    static std::string defaultValue();
    static value_type parse(const std::string&);
    static details::Access access();
    static details::Category category();
};

}  // namespace vpu

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <string>

#include "vpu/configuration/as_parameter_enabler.hpp"

namespace vpu {

enum class MovidiusDdrType;

namespace details {

enum class Access;
enum class Category;

}  // namespace details

class PluginConfiguration;

struct MemoryTypeOption : public AsParameterEnabler {
    using value_type = MovidiusDdrType;

    static std::string key();
    static void validate(const std::string&);
    static void validate(const PluginConfiguration&);
    static std::string defaultValue();
    static value_type parse(const std::string&);
    static details::Access access();
    static details::Category category();
};

}  // namespace vpu

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "vpu/private_plugin_config.hpp"
#include "vpu/utils/containers.hpp"
#include "vpu/configuration/options/check_preprocessing_inside_model.hpp"
#include "vpu/configuration/switch_converters.hpp"
#include "vpu/configuration/plugin_configuration.hpp"

namespace vpu {

void CheckPreprocessingInsideModelOption::validate(const std::string& value) {
    const auto& converters = string2switch();
    VPU_THROW_UNLESS(converters.count(value) != 0, R"(unexpected {} option value "{}", only {} are supported)",
        key(), value, getKeys(converters));
}

void CheckPreprocessingInsideModelOption::validate(const PluginConfiguration& configuration) {
    validate(configuration[key()]);
}

std::string CheckPreprocessingInsideModelOption::key() {
    return InferenceEngine::MYRIAD_CHECK_PREPROCESSING_INSIDE_MODEL;
}

details::Access CheckPreprocessingInsideModelOption::access() {
    return details::Access::Private;
}

details::Category CheckPreprocessingInsideModelOption::category() {
    return details::Category::CompileTime;
}

std::string CheckPreprocessingInsideModelOption::defaultValue() {
    return InferenceEngine::PluginConfigParams::YES;
}

CheckPreprocessingInsideModelOption::value_type CheckPreprocessingInsideModelOption::parse(const std::string& value) {
    const auto& converters = string2switch();
    VPU_THROW_UNSUPPORTED_OPTION_UNLESS(converters.count(value) != 0, R"(unexpected {} option value "{}", only {} are supported)",
        key(), value, getKeys(converters));
    return converters.at(value);
}

}  // namespace vpu

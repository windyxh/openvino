// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "memory_formats_attribute.hpp"

#include <ngraph/node.hpp>
#include <ngraph/opsets/opset1.hpp>
#include <ngraph/variant.hpp>

using namespace ngraph;

namespace ov {
namespace intel_cpu {

InputMemoryFormats::~InputMemoryFormats() = default;

std::string getInputMemoryFormats(const std::shared_ptr<ngraph::Node>& node) {
    auto it_info = node->get_rt_info().find(InputMemoryFormats::get_type_info_static());
    if (it_info != node->get_rt_info().end()) {
        if (it_info->second.is<InputMemoryFormats>()) {
            return it_info->second.as<InputMemoryFormats>().getMemoryFormats();
        }
    }
    return {};
}

OutputMemoryFormats::~OutputMemoryFormats() = default;

std::string getOutputMemoryFormats(const std::shared_ptr<ngraph::Node>& node) {
    auto it_info = node->get_rt_info().find(OutputMemoryFormats::get_type_info_static());
    if (it_info != node->get_rt_info().end()) {
        if (it_info->second.is<OutputMemoryFormats>()) {
            return it_info->second.as<OutputMemoryFormats>().getMemoryFormats();
        }
    }
    return {};
}

}   // namespace intel_cpu
}   // namespace ov

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/core/model.hpp>
#include <openvino/pass/graph_rewrite.hpp>
#include <openvino/pass/pass.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API ReverseInputChannelsFusion;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief ReverseInputChannelsFusion
 */

class ov::pass::ReverseInputChannelsFusion : public ov::pass::ModelPass {
public:
    OPENVINO_RTTI("ReverseInputChannelsFusion", "0");
    bool run_on_model(const std::shared_ptr<ov::Model>&) override;
};

namespace ngraph {
namespace pass {
using ov::pass::ReverseInputChannelsFusion;
}  // namespace pass
}  // namespace ngraph

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API ConvertDetectionOutput1ToDetectionOutput8;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief ConvertDetectionOutput1ToDetectionOutput8 converts v0::DetectionOutput
 * into v8::DetectionOutput.
 */
class ov::pass::ConvertDetectionOutput1ToDetectionOutput8 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertDetectionOutput1ToDetectionOutput8", "0");
    ConvertDetectionOutput1ToDetectionOutput8();
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertDetectionOutput1ToDetectionOutput8;
}  // namespace pass
}  // namespace ngraph

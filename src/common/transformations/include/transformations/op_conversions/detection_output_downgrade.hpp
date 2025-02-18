// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API ConvertDetectionOutput8ToDetectionOutput1;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief ConvertDetectionOutput8ToDetectionOutput1 converts v8::DetectionOutput
 * into v0::DetectionOutput.
 */
class ov::pass::ConvertDetectionOutput8ToDetectionOutput1 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertDetectionOutput8ToDetectionOutput1", "0");
    ConvertDetectionOutput8ToDetectionOutput1();
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertDetectionOutput8ToDetectionOutput1;
}  // namespace pass
}  // namespace ngraph

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API ConvertDeformableConv8To1;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief ConvertDeformableConv8To1 converts v8::DeformableConvolution into v1::DeformableConvolution.
 */
class ov::pass::ConvertDeformableConv8To1 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertDeformableConv8To1", "0");
    ConvertDeformableConv8To1();
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertDeformableConv8To1;
}  // namespace pass
}  // namespace ngraph

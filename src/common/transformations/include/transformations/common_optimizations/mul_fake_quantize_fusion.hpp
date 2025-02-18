// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>
#include <vector>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API MulFakeQuantizeFusion;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief MulFakeQuantizeFusion transformation replaces following graph:
 * Mul->FakeQuantize to a single FakeQuantize
 * Restrictions:
 * - second input to Mul is a Constant
 */
class ov::pass::MulFakeQuantizeFusion : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("MulFakeQuantizeFusion", "0");
    MulFakeQuantizeFusion();
};

namespace ngraph {
namespace pass {
using ov::pass::MulFakeQuantizeFusion;
}  // namespace pass
}  // namespace ngraph

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API RandomUniformFusion;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief RandomUniformFusion transformation replaces RandomUniform -> Add or
 * RandomUniform -> Mul subgraph with a RandomUniform and replaces min and max const
 * with corrected values.
 */
class ov::pass::RandomUniformFusion : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("RandomUniformFusion", "0");
    RandomUniformFusion();
};

namespace ngraph {
namespace pass {
using ov::pass::RandomUniformFusion;
}  // namespace pass
}  // namespace ngraph

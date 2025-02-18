// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>
#include <utility>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API GeluFusion;
class TRANSFORMATIONS_API GeluFusionWithErfOne;
class TRANSFORMATIONS_API GeluFusionWithErfTwo;
class TRANSFORMATIONS_API GeluFusionWithErfThree;
class TRANSFORMATIONS_API GeluFusionWithErfFour;
class TRANSFORMATIONS_API GeluFusionWithTanh;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief GeluFusion transformation replaces a sub-graph
 * (0.5 * x) * (1 + erf(x / sqrt(2))) with a Gelu op.
 */
class ov::pass::GeluFusionWithErfOne : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("GeluFusionWithErfOne", "0");
    GeluFusionWithErfOne();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief GeluFusion transformation replaces a sub-graph
 * 0.5 * (x * (1 + erf(x / sqrt(2)))) with a Gelu op.
 */
class ov::pass::GeluFusionWithErfTwo : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("GeluFusionWithErfTwo", "0");
    GeluFusionWithErfTwo();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief GeluFusion transformation replaces a sub-graph
 * x * (0.5 * (1 + erf(x / sqrt(2)))) with a Gelu op.
 */
class ov::pass::GeluFusionWithErfThree : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("GeluFusionWithErfThree", "0");
    GeluFusionWithErfThree();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief GeluFusion transformation replaces a sub-graph
 * x * (0.5 + 0.5 * erf(x * (1 / sqrt(2)))) with a Gelu op.
 */
class ov::pass::GeluFusionWithErfFour : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("GeluFusionWithErfFour", "0");
    GeluFusionWithErfFour();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief GeluFusion transformation replaces a sub-graph
 * x * (0.5 * (1 + tanh([sqrt(2 / pi)] * [x + 0.044715^3]))) with a Gelu (Tanh) op.
 */
class ov::pass::GeluFusionWithTanh : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("GeluFusionWithTanh", "0");
    GeluFusionWithTanh();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief GeluFusion transformation replaces various sub-graphs with a Gelu op.
 */
class ov::pass::GeluFusion : public ov::pass::GraphRewrite {
public:
    OPENVINO_RTTI("GeluFusion", "0");
    GeluFusion() {
        add_matcher<ov::pass::GeluFusionWithErfOne>();
        add_matcher<ov::pass::GeluFusionWithErfTwo>();
        add_matcher<ov::pass::GeluFusionWithErfThree>();
        add_matcher<ov::pass::GeluFusionWithErfFour>();
        add_matcher<ov::pass::GeluFusionWithTanh>();
    }
};

namespace ngraph {
namespace pass {
using ov::pass::GeluFusion;
using ov::pass::GeluFusionWithErfFour;
using ov::pass::GeluFusionWithErfOne;
using ov::pass::GeluFusionWithErfThree;
using ov::pass::GeluFusionWithErfTwo;
using ov::pass::GeluFusionWithTanh;
}  // namespace pass
}  // namespace ngraph

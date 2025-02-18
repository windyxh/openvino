// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <openvino/pass/graph_rewrite.hpp>
#include <openvino/pass/pattern/matcher.hpp>
#include <transformations_visibility.hpp>
#include <vector>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API TransposeSinking;
class TRANSFORMATIONS_API TransposeConvert;
class TRANSFORMATIONS_API TransposeEltwise;
class TRANSFORMATIONS_API TransposeReduction;
class TRANSFORMATIONS_API TransposeFQReduction;
class TRANSFORMATIONS_API TransposeFuse;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief TransposeReduction transformation sinks Transpose through Reduce operations
 */
class ov::pass::TransposeReduction : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("TransposeReduction", "0");
    TransposeReduction();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief TransposeFQReduction transformation sinks Transpose through FakeQuantize in case it is followed by reduction
 * or squeeze
 */
class ov::pass::TransposeFQReduction : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("TransposeFQReduction", "0");
    TransposeFQReduction();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief TransposeConvert transformation sinks Transpose through Convert
 */
class ov::pass::TransposeConvert : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("TransposeConvert", "0");
    TransposeConvert();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief TransposeEltwise transformation sinks Transpose through Eltwise
 */
class ov::pass::TransposeEltwise : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("TransposeEltwise", "0");
    TransposeEltwise();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief TransposeFuse transformation eliminates 2 consequtive Transposes if they result in no changes to input or
 * fuses them to single Transpose if input gets changed
 */
class ov::pass::TransposeFuse : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("TransposeFuse", "0");
    TransposeFuse();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief TransposeSinking transformation sinks Transposes through known operations
 */
class ov::pass::TransposeSinking : public ov::pass::GraphRewrite {
public:
    OPENVINO_RTTI("TransposeSinking", "0");
    TransposeSinking() {
        add_matcher<ov::pass::TransposeFQReduction>();
        add_matcher<ov::pass::TransposeReduction>();
        add_matcher<ov::pass::TransposeConvert>();
        add_matcher<ov::pass::TransposeEltwise>();
        add_matcher<ov::pass::TransposeFuse>();
    }
};

namespace ngraph {
namespace pass {
using ov::pass::TransposeConvert;
using ov::pass::TransposeEltwise;
using ov::pass::TransposeFQReduction;
using ov::pass::TransposeFuse;
using ov::pass::TransposeReduction;
using ov::pass::TransposeSinking;
}  // namespace pass
}  // namespace ngraph

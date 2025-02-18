// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API ConvertSoftMax8ToSoftMax1;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief ConvertSoftMax8ToSoftMax1 converts v8::SoftMax into v1::SoftMax.
 */
class ov::pass::ConvertSoftMax8ToSoftMax1 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertSoftMax8ToSoftMax1", "0");
    ConvertSoftMax8ToSoftMax1();
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertSoftMax8ToSoftMax1;
}  // namespace pass
}  // namespace ngraph

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API BroadcastElementwiseFusion;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief Removing Broadcast OP before ElementWise if output shape of Broadcast
 * are equal neighboring input shape of ElementWise.
 */

class ov::pass::BroadcastElementwiseFusion : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("BroadcastElementwiseFusion", "0");
    BroadcastElementwiseFusion();
};

namespace ngraph {
namespace pass {
using ov::pass::BroadcastElementwiseFusion;
}  // namespace pass
}  // namespace ngraph

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

class TRANSFORMATIONS_API ConvertShuffleChannels3;

}  // namespace pass
}  // namespace ov

class ov::pass::ConvertShuffleChannels3 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertShuffleChannels3", "0");
    ConvertShuffleChannels3();
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertShuffleChannels3;
}  // namespace pass
}  // namespace ngraph

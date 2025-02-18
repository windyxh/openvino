// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API ConvertMulticlassNmsToMulticlassNmsIE;

}  // namespace pass
}  // namespace ov

class ov::pass::ConvertMulticlassNmsToMulticlassNmsIE : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("public", "0");
    ConvertMulticlassNmsToMulticlassNmsIE(bool force_i32_output_type = true);
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertMulticlassNmsToMulticlassNmsIE;
}  // namespace pass
}  // namespace ngraph

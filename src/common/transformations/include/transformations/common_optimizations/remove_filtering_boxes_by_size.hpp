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

class TRANSFORMATIONS_API FuseFilteringBoxesBySize;
class TRANSFORMATIONS_API RemoveFilteringBoxesBySize;

}  // namespace pass
}  // namespace ov

class ov::pass::FuseFilteringBoxesBySize : public ov::pass::GraphRewrite {
public:
    OPENVINO_RTTI("FuseFilteringBoxesBySize", "0");
    FuseFilteringBoxesBySize();
};

class ov::pass::RemoveFilteringBoxesBySize : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("RemoveFilteringBoxesBySize", "0");
    RemoveFilteringBoxesBySize();
};

namespace ngraph {
namespace pass {
using ov::pass::FuseFilteringBoxesBySize;
using ov::pass::RemoveFilteringBoxesBySize;
}  // namespace pass
}  // namespace ngraph

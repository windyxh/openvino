// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API ConvertOpSet2ToOpSet1;

}  // namespace pass
}  // namespace ov

class ov::pass::ConvertOpSet2ToOpSet1 : public ov::pass::ModelPass {
public:
    OPENVINO_RTTI("ConvertOpSet2ToOpSet1", "0");
    bool run_on_model(const std::shared_ptr<ov::Model>& m) override;
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertOpSet2ToOpSet1;
}  // namespace pass
}  // namespace ngraph

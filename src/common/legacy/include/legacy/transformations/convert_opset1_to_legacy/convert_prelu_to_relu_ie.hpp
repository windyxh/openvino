// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <vector>
#include <memory>
#include <string>

#include <ie_api.h>

#include <ngraph/pass/graph_rewrite.hpp>

namespace ngraph {
namespace pass {

class ConvertPReLUToReLUIE;

}  // namespace pass
}  // namespace ngraph

class ngraph::pass::ConvertPReLUToReLUIE: public ngraph::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertPReLUToReLUIE", "0");
    ConvertPReLUToReLUIE();
};

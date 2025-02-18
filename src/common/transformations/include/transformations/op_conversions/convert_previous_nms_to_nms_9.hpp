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

class TRANSFORMATIONS_API ConvertNMS1ToNMS9;
class TRANSFORMATIONS_API ConvertNMS3ToNMS9;
class TRANSFORMATIONS_API ConvertNMS4ToNMS9;
class TRANSFORMATIONS_API ConvertNMS5ToNMS9;

}  // namespace pass
}  // namespace ov

class ov::pass::ConvertNMS1ToNMS9 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertNMS1ToNMS9", "0");
    ConvertNMS1ToNMS9();
};

class ov::pass::ConvertNMS3ToNMS9 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertNMS3ToNMS9", "0");
    ConvertNMS3ToNMS9();
};

class ov::pass::ConvertNMS4ToNMS9 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertNMS4ToNMS9", "0");
    ConvertNMS4ToNMS9();
};

class ov::pass::ConvertNMS5ToNMS9 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertNMS5ToNMS9", "0");
    ConvertNMS5ToNMS9();
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertNMS1ToNMS9;
using ov::pass::ConvertNMS3ToNMS9;
using ov::pass::ConvertNMS4ToNMS9;
using ov::pass::ConvertNMS5ToNMS9;
}  // namespace pass
}  // namespace ngraph

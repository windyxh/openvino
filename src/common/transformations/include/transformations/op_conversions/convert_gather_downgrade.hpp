// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API ConvertGather7ToGather1;
class TRANSFORMATIONS_API ConvertGather8ToGather7;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief ConvertGather7ToGather1 converts v7::Gather into v1::Gather.
 */
class ov::pass::ConvertGather7ToGather1 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertGather7ToGather1", "0");
    ConvertGather7ToGather1();
};

/**
 * @ingroup ie_transformation_common_api
 * @brief ConvertGather8ToGather7 converts v8::Gather into v7::Gather.
 */
class ov::pass::ConvertGather8ToGather7 : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("ConvertGather8ToGather7", "0");
    ConvertGather8ToGather7();
};

namespace ngraph {
namespace pass {
using ov::pass::ConvertGather7ToGather1;
using ov::pass::ConvertGather8ToGather7;
}  // namespace pass
}  // namespace ngraph

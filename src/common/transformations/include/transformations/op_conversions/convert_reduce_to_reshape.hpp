// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <algorithm>
#include <memory>
#include <openvino/core/rt_info.hpp>
#include <openvino/opsets/opset1.hpp>
#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>
#include <vector>

namespace ov {
namespace pass {

// Convert redundant reduce to reshape : expected to be optimized out or modified
class TRANSFORMATIONS_API ConvertReduceToReshape;
class TRANSFORMATIONS_API ConvertReduceMeanToReshape;
class TRANSFORMATIONS_API ConvertReduceSumToReshape;
class TRANSFORMATIONS_API ConvertReduceMaxToReshape;
class TRANSFORMATIONS_API ConvertReduceMinToReshape;
class TRANSFORMATIONS_API ConvertReduceLogicalAndToReshape;
class TRANSFORMATIONS_API ConvertReduceLogicalOrToReshape;

}  // namespace pass
}  // namespace ov

class CvtReduceBase : public ov::pass::MatcherPass {
public:
    template <class T>
    ov::matcher_pass_callback convert_reduce_to_reshape();

    bool is_redundant(ov::Shape input, ov::Shape output);
};

class ov::pass::ConvertReduceMeanToReshape : public CvtReduceBase {
public:
    OPENVINO_RTTI("ConvertReduceMeanToReshape", "0");
    ConvertReduceMeanToReshape();
};

class ov::pass::ConvertReduceSumToReshape : public CvtReduceBase {
public:
    OPENVINO_RTTI("ConvertReduceSumToReshape", "0");
    ConvertReduceSumToReshape();
};

class ov::pass::ConvertReduceMaxToReshape : public CvtReduceBase {
public:
    OPENVINO_RTTI("ConvertReduceMaxToReshape", "0");
    ConvertReduceMaxToReshape();
};

class ov::pass::ConvertReduceMinToReshape : public CvtReduceBase {
public:
    OPENVINO_RTTI("ConvertReduceMinToReshape", "0");
    ConvertReduceMinToReshape();
};

class ov::pass::ConvertReduceLogicalAndToReshape : public CvtReduceBase {
public:
    OPENVINO_RTTI("ConvertReduceLogicalAndToReshape", "0");
    ConvertReduceLogicalAndToReshape();
};

class ov::pass::ConvertReduceLogicalOrToReshape : public CvtReduceBase {
public:
    OPENVINO_RTTI("ConvertReduceLogicalOrToReshape", "0");
    ConvertReduceLogicalOrToReshape();
};

class ov::pass::ConvertReduceToReshape : public ov::pass::GraphRewrite {
public:
    OPENVINO_RTTI("ConvertReduceToReshape", "0");
    // Handling reduce if it can be converted to reshape (check input/output tensor)
    ConvertReduceToReshape() {
        // Redundant reduce based on its mode
        add_matcher<ConvertReduceMeanToReshape>();
        add_matcher<ConvertReduceSumToReshape>();
        add_matcher<ConvertReduceMaxToReshape>();
        add_matcher<ConvertReduceMinToReshape>();
        add_matcher<ConvertReduceLogicalAndToReshape>();
        add_matcher<ConvertReduceLogicalOrToReshape>();
    }
};

template <class T>
ov::matcher_pass_callback CvtReduceBase::convert_reduce_to_reshape() {
    return [&](ov::pass::pattern::Matcher& m) {
        auto reduce = std::dynamic_pointer_cast<T>(m.get_match_root());
        if (!reduce)
            return false;

        auto input = reduce->input_value(0);
        const auto input_shape = input.get_shape();
        const auto reduce_shape = reduce->output(0).get_shape();

        // convert redundant reduce to reshape if the input shape is supported by reshape
        if (is_redundant(input_shape, reduce_shape) && input_shape.size() < 6) {
            const auto reshape_shape = reduce->output(0).get_shape();
            auto reshape = std::make_shared<ov::opset1::Reshape>(
                input,
                ov::opset1::Constant::create(ov::element::i64, ov::Shape{reshape_shape.size()}, reshape_shape),
                true);

            reshape->set_friendly_name(reduce->get_friendly_name());
            copy_runtime_info(reduce, reshape);
            replace_node(reduce, reshape);
            return true;
        }

        return false;
    };
}

namespace ngraph {
namespace pass {
using ov::pass::ConvertReduceLogicalAndToReshape;
using ov::pass::ConvertReduceLogicalOrToReshape;
using ov::pass::ConvertReduceMaxToReshape;
using ov::pass::ConvertReduceMeanToReshape;
using ov::pass::ConvertReduceMinToReshape;
using ov::pass::ConvertReduceSumToReshape;
using ov::pass::ConvertReduceToReshape;
}  // namespace pass
}  // namespace ngraph

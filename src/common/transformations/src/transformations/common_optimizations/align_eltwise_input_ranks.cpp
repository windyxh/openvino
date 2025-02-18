// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "transformations/common_optimizations/align_eltwise_input_ranks.hpp"

#include <ngraph/pattern/op/wrap_type.hpp>
#include <openvino/opsets/opset8.hpp>

ov::pass::AlignEltwiseInputRanks::AlignEltwiseInputRanks() {
    auto eltwise_pattern = pattern::wrap_type<opset8::SquaredDifference,
                                              op::util::BinaryElementwiseComparison,
                                              op::util::BinaryElementwiseLogical,
                                              op::util::BinaryElementwiseArithmetic,
                                              opset8::FakeQuantize>(pattern::has_static_rank());

    matcher_pass_callback callback = [=](pattern::Matcher& m) {
        auto node = m.get_match_root();

        auto fq = as_type<opset8::FakeQuantize>(node.get());
        if (fq) {
            if (fq->get_auto_broadcast() != ngraph::op::AutoBroadcastType::NUMPY) {
                return false;
            }
        } else if (node->get_autob() != ngraph::op::AutoBroadcastType::NUMPY) {
            return false;
        }

        // NormalizeL2 and Multiply can be fused to NormalizeIE.
        // NormalizeIE has an attribute called channel_shared, which is set
        // based on Multiply's constant input rank - it's true if the rank is 1.
        // So we skip extending Multiply's constant input rank here.
        if (ov::is_type<opset8::Multiply>(node)) {
            auto inputs = node->input_values();
            if (std::any_of(inputs.begin(), inputs.end(), [](const Output<Node>& input) -> bool {
                    return ov::is_type<opset8::NormalizeL2>(input.get_node());
                }))
                return false;
        }

        const auto rank = node->get_output_partial_shape(0).size();

        for (size_t i = 0; i < node->get_input_size(); i++) {
            auto const_node = as_type<opset8::Constant>(node->get_input_node_ptr(i));
            if (const_node == nullptr)
                continue;
            const auto& const_shape = const_node->get_shape();
            auto diff = rank - const_shape.size();
            if (diff > 0) {
                Shape new_shape = const_shape;
                new_shape.insert(new_shape.begin(), diff, 1);
                auto new_const = std::make_shared<opset8::Constant>(*const_node, new_shape);
                node->input(i).replace_source_output(new_const);
            }
        }

        return false;
    };

    auto m = std::make_shared<pattern::Matcher>(eltwise_pattern, "AlignEltwiseInputRanks");
    this->register_matcher(m, callback);
}

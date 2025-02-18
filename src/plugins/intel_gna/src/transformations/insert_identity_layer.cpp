// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
#include <openvino/cc/ngraph/itt.hpp>

#include "transformations/insert_identity_layer.hpp"
#include "transformations/rt_info/gna_precision_change_flag.hpp"

#include <ngraph/opsets/opset9.hpp>
#include <ngraph/pass/graph_rewrite.hpp>
#include <ngraph/pattern/op/or.hpp>
#include <ngraph/pattern/op/wrap_type.hpp>
#include <ngraph/rt_info.hpp>
#include <ops/identity.hpp>
#include <legacy/ngraph_ops/eltwise.hpp>
#include "log/log.hpp"
#include "ops/util/util.hpp"

using namespace ov::intel_gna;
using namespace ov::intel_gna::pass;
using namespace ov::intel_gna::rt_info;
using namespace ov::intel_gna::ngraph_util;

namespace {
void mark_for_identity_insertion(std::shared_ptr<ngraph::Node> node,
                                 size_t input_index) {
    log::debug() << "Mark input as candidate for identity insertion " << input_index << ":" << node->get_friendly_name() << std::endl;
    auto input = node->input(input_index);
    add_precision_change_flag(input, ov::element::i32, ov::element::i16);
}

std::shared_ptr<ov::intel_gna::op::Identity> create_indentity(std::shared_ptr<ngraph::Node>& input_op) {
    auto identity_op = std::make_shared<ov::intel_gna::op::Identity>(input_op);
    // Keep name of previous operation
    identity_op->set_friendly_name(input_op->get_friendly_name());
    input_op->set_friendly_name(input_op->get_friendly_name() + "/previous");
    ngraph::copy_runtime_info(input_op, identity_op);
    return identity_op;
}

void insert_identity_layer_after(std::shared_ptr<ngraph::Node>& input_op,
                                 size_t index) {
    NGRAPH_CHECK(input_op);

    log::debug() << "Insert identity layer after " << input_op->get_friendly_name() <<
        " (" << input_op->get_type_name() << "):"<< index << std::endl;

    auto consumers = input_op->output(index).get_target_inputs();
    auto identity_op = create_indentity(input_op);
    for (auto& consumer : consumers) {
        consumer.replace_source_output(identity_op);
    }
}

void insert_identity_layer_between(std::shared_ptr<ngraph::Node>& input_op,
                                   std::shared_ptr<ngraph::Node>& output_op,
                                   size_t index) {
    NGRAPH_CHECK(input_op);
    NGRAPH_CHECK(output_op);

    log::debug() << "Insert identity layer after " << input_op->get_friendly_name() <<
        " (" << input_op->get_type_name() << ") and before " << index << ":" <<
         output_op->get_friendly_name() << " (" << output_op->get_type_name() << ")" << std::endl;

    auto identity_op = create_indentity(input_op);
    output_op->input(index).replace_source_output(identity_op);
}

// forward declaration
bool walk_through_the_outputs(std::shared_ptr<ov::Node>& prev_node,
                              size_t& prev_node_output_index,
                              const std::shared_ptr<ov::Node>& node,
                              bool first_iteration = false);

bool process_next_node(std::shared_ptr<ov::Node>& prev_node,
                       size_t& prev_node_output_index,
                       const std::shared_ptr<ov::Node>& node,
                       const size_t input_index) {
    // Check whether node is going to be skipped
    bool to_be_skipped = (is_gna_precision_agnostic(node) && !std::dynamic_pointer_cast<ngraph::opset9::Concat>(node)) ||
        is_pooling(node);
    if (to_be_skipped) {
        // if it is pooling, update previous node, since activation
        // should be inserted after the pooling
        if (is_pooling(node)) {
            prev_node = node;
            // supported pooling from opset7 has 1 output port
            prev_node_output_index = 0;
        }
        // walk over all outputs of this node
        return walk_through_the_outputs(prev_node, prev_node_output_index, node);
    }
    // Don't skip this node, check whether precision is changed
    if (is_precision_changed(node->input(input_index))) {
        // if at least one target input requires Identity insertion,
        // process the appropriate output port
        // if there are other comsumers with i32 input
        // diagonal layer will be insrted anyway before them
        insert_identity_layer_after(prev_node, prev_node_output_index);
        // graph modified
        return true;
    }
    return false;
}

bool walk_through_the_outputs(std::shared_ptr<ov::Node>& prev_node,
                              size_t& prev_node_output_index,
                              const std::shared_ptr<ov::Node>& node,
                              bool first_iteration) {
    bool is_identity_inserted = false;
    // walk over all outputs
    for (size_t i = 0; i < node->get_output_size(); i++) {
        // check all target inputs node of this output
        for (auto&& input : node->output(i).get_target_inputs()) {
            // if it is first iteration track output port id
            // because prev_node is functional
            if (first_iteration)
                prev_node_output_index = i;
            // recursively check next node, skipping precision agnostic
            if (process_next_node(prev_node, prev_node_output_index, input.get_node()->shared_from_this(), input.get_index())) {
                // graph is modified
                is_identity_inserted = true;
                // go to the next output, other target inputs are not interesting anymore
                break;
            }
        }
    }
    return is_identity_inserted;
}
} // namespace

bool MarkIdentityCandidates::run_on_model(const std::shared_ptr<ov::Model>& m) {
    RUN_ON_FUNCTION_SCOPE(MarkIdentityCandidates);
    for (auto& node : m->get_ordered_ops()) {
        auto check_previos_node_and_mark = [&node](){
            for (size_t i = 0; i < node->get_input_size(); i++) {
                auto prev_node = node->get_input_node_shared_ptr(i);
                prev_node = get_prev_node_skipping_certain(prev_node, is_gna_precision_agnostic);
                if (has_32bit_output(prev_node) || is_pooling(prev_node)) {
                    mark_for_identity_insertion(node, i);
                }
            }
        };
        if (std::dynamic_pointer_cast<ngraph::op::Eltwise>(node)) {
            auto input0_node = node->get_input_node_shared_ptr(0);
            auto input1_node = node->get_input_node_shared_ptr(1);
            auto func_input0_node = get_prev_node_skipping_certain(input0_node, is_gna_precision_agnostic);
            auto func_input1_node = get_prev_node_skipping_certain(input1_node, is_gna_precision_agnostic);
            if (is_eltwise_add(node) && !is_low_precision_input) {
                if (!has_32bit_output(func_input0_node) || !has_32bit_output(func_input1_node))
                    continue;

                mark_for_identity_insertion(node, 0);
            } else if (is_eltwise_mul(node) || (is_eltwise_add(node) && is_low_precision_input)) {
                if (has_8bit_or_16_bit_output(func_input0_node) && has_8bit_or_16_bit_output(func_input1_node))
                    continue;

                if (has_32bit_output(func_input0_node)) {
                    mark_for_identity_insertion(node, 0);
                }

                if (has_32bit_output(func_input1_node)) {
                    mark_for_identity_insertion(node, 1);
                }
            }
        } else if (std::dynamic_pointer_cast<ngraph::opset9::Concat>(node) != nullptr) {
            check_previos_node_and_mark();
        } else {
            if (is_gna_precision_agnostic(node) || has_32bit_input(node) ||
                ngraph::op::is_parameter(node) || ngraph::op::is_constant(node) ||
                ngraph::op::is_output(node) || ngraph::op::is_sink(node)) {
                continue;
            }
            check_previos_node_and_mark();
        }
    }
    return false;
}

BreakFusingOfOutputLayers::BreakFusingOfOutputLayers() {
    MATCHER_SCOPE(BreakFusingOfOutputLayers);

    auto result_op = ngraph::pattern::wrap_type<ngraph::opset9::Result>({ngraph::pattern::any_input()});

    ngraph::matcher_pass_callback callback = [=](ngraph::pattern::Matcher& m) {
        const auto& pattern_map = m.get_pattern_value_map();
        auto result_node = pattern_map.at(result_op).get_node_shared_ptr();
        auto input_node = result_node->get_input_node_shared_ptr(0);

        for (size_t i = 0; i < input_node->get_output_size(); i++) {
            for (auto&& input : input_node->output(i).get_target_inputs()) {
                if (!is_activation(input.get_node())) {
                    continue;
                }
                insert_identity_layer_between(input_node, result_node, 0);
                return true;
            }
        }
        return false;
    };

    auto m = std::make_shared<ngraph::pattern::Matcher>(result_op, matcher_name);
    this->register_matcher(m, callback);
}

bool InsertIdentity::run_on_model(const std::shared_ptr<ov::Model>& m) {
    RUN_ON_FUNCTION_SCOPE(InsertIdentity);
    bool is_graph_modifed = false;

    for (auto& node : m->get_ordered_ops()) {
        // if node has 8 bit or 16 bit output already or Result or State, it is not our case, skip it
        if (has_8bit_or_16_bit_output(node) || ngraph::op::is_output(node) || ngraph::op::is_sink(node))
            continue;

        // walk through the all outputs
        std::shared_ptr<ov::Node> prev_node = node;
        size_t prev_node_output_index = 0;
        is_graph_modifed |= walk_through_the_outputs(prev_node, prev_node_output_index, node, true);
    }
    return is_graph_modifed;
}

bool IdentityCandidatesCleanup::run_on_model(const std::shared_ptr<ov::Model>& f) {
    RUN_ON_FUNCTION_SCOPE(IdentityCandidatesCleanup);
    for (auto& node : f->get_ordered_ops()) {
        for (auto& input : node->inputs()) {
            remove_precision_change_flag(input);
        }
    }
    return false;
}

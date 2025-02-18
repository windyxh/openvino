// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op_table.hpp"
#include "openvino/opsets/opset10.hpp"

using namespace std;
using namespace ov;
using namespace ov::opset10;

namespace ov {
namespace frontend {
namespace tensorflow {
namespace op {
Output<Node> normalize_selected_indices(const Output<Node>& ov_selected_indices,
                                        const Output<Node>& max_output_size,
                                        bool pad_to_max_output_size) {
    // OpenVINO NonMaxSuppression outputs selected_indices in a form [batch_index, class_index, box_index]
    // but we need only box_index
    auto start = make_shared<Constant>(element::i32, Shape{1}, 2);
    auto stop = make_shared<Constant>(element::i32, Shape{1}, 3);
    auto step = make_shared<Constant>(element::i32, Shape{1}, 1);
    auto axis = make_shared<Constant>(element::i32, Shape{1}, 1);
    Output<Node> selected_indices = make_shared<Slice>(ov_selected_indices, start, stop, step, axis)->output(0);
    selected_indices = make_shared<Squeeze>(selected_indices, axis)->output(0);

    if (pad_to_max_output_size) {
        auto num_selected_indices = make_shared<ShapeOf>(selected_indices, max_output_size.get_element_type());
        auto num_padded_elements = make_shared<Subtract>(max_output_size, num_selected_indices);
        auto pad_element = make_shared<Constant>(selected_indices.get_element_type(), Shape{}, 0);
        auto pads_begin = make_shared<Constant>(max_output_size.get_element_type(), Shape{1}, 0);
        auto pad_mode = ov::op::PadMode::CONSTANT;
        selected_indices =
            make_shared<Pad>(selected_indices, pads_begin, num_padded_elements, pad_element, pad_mode)->output(0);
    }

    return selected_indices;
}

OutputVector translate_non_max_suppression_op(const NodeContext& node) {
    default_op_checks(node,
                      3,
                      {"NonMaxSuppression",
                       "NonMaxSuppressionV2",
                       "NonMaxSuppressionV3",
                       "NonMaxSuppressionV4",
                       "NonMaxSuppressionV5"});
    auto boxes = node.get_input(0);
    auto scores = node.get_input(1);
    auto max_output_size = node.get_input(2);

    // prepare boxes: in TensorFlow NonMaxSupression operation there is no batch dimension
    // so we need to introduce it to be aligned with OpenVINO NonMaxSuppression
    // boxes of shape [num_batches, num_boxes, 4]
    auto boxes_axis = make_shared<Constant>(element::i32, Shape{1}, 0);
    auto ov_boxes = make_shared<Unsqueeze>(boxes, boxes_axis);

    // prepare scores: in TensorFlow NonMaxSupression operation there is no batch and class dimensions
    // so we need to introduce them to be aligned with OpenVINO NonMaxSuppression scores
    // of shape [num_batches, num_classes, num_boxes]
    auto scores_axes = make_shared<Constant>(element::i32, Shape{2}, vector<int32_t>{0, 1});
    auto ov_scores = make_shared<Unsqueeze>(scores, scores_axes);

    const auto& op_type = node.get_op_type();
    OutputVector results;

    // set all thresholds to zero, default values
    Output<Node> iou_threshold = make_shared<Constant>(element::f32, Shape{}, 0.0);
    Output<Node> score_threshold = make_shared<Constant>(element::f32, Shape{}, 0.0);
    Output<Node> soft_nms_sigma = make_shared<Constant>(element::f32, Shape{}, 0.0);
    bool pad_to_max_output_size = false;
    bool valid_outputs = false;
    bool selected_scores = false;
    if (op_type == "NonMaxSuppression") {
        // NonMaxSuppression has three inputs and one output, selected indices
        // iou_threshold comes as attribute
        auto iou_threshold_value = node.get_attribute<float>("iou_threshold", 0.5);
        iou_threshold = make_shared<Constant>(element::f32, Shape{}, iou_threshold_value);
    } else if (op_type == "NonMaxSuppressionV2") {
        // NonMaxSuppressionV2 has four inputs and one output, selected indices
        // iou_threshold comes as input
        default_op_checks(node, 4, {"NonMaxSuppressionV2"});
        auto iou_threshold = node.get_input(3);
    } else if (op_type == "NonMaxSuppressionV3") {
        // NonMaxSuppressionV3 has five inputs and one output, selected indices
        // score_threshold is a new input
        default_op_checks(node, 5, {"NonMaxSuppressionV3"});
        iou_threshold = node.get_input(3);
        score_threshold = node.get_input(4);
    } else if (op_type == "NonMaxSuppressionV4") {
        // NonMaxSuppressionV4 has five inputs (same as NonMaxSuppressionV3) and two outputs (selected indices and
        // valid_outputs) pad_to_max_output_size is a new attribute
        default_op_checks(node, 5, {"NonMaxSuppressionV4"});
        iou_threshold = node.get_input(3);
        score_threshold = node.get_input(4);
        pad_to_max_output_size = node.get_attribute<bool>("pad_to_max_output_size", false);
        valid_outputs = true;
    } else if (op_type == "NonMaxSuppressionV5") {
        // NonMaxSuppressionV5 has six inputs and three outputs (selected indices, selected scores,
        // valid_outputs)
        // soft_nms_sigma is a new input
        default_op_checks(node, 6, {"NonMaxSuppressionV5"});
        iou_threshold = node.get_input(3);
        score_threshold = node.get_input(4);
        soft_nms_sigma = node.get_input(5);
        pad_to_max_output_size = node.get_attribute<bool>("pad_to_max_output_size", false);
        selected_scores = true;
        valid_outputs = true;
    } else {
        TENSORFLOW_OP_VALIDATION(node, false, "Internal error TensorFlow Frontend: no translator for " + op_type);
    }

    auto non_max_suppression = make_shared<NonMaxSuppression>(ov_boxes,
                                                              ov_scores,
                                                              max_output_size,
                                                              iou_threshold,
                                                              score_threshold,
                                                              soft_nms_sigma,
                                                              NonMaxSuppression::BoxEncodingType::CORNER,
                                                              false,
                                                              element::i32);
    auto tf_selected_indices =
        normalize_selected_indices(non_max_suppression->output(0), max_output_size, pad_to_max_output_size);
    results.push_back(tf_selected_indices);

    Output<Node> tf_selected_scores;
    if (selected_scores) {
        tf_selected_scores =
            normalize_selected_indices(non_max_suppression->output(1), max_output_size, pad_to_max_output_size);
        tf_selected_scores = make_shared<Convert>(tf_selected_scores, boxes.get_element_type())->output(0);
        results.push_back(tf_selected_scores);
    }

    Output<Node> tf_valid_outputs;
    if (valid_outputs) {
        // In TensorFlow valid_outputs output is a scalar
        tf_valid_outputs = make_shared<Squeeze>(non_max_suppression->output(2))->output(0);
        results.push_back(tf_valid_outputs);
    }

    // set output tensor names based on a number of outputs
    set_node_name(node.get_name(), tf_selected_indices.get_node_shared_ptr());
    if (valid_outputs && !selected_scores) {
        set_node_name(node.get_name(), tf_selected_indices.get_node_shared_ptr());
        set_node_name(node.get_name() + ":1", tf_valid_outputs.get_node_shared_ptr());
    } else if (valid_outputs && selected_scores) {
        set_node_name(node.get_name(), tf_selected_indices.get_node_shared_ptr());
        set_node_name(node.get_name() + ":1", tf_selected_scores.get_node_shared_ptr());
        set_node_name(node.get_name() + ":2", tf_valid_outputs.get_node_shared_ptr());
    }

    return results;
}
}  // namespace op
}  // namespace tensorflow
}  // namespace frontend
}  // namespace ov

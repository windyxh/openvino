// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op_table.hpp"
#include "openvino/opsets/opset10.hpp"

using namespace std;
using namespace ov;

namespace ov {
namespace frontend {
namespace tensorflow {
namespace op {
OutputVector translate_unique_op(const NodeContext& node) {
    // This operation returns a tensor y containing all of the unique elements of x sorted in the same order that they
    // occur in x. This operation also returns a tensor idx the same size as x that contains the index of each value of
    // x in the unique output y.
    default_op_checks(node, 1, {"Unique"});
    auto node_name = node.get_name();
    auto input_values = node.get_input(0);
    auto output_indices_type = node.get_attribute<ov::element::Type>("out_idx", ov::element::i32);
    auto unique = make_shared<opset10::Unique>(input_values, false, output_indices_type);

    // set up new Unique node name and tensor names manually
    // because the second and fourth outputs of OpenVINO Unique are not needed
    unique->set_friendly_name(node.get_name());
    set_out_name(node_name + ":0", unique->output(0));
    set_out_name(node_name + ":1", unique->output(2));

    return {unique->output(0), unique->output(2)};
}

}  // namespace op
}  // namespace tensorflow
}  // namespace frontend
}  // namespace ov

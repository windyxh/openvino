// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "common_op_table.hpp"
#include "openvino/opsets/opset8.hpp"

using namespace std;
using namespace ov::opset8;

namespace ov {
namespace frontend {
namespace tensorflow {
namespace op {

OutputVector translate_unpack_op(const NodeContext& node) {
    default_op_checks(node, 1, {"Unpack", "UNPACK"});
    auto value = node.get_input(0);
    auto axis = node.get_attribute<int64_t>("axis", 0);
    auto num = node.get_attribute<int64_t>("num");

    auto axis_const = make_shared<Constant>(element::i64, Shape{}, axis);
    auto split = make_shared<Split>(value, axis_const, num);
    OutputVector unpack_outputs;
    for (int output_ind = 0; output_ind < num; ++output_ind) {
        auto unpack_output = make_shared<Squeeze>(split->output(output_ind), axis_const);
        set_out_name(node.get_name() + ":" + to_string(output_ind), unpack_output);
        unpack_outputs.push_back(unpack_output);
    }
    return unpack_outputs;
}
}  // namespace op
}  // namespace tensorflow
}  // namespace frontend
}  // namespace ov

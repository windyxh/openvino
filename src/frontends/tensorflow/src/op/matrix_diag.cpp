// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op_table.hpp"
#include "openvino/op/util/attr_types.hpp"
#include "openvino/opsets/opset8.hpp"
#include "utils.hpp"

using namespace std;
using namespace ov::opset8;

namespace ov {
namespace frontend {
namespace tensorflow {
namespace op {

OutputVector translate_matrix_diag_op(const NodeContext& node) {
    // The translation of MatrixDiag to OpenVINO opset relies on padding of input tensor with zeros,
    // reshape to a special form and cutting of unneeded padding part.
    // Here is a basic idea described by an example,
    // let us have a tensor [1, 2, 3] and generate padding tensor of zeros with a shape [3, 3].
    // Concatenate input tensor with padding and get the following:
    // [[1, 0, 0, 0]
    //  [2, 0, 0, 0]
    //  [3, 0, 0, 0]] of shape [3, 4]
    // Reshape to tensor of a shape [12] equal to [1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0]
    // Cut off last 3 elements and get [1, 0, 0, 0, 2, 0, 0, 0, 3] and reshape to [3, 3]
    // This idea is generalized to higher rank tensors
    TENSORFLOW_OP_VALIDATION(node, node.get_input_size() > 0, "MatrixDiag must have at least one input.");
    // diagonal is the single input to MatrixDiag operation and has a shape [I, J, ..., M, N]
    auto diagonal = node.get_input(0);
    auto diagonal_type = diagonal.get_element_type();

    // 1. unsqueeze to have at least three rank input of a shape [1, I, J, ..., M, N, 1]
    //    because dimensions [I, J, ..., M] can be absent
    auto unsqueeze_axis = make_shared<Constant>(element::i64, Shape{2}, std::vector<int64_t>{0, -1});
    auto unsqueeze_diag = make_shared<Unsqueeze>(diagonal, unsqueeze_axis);

    // 2. compute a size of the last dimension of the diagonal input of a shape [I, J, ..., M, N],
    //    i.e. N that will be diagonalized
    auto unsqueeze_diag_shape = make_shared<ShapeOf>(unsqueeze_diag);
    auto last_dim = make_shared<StridedSlice>(unsqueeze_diag_shape,
                                              make_shared<Constant>(element::i64, Shape{1}, std::vector<int64_t>{-2}),
                                              make_shared<Constant>(element::i64, Shape{1}, std::vector<int64_t>{-1}),
                                              make_shared<Constant>(element::i64, Shape{1}, std::vector<int64_t>{1}),
                                              std::vector<int64_t>({0}),
                                              std::vector<int64_t>({0}));

    // 3. generate a tensor of zeros of a shape [1, I, J, ..., M, N, N]
    auto diag_shape = make_shared<ShapeOf>(diagonal);
    auto one_dim = make_shared<Constant>(last_dim->get_element_type(), Shape{1}, std::vector<int64_t>{1});
    auto padding_shape = make_shared<Concat>(OutputVector({one_dim, diag_shape, last_dim}), 0);
    auto padding =
        make_shared<Broadcast>(make_shared<Constant>(diagonal_type, Shape{1}, std::vector<int64_t>{0}), padding_shape);

    // 4. concatenate to get input tensor with zero padding of a shape [1, I, J, ..., M, N, N + 1]
    auto zero_padded_diag = make_shared<Concat>(OutputVector({unsqueeze_diag, padding}), -1);

    // reshape padded tensor to get a shape [I, J, ..., M, N * N + N]
    // 4.1 retrieve a part of the shape value [1, I, J, ..., M]
    auto new_shape_padded_diag1 =
        make_shared<StridedSlice>(unsqueeze_diag_shape,
                                  make_shared<Constant>(element::i64, Shape{1}, std::vector<int64_t>{0}),
                                  make_shared<Constant>(element::i64, Shape{1}, std::vector<int64_t>{-2}),
                                  make_shared<Constant>(element::i64, Shape{1}, std::vector<int64_t>{1}),
                                  std::vector<int64_t>({0}),
                                  std::vector<int64_t>({0}));
    // 4.2 compute the last part of a shape that is [N * N + N]
    auto last_dim_squared = make_shared<Multiply>(last_dim, last_dim);
    auto new_shape_padded_diag2 = make_shared<Add>(last_dim_squared, last_dim);
    // 4.3 compute a new shape and reshape padded diagonal
    auto new_shape_padded_diag = make_shared<Concat>(OutputVector({new_shape_padded_diag1, new_shape_padded_diag2}), 0);
    auto reshaped_padded_diag = make_shared<Reshape>(zero_padded_diag, new_shape_padded_diag, false);

    // 5. cut off padding in the reshaped padded tensor to get a shape [1, I, J, ..., M, N * N]
    auto cut_padded_diag = make_shared<Slice>(
        reshaped_padded_diag,
        make_shared<Constant>(last_dim_squared->get_element_type(), Shape{1}, std::vector<int64_t>{0}),
        last_dim_squared,
        make_shared<Constant>(last_dim_squared->get_element_type(), Shape{1}, std::vector<int64_t>{1}),
        make_shared<Constant>(element::i64, Shape{1}, std::vector<int64_t>{-1}));

    // 6. return the expected shape for the result [I, J, ..., M, N, N]
    auto resulted_shape = make_shared<Concat>(OutputVector({diag_shape, last_dim}), 0);
    auto resulted_diag = make_shared<Reshape>(cut_padded_diag, resulted_shape, false);

    set_node_name(node.get_name(), resulted_diag);
    return {resulted_diag};
}
}  // namespace op
}  // namespace tensorflow
}  // namespace frontend
}  // namespace ov

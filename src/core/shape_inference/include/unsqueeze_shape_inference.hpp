// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include "openvino/op/unsqueeze.hpp"
#include "utils.hpp"

namespace ov {
namespace op {
namespace v0 {

template <class TOp>
void check_unsqueeze_axes_rank(const TOp* op, const Rank& rank) {
    NODE_VALIDATION_CHECK(op,
                          is_rank_compatible_any_of(rank, {0, 1}),
                          "Second input (axes) should not be of rank higher than 1. Got: ",
                          rank);
}

template <class T>
void shape_infer(const Unsqueeze* op,
                 const std::vector<T>& input_shapes,
                 std::vector<T>& output_shapes,
                 const std::map<size_t, std::shared_ptr<ngraph::runtime::HostTensor>>& constant_data = {}) {
    NODE_VALIDATION_CHECK(op, input_shapes.size() == 2 && output_shapes.size() == 1);
    check_unsqueeze_axes_rank(op, input_shapes[1].rank());
    const auto& arg_shape = input_shapes[0];
    auto& out_shape = output_shapes[0];

    std::vector<int64_t> axes_val;
    const auto has_axes = get_data_as_int64<T>(1, op, axes_val, constant_data);

    if (has_axes && arg_shape.rank().is_static()) {
        NODE_VALIDATION_CHECK(op, !axes_val.empty(), "'axes' input is mandatory");
        // Remove repeated axes on input
        std::unordered_set<int64_t> tmp(axes_val.begin(), axes_val.end());
        std::vector<int64_t> unique_axes(tmp.begin(), tmp.end());

        const auto expanded_rank = arg_shape.rank().get_length() + unique_axes.size();

        // Normalize then remove repeated axes after normalization.
        normalize_axes(op, expanded_rank, unique_axes);
        const std::set<int64_t> axes(unique_axes.begin(), unique_axes.end());

        out_shape = arg_shape;
        for (const auto& axis : axes) {
            NODE_VALIDATION_CHECK(op,
                                  static_cast<size_t>(axis) <= out_shape.size() + 1,
                                  "provided 'axes' value ",
                                  axis,
                                  " is not valid.");
            // As shape not throw exception on repeated axis it has to be check if insert or append dimension.
            // This will be not required if this op has same behaviour as numpy expand_dims.
            if (static_cast<size_t>(axis) <= out_shape.size()) {
                out_shape.insert(std::next(std::begin(out_shape), axis), 1);
            } else {
                // Append dimension at end when there is difference in size of input axes and after normalization
                // e.g. input shape {2,3,4} axes_value(4,-1) then output rank is determined as 5,
                // but after final normalization and removing duplicates it points sam location in shape.
                // The numpy throws exception "repeated axis" in that case.
                out_shape.push_back(1);
            }
        }
    } else {
        out_shape = ov::PartialShape::dynamic();
    }
}
}  // namespace v0
}  // namespace op
}  // namespace ov

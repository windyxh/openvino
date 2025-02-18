// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include "openvino/frontend/exception.hpp"

namespace ov {
namespace frontend {
namespace tensorflow {

class NodeContext;

class OpValidationFailure : public ov::frontend::OpValidationFailure {
public:
    OpValidationFailure(const CheckLocInfo& check_loc_info, const NodeContext& node, const std::string& explanation)
        : ov::frontend::OpValidationFailure(check_loc_info, get_error_msg_prefix_tf(node), explanation) {}

private:
    static std::string get_error_msg_prefix_tf(const NodeContext& node);
};
}  // namespace tensorflow
}  // namespace frontend

/// \brief Macro to check whether a boolean condition holds.
/// \param node_context Object of NodeContext class
/// \param cond Condition to check
/// \param ... Additional error message info to be added to the error message via the `<<`
///            stream-insertion operator. Note that the expressions here will be evaluated lazily,
///            i.e., only if the `cond` evalutes to `false`.
/// \throws ::ov::OpValidationFailure if `cond` is false.
#define TENSORFLOW_OP_VALIDATION(node_context, ...) \
    OPENVINO_ASSERT_HELPER(::ov::frontend::tensorflow::OpValidationFailure, (node_context), __VA_ARGS__)
}  // namespace ov

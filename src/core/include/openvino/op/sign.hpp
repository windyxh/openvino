// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "openvino/op/util/unary_elementwise_arithmetic.hpp"

namespace ov {
namespace op {
namespace v0 {
/// \brief Elementwise sign operation.
///
/// \ingroup ov_ops_cpp_api
class OPENVINO_API Sign : public util::UnaryElementwiseArithmetic {
public:
    OPENVINO_OP("Sign", "opset1", util::UnaryElementwiseArithmetic);
    BWDCMP_RTTI_DECLARATION;

    Sign() = default;
    /// \brief Constructs an elementwise sign operation.
    ///
    /// \param arg Node that produces the input tensor.
    Sign(const Output<Node>& arg);

    bool visit_attributes(AttributeVisitor& visitor) override;
    std::shared_ptr<Node> clone_with_new_inputs(const OutputVector& new_args) const override;
    OPENVINO_SUPPRESS_DEPRECATED_START
    bool evaluate(const HostTensorVector& outputs, const HostTensorVector& inputs) const override;
    OPENVINO_SUPPRESS_DEPRECATED_END
    bool has_evaluate() const override;
};
}  // namespace v0
}  // namespace op
}  // namespace ov

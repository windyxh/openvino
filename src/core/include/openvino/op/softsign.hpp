// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "openvino/op/util/unary_elementwise_arithmetic.hpp"

namespace ov {
namespace op {
namespace v9 {
class OPENVINO_API SoftSign : public util::UnaryElementwiseArithmetic {
public:
    OPENVINO_OP("SoftSign", "opset9", util::UnaryElementwiseArithmetic);
    BWDCMP_RTTI_DECLARATION;

    SoftSign() = default;
    /// \brief Constructs a SoftSign operation.
    ///
    /// \param arg Node that produces the input tensor.<br>
    /// `[d0, ...]`
    /// Output `[d0, ...]`
    ///
    SoftSign(const Output<Node>& arg);

    bool visit_attributes(AttributeVisitor& visitor) override;

    void validate_and_infer_types() override;

    std::shared_ptr<Node> clone_with_new_inputs(const OutputVector& new_args) const override;
    bool evaluate(ov::TensorVector& outputs,
                  const ov::TensorVector& inputs,
                  const ov::EvaluationContext& evaluation_context) const override;
    bool has_evaluate() const override;
};
}  // namespace v9
}  // namespace op
}  // namespace ov

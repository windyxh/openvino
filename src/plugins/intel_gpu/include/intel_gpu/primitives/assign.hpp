// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <vector>

#include "primitive.hpp"
#include "intel_gpu/runtime/memory.hpp"

namespace cldnn {
/// @addtogroup cpp_api C++ API
/// @{
/// @addtogroup cpp_topology Network Topology
/// @{
/// @addtogroup cpp_primitives Primitives
/// @{

/// @brief Sets an input value to the variable_id variable.
struct assign : public primitive_base<assign> {
    CLDNN_DECLARE_PRIMITIVE(assign)

    /// @brief Constructs Assign primitive.
    /// @param id This primitive id
    /// @param inputs Input parameters ids
    /// @param variable_id Variable id
    /// @param output_layout Memory layout
    assign(const primitive_id &id,
               const std::vector<input_info>& inputs,
               const std::string& variable_id,
               const layout& output_layout)
                : primitive_base(id, inputs, {padding()}, {optional_data_type{output_layout.data_type}}),
                  variable_id{variable_id},
                  output_layout{output_layout} {}

    std::string variable_id;
    layout output_layout;
};
/// @}
/// @}
/// @}
}  // namespace cldnn

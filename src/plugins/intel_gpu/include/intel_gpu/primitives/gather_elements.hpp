// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once
#include "primitive.hpp"

namespace cldnn {

/// @brief
/// @details
struct gather_elements : public primitive_base<gather_elements> {
    CLDNN_DECLARE_PRIMITIVE(gather_elements)

    /// @brief Constructs gather_elements primitive.
    /// @param id This primitive id.
    /// @param data Input data primitive id.
    /// @param indices Input indexes primitive id.
    /// @param output_format Output format.
    /// @param output_shape Output shape.
    /// @param axis Gathering axis.
    gather_elements(const primitive_id& id,
                    const input_info& data,
                    const input_info& indices,
                    const format& output_format,
                    const tensor& output_shape,
                    const int64_t axis,
                    const padding& output_padding = padding())
        : primitive_base(id, {data, indices}, {output_padding}), output_format(output_format), output_shape(output_shape), axis(axis) {}

    gather_elements(const primitive_id& id,
                    const input_info& data,
                    const input_info& indices,
                    const int64_t axis,
                    const padding& output_padding = padding())
        : primitive_base(id, {data, indices}, {output_padding}), output_format({}), output_shape({}), axis(axis) {}

    /// @brief Gather Elements output format
    format output_format;
    /// @brief Gather Elements output shape
    tensor output_shape;

    /// @brief Which axis to gather on.
    int64_t axis;

    size_t hash() const override {
        size_t seed = primitive::hash();
        seed = hash_combine(seed, output_format.value);
        seed = hash_combine(seed, axis);
        return seed;
    }

    bool operator==(const primitive& rhs) const override {
        if (!compare_common_params(rhs))
            return false;

        auto rhs_casted = downcast<const gather_elements>(rhs);

        return output_format == rhs_casted.output_format &&
               axis == rhs_casted.axis;
    }
};
}  // namespace cldnn

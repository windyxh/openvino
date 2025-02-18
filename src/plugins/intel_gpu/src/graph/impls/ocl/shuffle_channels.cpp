// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "shuffle_channels_inst.h"
#include "primitive_base.hpp"
#include "impls/implementation_map.hpp"
#include "kernel_selector_helper.h"
#include "shuffle_channels/shuffle_channels_kernel_selector.h"
#include "shuffle_channels/shuffle_channels_kernel_ref.h"
#include "intel_gpu/runtime/error_handler.hpp"

using namespace cldnn;

namespace cldnn {
namespace ocl {

struct shuffle_channels_impl : typed_primitive_impl_ocl<shuffle_channels> {
    using parent = typed_primitive_impl_ocl<shuffle_channels>;
    using parent::parent;
    using kernel_selector_t = kernel_selector::shuffle_channels_kernel_selector;
    using kernel_params_t = std::pair<kernel_selector::shuffle_channels_params, kernel_selector::shuffle_channels_optional_params>;

    DECLARE_OBJECT_TYPE_SERIALIZATION

    std::unique_ptr<primitive_impl> clone() const override {
        return make_unique<shuffle_channels_impl>(*this);
    }

    static kernel_params_t get_kernel_params(const kernel_impl_params& impl_param) {
        const auto& primitive = impl_param.typed_desc<shuffle_channels>();
        auto params = get_default_params<kernel_selector::shuffle_channels_params>(impl_param);
        auto optional_params = get_default_optional_params<kernel_selector::shuffle_channels_optional_params>(impl_param.get_program());

        const int32_t number_of_dims = 4;
        int32_t axis = primitive->axis;

        if (axis < 0)
            axis += number_of_dims;

        params.group = primitive->group;
        params.axis = axis;

        return {params, optional_params};
    }
};

namespace detail {

attach_shuffle_channels_impl::attach_shuffle_channels_impl() {
    implementation_map<shuffle_channels>::add(impl_types::ocl, typed_primitive_impl_ocl<shuffle_channels>::create<shuffle_channels_impl>, {
        std::make_tuple(data_types::f32, format::bfyx),
        std::make_tuple(data_types::f16, format::bfyx),
        std::make_tuple(data_types::u8, format::bfyx),
        std::make_tuple(data_types::i8, format::bfyx),
        std::make_tuple(data_types::f32, format::b_fs_yx_fsv16),
        std::make_tuple(data_types::f16, format::b_fs_yx_fsv16),
        std::make_tuple(data_types::u8, format::b_fs_yx_fsv16),
        std::make_tuple(data_types::i8, format::b_fs_yx_fsv16),
        std::make_tuple(data_types::f32, format::b_fs_yx_fsv4),
        std::make_tuple(data_types::f16, format::b_fs_yx_fsv4),
        std::make_tuple(data_types::u8, format::b_fs_yx_fsv4),
        std::make_tuple(data_types::i8, format::b_fs_yx_fsv4),
        std::make_tuple(data_types::f32, format::bs_fs_yx_bsv16_fsv16),
        std::make_tuple(data_types::f16, format::bs_fs_yx_bsv16_fsv16),
        std::make_tuple(data_types::u8, format::bs_fs_yx_bsv16_fsv16),
        std::make_tuple(data_types::i8, format::bs_fs_yx_bsv16_fsv16),
        std::make_tuple(data_types::f32, format::fs_b_yx_fsv32),
        std::make_tuple(data_types::f16, format::fs_b_yx_fsv32),
    });
}

}  // namespace detail
}  // namespace ocl
}  // namespace cldnn

BIND_BINARY_BUFFER_WITH_TYPE(cldnn::ocl::shuffle_channels_impl)

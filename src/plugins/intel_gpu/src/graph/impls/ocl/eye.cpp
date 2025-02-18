// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <data_inst.h>
#include <eye/eye_kernel_ref.h>
#include <eye/eye_kernel_selector.h>
#include <eye_inst.h>

#include <algorithm>
#include <cstddef>
#include <impls/implementation_map.hpp>
#include <intel_gpu/runtime/error_handler.hpp>
#include <vector>

#include "primitive_base.hpp"

namespace cldnn {
namespace ocl {

struct eye_impl : typed_primitive_impl_ocl<eye> {
    using parent = typed_primitive_impl_ocl<eye>;
    using parent::parent;
    using kernel_selector_t = kernel_selector::eye_kernel_selector;
    using kernel_params_t = std::pair<kernel_selector::eye_params, kernel_selector::eye_optional_params>;

    DECLARE_OBJECT_TYPE_SERIALIZATION

    std::unique_ptr<primitive_impl> clone() const override {
        return make_unique<eye_impl>(*this);
    }

    static kernel_params_t get_kernel_params(const kernel_impl_params& impl_param) {
        const auto& primitive = impl_param.typed_desc<eye>();
        auto params = get_default_params<kernel_selector::eye_params>(impl_param);
        auto op_params = get_default_optional_params<kernel_selector::eye_optional_params>(impl_param.get_program());

        params.diagonal_index = primitive->shift;

        return {params, {}};
    }
};

namespace detail {

attach_eye_impl::attach_eye_impl() {
    const std::vector<data_types> types{data_types::f16,
                                        data_types::f32,
                                        data_types::i8,
                                        data_types::u8,
                                        data_types::i32,
                                        data_types::i64};
    const std::vector<format::type> formats{
        format::bfyx,
        format::b_fs_yx_fsv16,
        format::b_fs_yx_fsv32,
        format::bs_fs_yx_bsv16_fsv16,
        format::bs_fs_yx_bsv32_fsv32,
        format::bs_fs_yx_bsv32_fsv16,

        format::bfzyx,
        format::b_fs_zyx_fsv16,
        format::b_fs_zyx_fsv32,
        format::bs_fs_zyx_bsv16_fsv32,
        format::bs_fs_zyx_bsv16_fsv16,
        format::bs_fs_zyx_bsv32_fsv32,
        format::bs_fs_zyx_bsv32_fsv16,
    };
    implementation_map<eye>::add(impl_types::ocl, typed_primitive_impl_ocl<eye>::create<eye_impl>, types, formats);
}

}  // namespace detail

}  // namespace ocl
}  // namespace cldnn

BIND_BINARY_BUFFER_WITH_TYPE(cldnn::ocl::eye_impl)

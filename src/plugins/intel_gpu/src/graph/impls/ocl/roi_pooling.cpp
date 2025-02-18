// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "roi_pooling_inst.h"
#include "primitive_base.hpp"
#include "impls/implementation_map.hpp"
#include "intel_gpu/runtime/error_handler.hpp"
#include "kernel_selector_helper.h"
#include "roi_pooling/roi_pooling_kernel_selector.h"
#include "roi_pooling/roi_pooling_kernel_ref.h"

namespace cldnn {
namespace ocl {

namespace {
kernel_selector::pool_type cldnn_2_pool_type(pooling_mode mode) {
    switch (mode) {
        case pooling_mode::max:
            return kernel_selector::pool_type::MAX;
        case pooling_mode::average:
            return kernel_selector::pool_type::AVG;
        case pooling_mode::average_no_padding:
            return kernel_selector::pool_type::AVG;
        case pooling_mode::bilinear:
            return kernel_selector::pool_type::BILINEAR;
        case pooling_mode::deformable_bilinear:
            return kernel_selector::pool_type::DEFORMABLE_BILINEAR;
        default:
            assert(0);
            return kernel_selector::pool_type::MAX;
    }
}
}  // namespace

struct roi_pooling_impl : typed_primitive_impl_ocl<roi_pooling> {
    using parent = typed_primitive_impl_ocl<roi_pooling>;
    using parent::parent;
    using kernel_selector_t = kernel_selector::roi_pooling_kernel_selector;
    using kernel_params_t = std::pair<kernel_selector::roi_pooling_params, kernel_selector::roi_pooling_optional_params>;

    DECLARE_OBJECT_TYPE_SERIALIZATION

    std::unique_ptr<primitive_impl> clone() const override {
        return make_unique<roi_pooling_impl>(*this);
    }

protected:
    kernel_arguments_data get_arguments(const typed_primitive_inst<roi_pooling>& instance, int32_t) const override {
        kernel_arguments_data args;

        if (instance.argument->mode == pooling_mode::deformable_bilinear && !instance.argument->no_trans)
            args.inputs = {
                instance.input_memory_ptr(),
                instance.rois_memory(),
                instance.trans_memory()};
        else
            args.inputs = {instance.input_memory_ptr(), instance.rois_memory()};

        args.outputs = { instance.output_memory_ptr() };

        return args;
    }

public:
    static kernel_params_t get_kernel_params(const kernel_impl_params& impl_param) {
        const auto& primitive = impl_param.typed_desc<roi_pooling>();
        const auto& rois_layout = impl_param.get_input_layout(1);

        auto params = get_default_params<kernel_selector::roi_pooling_params>(impl_param);
        auto optional_params = get_default_optional_params<kernel_selector::roi_pooling_optional_params>(impl_param.get_program());

        params.inputs.push_back(convert_data_tensor(rois_layout));
        if (primitive->mode == pooling_mode::deformable_bilinear && !primitive->no_trans)
            params.inputs.push_back(convert_data_tensor(impl_param.get_input_layout(2)));
        params.mode = cldnn_2_pool_type(primitive->mode);
        params.position_sensitive = primitive->position_sensitive;
        params.pooled_width = primitive->pooled_width;
        params.pooled_height = primitive->pooled_height;
        params.spatial_scale = primitive->spatial_scale;
        params.spatial_bins_x = primitive->spatial_bins_x;
        params.spatial_bins_y = primitive->spatial_bins_y;
        params.trans_std = primitive->trans_std;
        params.no_trans = primitive->no_trans;
        params.part_size = primitive->part_size;
        params.group_size = primitive->group_size;

        return {params, optional_params};
    }
};

namespace detail {

attach_roi_pooling_impl::attach_roi_pooling_impl() {
    auto formats = {format::bfyx,
                    format::b_fs_yx_fsv16,
                    format::b_fs_yx_fsv32,
                    format::bs_fs_yx_bsv16_fsv16,
                    format::bs_fs_yx_bsv32_fsv32,
                    format::bs_fs_yx_bsv32_fsv16};

    auto types = {data_types::f16, data_types::f32};

    implementation_map<roi_pooling>::add(impl_types::ocl, typed_primitive_impl_ocl<roi_pooling>::create<roi_pooling_impl>, types, formats);
}

}  // namespace detail
}  // namespace ocl
}  // namespace cldnn

BIND_BINARY_BUFFER_WITH_TYPE(cldnn::ocl::roi_pooling_impl)

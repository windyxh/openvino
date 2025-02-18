// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "intel_gpu/primitives/experimental_detectron_topk_rois.hpp"
#include "primitive_inst.h"
#include "intel_gpu/runtime/error_handler.hpp"

namespace cldnn {

using experimental_detectron_topk_rois_node = typed_program_node<experimental_detectron_topk_rois>;

template<>
class typed_primitive_inst<experimental_detectron_topk_rois> : public typed_primitive_inst_base<experimental_detectron_topk_rois> {
    using parent = typed_primitive_inst_base<experimental_detectron_topk_rois>;
    using parent::parent;

public:
    static layout calc_output_layout(experimental_detectron_topk_rois_node const &node, kernel_impl_params const& impl_param);

    static std::string to_string(experimental_detectron_topk_rois_node const &node);

public:
    typed_primitive_inst(network &network, experimental_detectron_topk_rois_node const &desc);
};

using experimental_detectron_topk_rois_inst = typed_primitive_inst<experimental_detectron_topk_rois>;

} // namespace cldnn

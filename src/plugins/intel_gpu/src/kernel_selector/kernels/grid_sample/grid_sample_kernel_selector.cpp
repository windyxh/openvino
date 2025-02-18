// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "grid_sample_kernel_selector.hpp"

#include "grid_sample_kernel_ref.hpp"

namespace kernel_selector {

grid_sample_kernel_selector::grid_sample_kernel_selector() {
    Attach<GridSampleKernelRef>();
}

KernelsData grid_sample_kernel_selector::GetBestKernels(const Params& params, const optional_params& options) const {
    return GetNaiveBestKernel(params, options, KernelType::GRID_SAMPLE);
}

grid_sample_kernel_selector& grid_sample_kernel_selector::Instance() {
    static grid_sample_kernel_selector instance;
    return instance;
}

}  // namespace kernel_selector

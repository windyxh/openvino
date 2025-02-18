// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "prior_grid_generator_kernel_selector.h"
#include "prior_grid_generator_kernel_ref.h"

namespace kernel_selector {

experimental_detectron_prior_grid_generator_kernel_selector::experimental_detectron_prior_grid_generator_kernel_selector() {
    Attach<ExperimentalDetectronPriorGridGeneratorKernelRef>();
}

KernelsData experimental_detectron_prior_grid_generator_kernel_selector::GetBestKernels(const Params& params, const optional_params& options) const {
    return GetNaiveBestKernel(params, options, KernelType::EXPERIMENTAL_DETECTRON_PRIOR_GRID_GENERATOR);
}
}  // namespace kernel_selector

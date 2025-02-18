// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "dft_kernel_selector.h"

#include "dft_kernel_ref.h"

namespace kernel_selector {

dft_kernel_selector::dft_kernel_selector() {
    Attach<DFTKernelRef>();
}

KernelsData dft_kernel_selector::GetBestKernels(const Params& params, const optional_params& options) const {
    return GetNaiveBestKernel(params, options, KernelType::DFT);
}

dft_kernel_selector& dft_kernel_selector::Instance() {
    static dft_kernel_selector instance;
    return instance;
}

}  // namespace kernel_selector

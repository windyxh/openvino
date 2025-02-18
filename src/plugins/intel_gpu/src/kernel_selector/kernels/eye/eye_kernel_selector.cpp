// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
#include "eye_kernel_selector.h"

#include "eye_kernel_ref.h"

namespace kernel_selector {

eye_kernel_selector::eye_kernel_selector() {
    Attach<EyeKernelRef>();
}

KernelsData eye_kernel_selector::GetBestKernels(const Params& params, const optional_params& options) const {
    return GetNaiveBestKernel(params, options, KernelType::EYE);
}

}  // namespace kernel_selector

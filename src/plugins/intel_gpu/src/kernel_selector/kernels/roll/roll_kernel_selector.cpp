// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "roll_kernel_selector.hpp"

#include "roll_kernel_ref.hpp"

namespace kernel_selector {

roll_kernel_selector::roll_kernel_selector() {
    Attach<RollKernelRef>();
}

KernelsData roll_kernel_selector::GetBestKernels(const Params& params, const optional_params& options) const {
    return GetNaiveBestKernel(params, options, KernelType::ROLL);
}

roll_kernel_selector& roll_kernel_selector::Instance() {
    static roll_kernel_selector instance;
    return instance;
}

}  // namespace kernel_selector

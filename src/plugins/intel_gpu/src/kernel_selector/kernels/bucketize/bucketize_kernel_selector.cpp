// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "bucketize_kernel_selector.hpp"

#include "bucketize_kernel_ref.hpp"

namespace kernel_selector {

bucketize_kernel_selector::bucketize_kernel_selector() {
    Attach<BucketizeKernelRef>();
}

KernelsData bucketize_kernel_selector::GetBestKernels(const Params& params, const optional_params& options) const {
    return GetNaiveBestKernel(params, options, KernelType::BUCKETIZE);
}

bucketize_kernel_selector& bucketize_kernel_selector::Instance() {
    static bucketize_kernel_selector instance;
    return instance;
}

}  // namespace kernel_selector

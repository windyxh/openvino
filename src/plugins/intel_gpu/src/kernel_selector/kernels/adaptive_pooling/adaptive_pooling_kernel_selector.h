// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "kernel_selector.h"

namespace kernel_selector {
class adaptive_pooling_kernel_selector : public kernel_selector_base {
public:
    static adaptive_pooling_kernel_selector& Instance();

    adaptive_pooling_kernel_selector();

    KernelsData GetBestKernels(const Params& params, const optional_params& options) const override;
};
}  // namespace kernel_selector

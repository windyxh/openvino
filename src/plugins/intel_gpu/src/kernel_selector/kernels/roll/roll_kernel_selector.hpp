// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once
#include "kernel_selector.h"

namespace kernel_selector {

class roll_kernel_selector : public kernel_selector_base {
public:
    roll_kernel_selector();
    KernelsData GetBestKernels(const Params& params, const optional_params& options) const override;
    static roll_kernel_selector& Instance();
};

}  // namespace kernel_selector

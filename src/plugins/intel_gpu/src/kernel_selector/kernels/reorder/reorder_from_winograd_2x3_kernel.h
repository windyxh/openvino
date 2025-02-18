﻿// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "reorder_kernel_base.h"

namespace kernel_selector {
class ReorderFromWinograd2x3Kernel : public ReorderKernelBase {
public:
    ReorderFromWinograd2x3Kernel() : ReorderKernelBase("reorder_from_winograd_2x3_s1") {}

    KernelsData GetKernelsData(const Params& params, const optional_params& options) const override;
    KernelsPriority GetKernelsPriority(const Params& params, const optional_params& options) const override;
    ParamsKey GetSupportedKey() const override;
    JitConstants GetJitConstants(const reorder_params& params) const override;
    DispatchData SetDefault(const reorder_params& arg) const override;
};
}  // namespace kernel_selector
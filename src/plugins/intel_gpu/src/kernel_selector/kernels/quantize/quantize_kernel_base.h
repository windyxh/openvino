﻿// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "kernel_base_opencl.h"
#include "quantize_kernel_params.h"

namespace kernel_selector {

class QuantizeKernelBase : public KernelBaseOpenCL {
public:
    using KernelBaseOpenCL::KernelBaseOpenCL;
    virtual ~QuantizeKernelBase() {}

    bool Validate(const Params& p, const optional_params& o) const override;
    KernelsData GetKernelsData(const Params& params, const optional_params& options) const override;

protected:
    virtual JitConstants GetJitConstants(const quantize_params& params, const CommonDispatchData& dispatchData) const;
    virtual CommonDispatchData SetDefault(const quantize_params& params, const optional_params&) const = 0;
};
}  // namespace kernel_selector

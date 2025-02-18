// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "pooling_kernel_base.h"

namespace kernel_selector {
class PoolingKernel_b_fs_yx_fsv16 : public PoolingKernelBase {
public:
    PoolingKernel_b_fs_yx_fsv16() : PoolingKernelBase("pooling_gpu_blocked") {}
    virtual ~PoolingKernel_b_fs_yx_fsv16() {}

    KernelsData GetKernelsData(const Params& params, const optional_params& options) const override;
    KernelsPriority GetKernelsPriority(const Params& params, const optional_params& options) const override;
    ParamsKey GetSupportedKey() const override;

protected:
    bool Validate(const Params&, const optional_params&) const override;
    JitConstants GetJitConstants(const pooling_params& params, DispatchData dispatchData) const override;
    DispatchData SetDefault(const pooling_params& params) const override;
    std::vector<FusedOpType> GetSupportedFusedOps() const override {
        return { FusedOpType::ELTWISE,
                 FusedOpType::QUANTIZE,
                 FusedOpType::ACTIVATION };
    }

    size_t GetBlockSize(const pooling_params& params) const;
    size_t GetSimdSize(const pooling_params& params) const;
};
}  // namespace kernel_selector

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "pooling_kernel_base.h"

namespace kernel_selector {
class PoolingKerneGPU_fs_b_yx_fsv32 : public PoolingKernelBase {
public:
    PoolingKerneGPU_fs_b_yx_fsv32() : PoolingKernelBase("pooling_gpu_fs_b_yx_fsv32") {}
    virtual ~PoolingKerneGPU_fs_b_yx_fsv32() {}

    KernelsData GetKernelsData(const Params& params, const optional_params& options) const override;
    KernelsPriority GetKernelsPriority(const Params& params, const optional_params& options) const override;
    ParamsKey GetSupportedKey() const override;
    DispatchData SetDefault(const pooling_params& params) const override;

protected:
    bool Validate(const Params& p, const optional_params& o) const override;
    JitConstants GetJitConstants(const pooling_params& params, DispatchData dispatchData) const override;
    std::vector<FusedOpType> GetSupportedFusedOps() const override {
        return { FusedOpType::ELTWISE,
                 FusedOpType::QUANTIZE,
                 FusedOpType::ACTIVATION };
    }
};
}  // namespace kernel_selector

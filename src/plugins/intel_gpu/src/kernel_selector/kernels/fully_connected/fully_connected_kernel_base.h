﻿// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "weight_bias_kernel_base.h"
#include "fully_connected_params.h"
#include <string>
#include <vector>

namespace kernel_selector {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FullyConnectedKernelBase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FullyConnectedKernelBase : public WeightBiasKernelBase {
public:
    using WeightBiasKernelBase::WeightBiasKernelBase;
    using FusedOpDesc = fused_operation_desc;
    virtual ~FullyConnectedKernelBase() {}

    struct DispatchData : public CommonDispatchData {
        uint32_t unit_byte_size = 0;
        const char* chunk_type;
        uint32_t chunk_byte_size = 0;
        uint32_t units_per_chunk = 0;
        uint32_t bytes_per_sg_read = 0;
        uint32_t units_per_sg_read = 0;
        uint32_t responses_per_sg_exec = 0;
        uint32_t in_chunk_prefetch_size = 0;
        uint32_t filter_chunk_prefetch_size = 0;

        uint32_t last_rg_size = 0;
        uint32_t rg_count = 0;

        // Gemm style params
        uint32_t tile_m = 0;
        uint32_t tile_n = 0;
        uint32_t tile_mk = 0;
        uint32_t tile_nk = 0;
        uint32_t tile_ms = 0;
        uint32_t tile_ns = 0;
    };

    std::string GetAutoTuneOptions(int autoTuneIndex) const;
    std::vector<std::string> autoTuneOptions = {DEFAULT, NO_PRERA_SCH, AGE_BASED};
    virtual KernelsData GetTunedKernelsDataByIndex(const Params &params,
                                                   const optional_params &options,
                                                   DataLayout dl,
                                                   WeightsLayout wl,
                                                   const int autoTuneIndex = -1) const;

protected:
    virtual JitConstants GetJitConstants(const fully_connected_params& params, const DispatchData& dispatchData) const;
    virtual DispatchData SetDefault(const fully_connected_params& params, int autoTuneIndex = -1) const;
    KernelsData GetCommonKernelsData(const Params &params,
                                     const optional_params &options,
                                     DataLayout dl,
                                     WeightsLayout wl,
                                     const std::string exeMode = DEFAULT,
                                     int autoTuneIndex = -1) const;

    // Fused ops
    virtual JitConstants GetFusedPrimitivesJitConstants(const fully_connected_params& params, const DispatchData& dispatchData) const;
    Datatype GetAccumulatorType(const fully_connected_params& params) const;
    Datatype GetActivationType(const fully_connected_params& params) const;
    // --Fused ops

    bool Validate(const Params& p, const optional_params&) const override;
};
}  // namespace kernel_selector

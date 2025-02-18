// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "kernel_base_opencl.h"

namespace kernel_selector {

/**
 * ExperimentalDetectronTopKROIs kernel params.
 */
struct experimental_detectron_topk_roi_params : public base_params {
    experimental_detectron_topk_roi_params() : base_params(KernelType::EXPERIMENTAL_DETECTRON_TOPK_ROIS) {}

    size_t max_rois = 0; // maximal numbers of output ROIs.
};

struct experimental_detectron_topk_roi_optional_params : public optional_params {
    experimental_detectron_topk_roi_optional_params() : optional_params(KernelType::EXPERIMENTAL_DETECTRON_TOPK_ROIS) {}
};

/**
 * Reference GPU kernel for the ExperimentalDetectronTopKROIs-6 operation to set output by indices sorted before.
 */
class ExperimentalDetectronTopKROIRef : public KernelBaseOpenCL {
public:
    ExperimentalDetectronTopKROIRef() : KernelBaseOpenCL("experimental_detectron_topk_rois_ref") {}

private:
    virtual JitConstants GetJitConstants(const experimental_detectron_topk_roi_params &params) const;

    KernelsData GetKernelsData(const Params &params, const optional_params &options) const override;

    KernelsPriority GetKernelsPriority(const Params &params, const optional_params &options) const override;

    bool Validate(const Params &params, const optional_params &optionalParams) const override;

    ParamsKey GetSupportedKey() const override;
};
}  // namespace kernel_selector

// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "topk_rois_kernel_selector.h"
#include "topk_rois_ref.h"

namespace kernel_selector {


experimental_detectron_topk_rois_kernel_selector &experimental_detectron_topk_rois_kernel_selector::Instance() {
    static experimental_detectron_topk_rois_kernel_selector instance_;
    return instance_;
}

experimental_detectron_topk_rois_kernel_selector::experimental_detectron_topk_rois_kernel_selector() {
    Attach<ExperimentalDetectronTopKROIRef>();
}

KernelsData experimental_detectron_topk_rois_kernel_selector::GetBestKernels(const Params &params,
                                                                             const optional_params &options) const {
    return GetNaiveBestKernel(params, options, KernelType::EXPERIMENTAL_DETECTRON_TOPK_ROIS);
}

} // namespace kernel_selector

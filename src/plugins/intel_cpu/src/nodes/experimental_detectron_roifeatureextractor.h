// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <ie_common.h>
#include <node.h>

namespace ov {
namespace intel_cpu {
namespace node {

class ExperimentalDetectronROIFeatureExtractor : public Node {
public:
    ExperimentalDetectronROIFeatureExtractor(const std::shared_ptr<ngraph::Node>& op, const dnnl::engine& eng, WeightsSharing::Ptr &cache);

    void getSupportedDescriptors() override {};
    void initSupportedPrimitiveDescriptors() override;
    void execute(dnnl::stream strm) override;
    bool created() const override;

    bool needPrepareParams() const override { return false; };
    void executeDynamicImpl(dnnl::stream strm) override { execute(strm); };

    static bool isSupportedOperation(const std::shared_ptr<const ngraph::Node>& op, std::string& errorMessage) noexcept;

private:
    const int INPUT_ROIS {0};
    const int INPUT_FEATURES_START {1};

    const int OUTPUT_ROI_FEATURES {0};
    const int OUTPUT_ROIS {1};

    int output_dim_ = 0;
    int pooled_height_ = 0;
    int pooled_width_ = 0;
    std::vector<int64_t> pyramid_scales_;
    int sampling_ratio_ = 0;
    bool aligned_ = false;
};

}   // namespace node
}   // namespace intel_cpu
}   // namespace ov

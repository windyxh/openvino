// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <vector>

#include <vpu/model/stage.hpp>

namespace vpu {

class StubStage final : public StageNode {
public:
    using StageNode::StageNode;

private:
    StagePtr cloneImpl() const override;

    void propagateDataOrderImpl(StageDataInfo<DimsOrder>& orderInfo) override;

    void getDataStridesRequirementsImpl(StageDataInfo<StridesRequirement>& stridesInfo) override;

    void finalizeDataLayoutImpl() override;

    void getBatchSupportInfoImpl(StageDataInfo<BatchSupport>& batchInfo) override;

    void initialCheckImpl() const override;

    void finalCheckImpl() const override;

    void serializeParamsImpl(BlobSerializer& serializer) const override;

    void serializeDataImpl(BlobSerializer& serializer) const override;
};

}  // namespace vpu

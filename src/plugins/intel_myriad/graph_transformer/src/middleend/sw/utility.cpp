// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <vpu/middleend/sw/utility.hpp>

#include <memory>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include <vpu/model/model.hpp>
#include <vpu/utils/profiling.hpp>

namespace vpu {

//
// getOneOfSingleNextStage
//

Stage getOneOfSingleNextStage(
        const Stage& curStage,
        const std::unordered_set<StageType, EnumClassHash>& supportedTypes) {
    IE_ASSERT(curStage->numOutputs() == 1);

    auto output = curStage->output(0);

    IE_ASSERT(output->parentData() == nullptr);
    IE_ASSERT(output->numChildDatas() == 0);

    if (output->usage() != DataUsage::Intermediate) {
        return nullptr;
    }

    if (output->numConsumers() != 1) {
        return nullptr;
    }

    auto consumer = output->singleConsumer();
    if (supportedTypes.count(consumer->type()) != 0) {
        return consumer;
    }

    return nullptr;
}

StageVector getExactNextStages(
        const Stage& parent,
        const std::vector<StageType>& requiredTypes) {
    IE_ASSERT(parent != nullptr);

    StageVector result;

    for (const auto& type : requiredTypes) {
        const auto& nextStages = parent->nextStages();
        auto isRequested = [&](const Stage& stage) {
            const bool isNotAlreadyIncluded = std::find(result.cbegin(), result.cend(), stage) == result.cend();
            return stage->type() == type && isNotAlreadyIncluded;
        };

        const auto& successor = std::find_if(nextStages.begin(), nextStages.end(), isRequested);

        if (successor == nextStages.end()) {
            return {};
        }

        result.push_back(*successor);
    }

    if (result.size() == requiredTypes.size()) {
        return result;
    }

    return {};
}

}  // namespace vpu

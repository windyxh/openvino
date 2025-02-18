// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "ngraph/node.hpp"

#include <memory>

namespace vpu {

void dynamicToStaticNonMaxSuppression(std::shared_ptr<ngraph::Node> node);

}  // namespace vpu

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API SoftmaxFusion;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief SoftmaxFusion transformation replaces following graph:
 *
 *            +---------------+
 *            │               │
 *            │     input     │
 *            │               │
 *            +---------------+
 *                │      │
 *                │      v
 *                │ +-----------+
 *                │ │           │
 *                │ │ ReduceMax │
 *                │ │           │
 *                │ +-----------+
 *                │      │
 *                │      │
 *                v      v
 *            +---------------+
 *            │               │
 *            │      Sub      │
 *            │               │
 *            +---------------+
 *                    |
 *                    |
 *                    v
 *            +---------------+
 *            │               │
 *            │      Exp      │
 *            │               │
 *            +---------------+
 *                │      │
 *                │      v
 *                │ +-----------+
 *                │ │           │
 *                │ │ ReduceSum │
 *                │ │           │
 *                │ +-----------+
 *                │      │
 *                │      │
 *                v      v
 *             +-------------+
 *             |             │
 *             |     Div     │
 *             │             │
 *             +-------------+
 *
 * to a single Softmax node
 *
 * * Restrictions:
 *   - ReduceMax and ReduceSum axes must be scalar constants and they have to point to the same axis
 */

class ov::pass::SoftmaxFusion : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("SoftmaxFusion", "0");
    SoftmaxFusion();
};

namespace ngraph {
namespace pass {
using ov::pass::SoftmaxFusion;
}  // namespace pass
}  // namespace ngraph

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API SimplifyCTCGreedyDecoderSeqLen;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief SimplifyCTCGreedyDecoder converts v6:CTCGreedyDecoderSeqLen into v0::CTCGreedyDecoder.
 *
 *            data[N, T, C]    seq_len[N]
 *                   \          /
 *            CTCGreedyDecoderSeqLen
 *
 * will be converted to
 *
 *           data[T, N, C]   seq_mask[T, N]
 *                    \         /
 *                  CTCGreedyDecoder
 *                    /         \
 *       class_index[N, T]    seq_len[N]
 *
 * The transformation works only for case when the blank_index input == C-1, where C is the number of classes.
 */
class ov::pass::SimplifyCTCGreedyDecoderSeqLen : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("SimplifyCTCGreedyDecoderSeqLen", "0");
    SimplifyCTCGreedyDecoderSeqLen();
};

namespace ngraph {
namespace pass {
using ov::pass::SimplifyCTCGreedyDecoderSeqLen;
}  // namespace pass
}  // namespace ngraph

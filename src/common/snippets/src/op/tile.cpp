// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "snippets/op/tile.hpp"
#include "snippets/generator.hpp"

using namespace std;
using namespace ngraph;

snippets::op::Tile::Tile(const std::vector<AllocatedEmitter>& region, size_t increment,
                         size_t num_inputs, size_t num_outputs,
                         const std::vector<size_t>& io_dims, const std::vector<size_t>& io_data_sizes) :
      Op(), region(region), increment(increment), num_inputs(num_inputs), num_outputs(num_outputs), io_dims(io_dims), io_data_size(io_data_sizes) {
}

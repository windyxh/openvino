// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "core/attribute.hpp"

#include "core/graph.hpp"
#include "core/model.hpp"
#include "ngraph/log.hpp"

namespace ngraph {
namespace onnx_import {
Subgraph Attribute::get_subgraph(const Graph* parent_graph) const {
    if (m_attribute_proto->type() != ONNX_NAMESPACE::AttributeProto_AttributeType_GRAPH) {
        throw error::attribute::InvalidData{m_attribute_proto->type()};
    }

    auto model_proto = std::make_shared<ONNX_NAMESPACE::ModelProto>();

    const auto& graph = m_attribute_proto->g();
    model_proto->mutable_graph()->CopyFrom(graph);

    // set opset version and domain from the parent graph
    model_proto->mutable_opset_import()->CopyFrom(parent_graph->get_opset_imports());
    return Subgraph{model_proto, parent_graph};
}

ov::Any Attribute::get_any() const {
    switch (get_type()) {
    case Type::float_point:
        // OV has automatic downcasting of node attributes:
        // double -> float
        // but upcasting is not supported:
        // float -> double
        // so float value from protobuf leads to the issue
        // when we are trying to get an attribute of double type in ov::Node
        return static_cast<double>(get_float());
    case Type::integer:
        return get_integer();
    case Type::string:
        return get_string();
    case Type::float_point_array: {
        auto float_array = get_float_array();
        // OV has automatic downcasting of node attributes:
        // double -> float
        // but upcasting is not supported:
        // float -> double
        // so float value from protobuf leads to the issue
        // when we are trying to get an attribute of double type in ov::Node
        std::vector<double> double_array(float_array.size());
        for (size_t i = 0; i < float_array.size(); ++i) {
            double_array[i] = static_cast<double>(float_array[i]);
        }
        return double_array;
    }
    case Type::integer_array:
        return get_integer_array();
    case Type::string_array:
        return get_string_array();
    // TODO: support attributes.
    case Type::sparse_tensor_array:
    case Type::graph_array:
    case Type::tensor_array:
    case Type::tensor:
    case Type::graph:
    case Type::sparse_tensor:
        throw ov::Exception(get_name() + " attribute is not supported.");
    default:
        throw ov::Exception("Unknown type of attribute " + get_name());
    }
}

}  // namespace onnx_import

}  // namespace ngraph

// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <openvino/frontend/exception.hpp>
#include <openvino/frontend/manager.hpp>

#include "test_common.hpp"
#include "tf_utils.hpp"
#include "utils.hpp"

using namespace std;
using namespace ov;
using namespace ov::frontend;

namespace {
shared_ptr<Model> convert_model(const string& model_path) {
    FrontEndManager fem;
    auto front_end = fem.load_by_framework(TF_FE);
    if (!front_end) {
        throw "TensorFlow Frontend is not initialized";
    }
    auto model_filename = FrontEndTestUtils::make_model_path(string(TEST_TENSORFLOW_MODELS_DIRNAME) + model_path);
    auto input_model = front_end->load(model_filename);
    if (!input_model) {
        throw "Input model is not read";
    }
    auto model = front_end->convert(input_model);
    if (!model) {
        throw "Model is not converted";
    }

    return model;
}
}  // namespace

TEST(FrontEndConvertTrickyModels, undefined_input_shape) {
    shared_ptr<Model> model;
    try {
        model = convert_model("undefined_input_shape/undefined_input_shape.pb");
    } catch (std::exception& ex) {
        ASSERT_TRUE(false) << ex.what();
    }

    for (auto& node : model->get_ordered_ops()) {
        if (node->get_friendly_name() == "x") {
            ASSERT_TRUE(node->get_output_partial_shape(0).same_scheme(ov::PartialShape::dynamic()));
        } else if (node->get_friendly_name() == "y") {
            ASSERT_TRUE(node->get_output_partial_shape(0).same_scheme(ov::PartialShape{2, 3}));
        } else if (node->get_friendly_name() == "z") {
            ASSERT_TRUE(node->get_output_partial_shape(0).same_scheme(ov::PartialShape::dynamic()));
        }
    }
}

TEST(FrontEndConvertTrickyModels, model_with_output_shapes) {
    shared_ptr<Model> model;
    try {
        model = convert_model("model_with_output_shapes_attr/model_with_output_shapes_attr.pb");
    } catch (std::exception& ex) {
        ASSERT_TRUE(false) << ex.what();
    }

    for (auto& node : model->get_ordered_ops()) {
        if (node->get_friendly_name() == "x") {
            ASSERT_TRUE(node->get_output_partial_shape(0).same_scheme(ov::PartialShape{2, 3}));
        } else if (node->get_friendly_name() == "relu") {
            ASSERT_TRUE(node->get_output_partial_shape(0).same_scheme(ov::PartialShape{2, 3}));
        }
    }
}

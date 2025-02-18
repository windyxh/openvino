// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <intel_gpu/primitives/input_layout.hpp>
#include <intel_gpu/primitives/multiclass_nms.hpp>
#include <intel_gpu/primitives/mutable_data.hpp>

#include "test_utils.h"

using namespace cldnn;
using namespace ::tests;

namespace {
template<typename T>
std::vector<T> getValues(const std::vector<float>& values) {
    std::vector<T> result(values.begin(), values.end());
    return result;
}

template<typename T>
float getError();

template<>
float getError<float>() {
    return 0.001;
}

template<>
float getError<half_t>() {
    return 0.2;
}

template<typename T, typename T_IND>
struct MulticlassNmsParams {
    std::string test_name;

    cldnn::multiclass_nms::sort_result_type sort_result_type;
    bool sort_result_across_batch;
    float iou_threshold;
    float score_threshold;
    int nms_top_k;
    int keep_top_k;
    int background_class;
    bool normalized;
    float nms_eta;
    bool has_roisnum;

    size_t num_batches;
    size_t num_classes;
    size_t num_boxes;

    std::vector<T> boxes;
    std::vector<T> scores;
    std::vector<T_IND> roisnum;

    std::vector<T> expected_selected_outputs;
    std::vector<T_IND> expected_selected_indices;
    std::vector<T_IND> expected_selected_num;
};

template<typename T, typename T_IND>
struct multiclass_nms_test : public ::testing::TestWithParam<MulticlassNmsParams<T, T_IND>> {
public:
    void test(const std::vector<format::type>& formats = {format::bfyx}) {
        const MulticlassNmsParams<T, T_IND> param = testing::TestWithParam<MulticlassNmsParams<T, T_IND>>::GetParam();
        auto data_type = type_to_data_type<T>::value;
        auto index_data_type = type_to_data_type<T_IND>::value;
        constexpr auto plain_format = format::bfyx;

        for (const auto target_format : formats) {

            auto& engine = get_test_engine();

            const auto input_boxes =
                    !param.has_roisnum
                    ? engine.allocate_memory({data_type,
                                              plain_format,
                                              tensor{batch(param.num_batches), feature(param.num_boxes),
                                                     spatial(1, 4)}})
                    : engine.allocate_memory({data_type,
                                              plain_format,
                                              tensor{batch(param.num_classes), feature(param.num_boxes),
                                                     spatial(1, 4)}});
            set_values(input_boxes, param.boxes);

            const auto input_scores =
                    !param.has_roisnum
                    ? engine.allocate_memory(
                            {data_type,
                             plain_format,
                             tensor{batch(param.num_batches), feature(param.num_classes),
                                    spatial(1, param.num_boxes)}})
                    : engine.allocate_memory(
                            {data_type, plain_format, tensor{batch(param.num_classes), feature(param.num_boxes)}});
            set_values(input_scores, param.scores);

            const auto input_roisnum =
                    param.has_roisnum
                    ? engine.allocate_memory({index_data_type, plain_format, tensor{batch(param.num_batches)}})
                    : nullptr;
            if (input_roisnum) {
                set_values(input_roisnum, param.roisnum);
            }

            auto real_num_classes = param.num_classes;
            if (param.background_class >= 0 && static_cast<size_t>(param.background_class) < param.num_classes) {
                real_num_classes = std::max(static_cast<size_t>(1), param.num_classes - 1);
            }
            int64_t max_output_boxes_per_class = 0;
            if (param.nms_top_k >= 0)
                max_output_boxes_per_class = std::min(static_cast<int>(param.num_boxes), param.nms_top_k);
            else
                max_output_boxes_per_class = param.num_boxes;

            auto max_output_boxes_per_batch = max_output_boxes_per_class * real_num_classes;
            if (param.keep_top_k >= 0)
                max_output_boxes_per_batch = std::min(static_cast<int>(max_output_boxes_per_batch), param.keep_top_k);

            const auto dim = max_output_boxes_per_batch * param.num_batches;

            const layout output_selected_indices_layout{index_data_type, target_format,
                                                        tensor{batch(dim), feature(1)}};
            const auto output_selected_indices = engine.allocate_memory(output_selected_indices_layout);
            const layout output_selected_num_layout{index_data_type, target_format,
                                                    tensor{batch(param.num_batches)}};
            const auto output_selected_num = engine.allocate_memory(output_selected_num_layout);

            topology topology;

            topology.add(input_layout("input_boxes", input_boxes->get_layout()));
            topology.add(input_layout("input_scores", input_scores->get_layout()));
            if (param.has_roisnum) {
                topology.add(input_layout("input_roisnum", input_roisnum->get_layout()));
            }

            topology.add(mutable_data("output_selected_indices", output_selected_indices));
            topology.add(mutable_data("output_selected_num", output_selected_num));

            topology.add(reorder("input_boxes_reordered", input_info("input_boxes"), target_format, data_type));
            topology.add(reorder("input_scores_reordered", input_info("input_scores"), target_format, data_type));
            if (param.has_roisnum) {
                topology.add(reorder("input_roisnum_reordered", input_info("input_roisnum"), target_format, index_data_type));
            }

            const auto primitive = multiclass_nms{
                    "multiclass_nms_reordered",
                    std::vector<cldnn::input_info>{
                        input_info("input_boxes_reordered"),
                        input_info("input_scores_reordered"),
                        param.has_roisnum ? input_info("input_roisnum_reordered") : input_info(""),
                        input_info("output_selected_indices"),
                        input_info("output_selected_num")
                    },
                    cldnn::multiclass_nms::attributes{
                        param.sort_result_type,
                        param.sort_result_across_batch,
                        index_data_type,
                        param.iou_threshold,
                        param.score_threshold,
                        param.nms_top_k,
                        param.keep_top_k,
                        param.background_class,
                        param.normalized,
                        param.nms_eta,
                    }
            };

            topology.add(primitive);
            topology.add(reorder("multiclass_nms", input_info("multiclass_nms_reordered"), plain_format, data_type));
            build_options bo;
            bo.set_option(build_option::optimize_data(false));
            network network(engine, topology, bo);

            network.set_input_data("input_boxes", input_boxes);
            network.set_input_data("input_scores", input_scores);
            if (param.has_roisnum) {
                network.set_input_data("input_roisnum", input_roisnum);
            }

            const auto outputs = network.execute();

            const auto output_boxes = outputs.at("multiclass_nms").get_memory();
            const cldnn::mem_lock<T> output_boxes_ptr(output_boxes, get_test_stream());
            ASSERT_EQ(output_boxes_ptr.size(), dim * 6) << "format=" << fmt_to_str(target_format);

            const auto get_plane_data = [&](const memory::ptr& mem, const data_types data_type,
                                            const layout& from_layout) {
                if (from_layout.format == plain_format) {
                    return mem;
                }
                cldnn::topology reorder_topology;
                reorder_topology.add(input_layout("data", from_layout));
                reorder_topology.add(reorder("plane_data", input_info("data"), plain_format, data_type));
                cldnn::network reorder_net{engine, reorder_topology};
                reorder_net.set_input_data("data", mem);
                const auto second_output_result = reorder_net.execute();
                const auto plane_data_mem = second_output_result.at("plane_data").get_memory();
                return plane_data_mem;
            };

            const cldnn::mem_lock<T_IND> output_selected_indices_ptr(
                    get_plane_data(output_selected_indices, index_data_type, output_selected_indices_layout),
                    get_test_stream());
            ASSERT_EQ(output_selected_indices_ptr.size(), dim) << "format=" << fmt_to_str(target_format);

            const cldnn::mem_lock<T_IND> output_selected_num_ptr(
                    get_plane_data(output_selected_num, index_data_type, output_selected_num_layout),
                    get_test_stream());
            ASSERT_EQ(output_selected_num_ptr.size(), param.num_batches) << "format=" << fmt_to_str(target_format);

            for (size_t i = 0; i < param.num_batches; ++i) {
                EXPECT_EQ(param.expected_selected_num[i], output_selected_num_ptr[i])
                                    << "format=" << fmt_to_str(target_format) << " i=" << i;
            }

            for (size_t box = 0; box < dim; ++box) {
                EXPECT_EQ(param.expected_selected_indices[box], output_selected_indices_ptr[box]) << "box=" << box;

                for (size_t j = 0; j < 6; ++j) {
                    const auto idx = box * 6 + j;
                    EXPECT_NEAR(param.expected_selected_outputs[idx], output_boxes_ptr[idx], getError<T>())
                                        << "format=" << fmt_to_str(target_format) << " box=" << box << ", j=" << j;
                }
            }
        }
    }
};

struct PrintToStringParamName {
    template<class T, class T_IND>
    std::string operator()(const testing::TestParamInfo<MulticlassNmsParams<T, T_IND>> &info) {
        const auto &p = info.param;
        std::ostringstream result;
        result << p.test_name << "_";
        result << "InputType=" << data_type_traits::name(type_to_data_type<T_IND>::value) << "_";
        result << "DataType=" << data_type_traits::name(type_to_data_type<T>::value);
        return result.str();
    }
};

using multiclass_nms_test_f32_i32 = multiclass_nms_test<float, int32_t>;
using multiclass_nms_test_f16_i64 = multiclass_nms_test<half_t, int64_t>;
using multiclass_nms_test_blocked = multiclass_nms_test<float, int32_t>;

TEST_P(multiclass_nms_test_f32_i32, basic) {
    ASSERT_NO_FATAL_FAILURE(test());
}

TEST_P(multiclass_nms_test_f16_i64, basic) {
    ASSERT_NO_FATAL_FAILURE(test());
}

TEST_P(multiclass_nms_test_blocked, basic) {
    const std::vector<format::type> formats = {
            format::bfyx,
            format::b_fs_yx_fsv16,
            format::b_fs_yx_fsv32,
            format::bs_fs_yx_bsv16_fsv16,
            format::bs_fs_yx_bsv16_fsv32,
            format::bs_fs_yx_bsv32_fsv16,
            format::bs_fs_yx_bsv32_fsv32
    };

    ASSERT_NO_FATAL_FAILURE(test(formats));
}

template<typename T, typename T_IND>
std::vector<MulticlassNmsParams<T, T_IND>> getMulticlassNmsParams() {
    std::vector<MulticlassNmsParams<T, T_IND>> params = {
        {"by_score",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.5f,
         0.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         1,
         2,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
         std::vector<T_IND>{},
         getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00, 1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                       0.00, 0.90, 0.00, 0.00, 1.00, 1.00, 1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
         std::vector<T_IND>{3, 0, 0, 3, -1, -1},
         std::vector<T_IND>{4}},

        {"by_class_id",
         cldnn::multiclass_nms::sort_result_type::classid,
          false, 0.5f, 0.0f, 3, -1, -1, true, 1.0f, false,
          1, 2, 6,
            getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                          0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
            getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
            std::vector<T_IND>{},
            getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00, 0.00, 0.90, 0.00, 0.00, 1.00, 1.00,
                          1.00, 0.95, 0.00, 0.00, 1.00, 1.00, 1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                          -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
            std::vector<T_IND>{3, 0, 0, 3, -1, -1},
            std::vector<T_IND>{4}},

        {"three_inputs",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.5f,
         0.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         true,

         2,
         2,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                       0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0,
                       1.1,
                       0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0,
                       101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
         std::vector<T_IND>{1, 1},

         getValues<T>({1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                       0.00, 0.90, 0.00, 0.00, 1.00, 1.00,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                       0.0, 0.75, 0.0, 0.1, 1.0, 1.1,
                       1.0, 0.75, 0.0, 0.1, 1.0, 1.1,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
        std::vector<T_IND>{1, 0, -1, -1, -1, -1,
                           2, 3, -1, -1, -1, -1},
        std::vector<T_IND>{2, 2}},

        {"across_batches_by_score",
         cldnn::multiclass_nms::sort_result_type::score,
         true,
         0.5f,
         0.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         2,
         2,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                       0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1,
                       0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
        getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3,
                      0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
        std::vector<T_IND>{},

        getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00,
                      1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                      1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                      0.00, 0.90, 0.00, 0.00, 1.00, 1.00,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                      1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                      1.0, 0.5, 0.0, 10.1, 1.0, 11.1,
                      1.0, 0.3, 0.0, 100.0, 1.0, 101.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
        std::vector<T_IND>{3, 0, 6, 0, -1, -1, 3, 9, 4, 5, -1, -1},
        std::vector<T_IND>{4, 4}},

        {"across_batches_by_class_id",
         cldnn::multiclass_nms::sort_result_type::classid,
         true,
         0.5f,
         0.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         2,
         2,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                       0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1,
                       0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3,
                       0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
         std::vector<T_IND>{},

         getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00,
                      0.00, 0.90, 0.00, 0.00, 1.00, 1.00,
                      1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                      1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,

                      1.0, 0.5, 0.0, 10.1, 1.0, 11.1,
                      1.0, 0.3, 0.0, 100.0, 1.0, 101.0,
                      1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                      1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
         std::vector<T_IND>{3, 0, 0, 3, -1, -1, 4, 5, 6, 9, -1, -1},
         std::vector<T_IND>{4, 4}},

        {"normalized",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.5f,
         0.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         1,
         1,
         6,

         getValues<T>({1.0, 1.0, 0.0, 0.0, 0.0, 0.1, 1.0, 1.1, 0.0, 0.9, 1.0, -0.1,
                       0.0, 10.0, 1.0, 11.0, 1.0, 10.1, 0.0, 11.1, 1.0, 101.0, 0.0, 100.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3}),
         std::vector<T_IND>{},

         getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00, 0.00, 0.90, 1.00,
                       1.00, 0.00, 0.00, 0.00, 0.75, 0.00, 0.10, 1.00, 1.10}),
         std::vector<T_IND>{3, 0, 1},
         std::vector<T_IND>{3}},

        {"identical_boxes",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.5f,
         0.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         1,
         1,
         10,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0,
                       1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0,
                       0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0}),
         getValues<T>({0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9}),
         std::vector<T_IND>{},

         getValues<T>({0.00, 0.90, 0.00, 0.00, 1.00, 1.00,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
        std::vector<T_IND>{0, -1, -1},
        std::vector<T_IND>{1}},

        {"limit_output_size",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.5f,
         0.0f,
         2,
         -1,
         -1,
         true,
         1.0f,
         false,

         1,
         1,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3}),
         std::vector<T_IND>{},
         getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00, 0.00, 0.90, 0.00, 0.00, 1.00, 1.00}),
         std::vector<T_IND>{3, 0},
         std::vector<T_IND>{2}},

        {"single_box",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.5f,
         0.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         1,
         1,
         1,

         getValues<T>({0.0, 0.0, 1.0, 1.0}),
         getValues<T>({0.9}),
         std::vector<T_IND>{},

         getValues<T>({0.00, 0.90, 0.00, 0.00, 1.00, 1.00}),
         std::vector<T_IND>{0},
         std::vector<T_IND>{1}},

        {"iou_threshold",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.2f,
         0.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         1,
         1,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3}),
         std::vector<T_IND>{},

         getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00, 0.00, 0.90, 0.00,
                       0.00, 1.00, 1.00, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
        std::vector<T_IND>{3, 0, -1},
        std::vector<T_IND>{2}},

        {"iou_and_score_thresholds",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.5f,
         0.95f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         1,
         1,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.96, 0.5, 0.3}),
         std::vector<T_IND>{},

         getValues<T>({0.00, 0.96, 0.00, 10.00, 1.00, 11.00, -1.0, -1.0, -1.0,
                       -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
         std::vector<T_IND>{3, -1, -1},
         std::vector<T_IND>{1}},

        {"no_output",
         cldnn::multiclass_nms::sort_result_type::score,
         false,
         0.5f,
         2.0f,
         3,
         -1,
         -1,
         true,
         1.0f,
         false,

         1,
         1,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3}),
         std::vector<T_IND>{},

         getValues<T>({-1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
         std::vector<T_IND>{-1, -1, -1, -1, -1, -1},
         std::vector<T_IND>{0}},

        {"background_class",
         cldnn::multiclass_nms::sort_result_type::classid,
         false,
         0.5f,
         0.0f,
         3,
         -1,
         0,  // background class
         true,
         1.0f,
         false,

         2,
         2,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0,
                       0.0, 0.1, 1.0, 1.1,
                       0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0,
                       0.0, 10.1, 1.0, 11.1,
                       0.0, 100.0, 1.0, 101.0,
                       0.0, 0.0, 1.0, 1.0,
                       0.0, 0.1, 1.0, 1.1,
                       0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0,
                       0.0, 10.1, 1.0, 11.1,
                       0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3,
                       0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
         std::vector<T_IND>{},

        getValues<T>({1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                      1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                      1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
        std::vector<T_IND>{0, 3, -1, 6, 9, -1},
        std::vector<T_IND>{2, 2}},

        {"keep_top_k",
         cldnn::multiclass_nms::sort_result_type::classid,
         false,
         0.5f,
         0.0f,
         3,
         3,
         -1,
         true,
         1.0f,
         false,

         2,
         2,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0,
                       0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3,
                       0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
         std::vector<T_IND>{},

         getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00,
                       0.00, 0.90, 0.00, 0.00, 1.00, 1.00,
                       1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                       1.00, 0.5, 0.00, 10.1, 1.00, 11.1,
                       1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                       1.00, 0.80, 0.00, 10.00, 1.00, 11.00}),
        std::vector<T_IND>{3, 0, 0, 4, 6, 9},
        std::vector<T_IND>{3, 3}},

        {"normalized_by_classid",
         cldnn::multiclass_nms::sort_result_type::classid,
         false,
         1.0f,
         0.0f,
         -1,
         -1,
         -1,
         true,
         0.1f,
         false,

         2,
         2,
         6,

         getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0,
                       0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9,
                       0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0}),
         getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3,
                       0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
         std::vector<T_IND>{},

         getValues<T>({0.00, 0.95, 0.00, 10.00, 1.00, 11.00,
                      0.00, 0.90, 0.00, 0.00, 1.00, 1.00,
                      0.00, 0.30, 0.00, 100.00, 1.00, 101.00,
                      1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                      1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                      1.00, 0.30, 0.00, 100.00, 1.00, 101.00,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      1.0, 0.6, 0.0, -0.1, 1.0, 0.9,
                      1.0, 0.5, 0.0, 10.1, 1.0, 11.1,
                      1.00, 0.30, 0.00, 100.00, 1.00, 101.00,
                      1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                      1.00, 0.80, 0.00, 10.00, 1.00, 11.00,
                      1.00, 0.30, 0.00, 100.00, 1.00, 101.00,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                      -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}),
         std::vector<T_IND>{3, 0, 5, 0, 3, 5,
                            -1, -1, -1, -1, -1, -1,
                            2, 4, 5, 6, 9, 11,
                            -1, -1, -1, -1, -1, -1},
         std::vector<T_IND>{6, 6}},
    };

    return params;
}

template<typename T, typename T_IND>
std::vector<MulticlassNmsParams<T, T_IND>> getParamsForBlockedLayout() {
    MulticlassNmsParams<T, T_IND> param = {
        "blocked_format_three_inputs",
        cldnn::multiclass_nms::sort_result_type::score,
        false,
        0.5f,
        0.0f,
        3,
        -1,
        -1,
        true,
        1.0f,
        true,

        34, //batches
        2,  //classes
        6,  //boxes

        getValues<T>({0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                      0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1,
                      0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0,
                      0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                      0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1,
                      0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0,
                      0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                      0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                      0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                      0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1,
                      0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0,
                      0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                      0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1,
                      0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0, 0.0, 10.1, 1.0, 11.1, 0.0, 100.0, 1.0, 101.0,
                      0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                      0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                      0.0, 0.0, 1.0, 1.0, 0.0, 0.1, 1.0, 1.1, 0.0, -0.1, 1.0, 0.9, 0.0, 10.0, 1.0, 11.0,
                     }),
        getValues<T>({0.9, 0.75, 0.6, 0.95, 0.5, 0.3, 0.95, 0.75, 0.6, 0.80, 0.5, 0.3}),
        std::vector<T_IND>{1, 1},

        getValues<T>({
                             1.00, 0.95, 0.00, 0.00, 1.00, 1.00,
                             0.00, 0.90, 0.00, 0.00, 1.00, 1.00,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                             0.0, 0.75, 0.0, 0.1, 1.0, 1.1,
                             1.0, 0.75, 0.0, 0.1, 1.0, 1.1,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                             -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
                     }),
        std::vector<T_IND>{1, 0, -1, -1, -1, -1,
                           2, 3, -1, -1, -1, -1},
        std::vector<T_IND>{2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    const auto indices_size = param.num_batches * param.num_boxes;
    const auto filled_indices = param.expected_selected_indices.size();
    param.expected_selected_indices.resize(indices_size);
    for (auto i = filled_indices; i < indices_size; ++i) {
        param.expected_selected_indices[i] = -1;
    }

    const auto outputs_size = param.num_batches * param.num_classes * param.num_boxes * 6;
    const auto filled_outputs = param.expected_selected_outputs.size();
    param.expected_selected_outputs.resize(outputs_size);
    for (auto i = filled_outputs; i < outputs_size; ++i) {
        param.expected_selected_outputs[i] = -1.0;
    }

    return {param};
}

INSTANTIATE_TEST_SUITE_P(multiclass_nms_gpu_test,
                     multiclass_nms_test_f32_i32,
                     ::testing::ValuesIn(getMulticlassNmsParams<float, int32_t>()),
                     PrintToStringParamName());

INSTANTIATE_TEST_SUITE_P(multiclass_nms_gpu_test,
                     multiclass_nms_test_f16_i64,
                     ::testing::ValuesIn(getMulticlassNmsParams<half_t, int64_t>()),
                     PrintToStringParamName());

INSTANTIATE_TEST_SUITE_P(multiclass_nms_gpu_test_blocked,
                     multiclass_nms_test_f32_i32,
                     ::testing::ValuesIn(getParamsForBlockedLayout<float, int32_t>()),
                     PrintToStringParamName());

};  // namespace

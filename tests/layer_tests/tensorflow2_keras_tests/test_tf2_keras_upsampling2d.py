# Copyright (C) 2022-2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

import pytest
import tensorflow as tf

from common.tf2_layer_test_class import CommonTF2LayerTest


class TestKerasUpSampling2D(CommonTF2LayerTest):
    def create_keras_upsampling2d_net(self, input_names, input_shapes, input_type, size,
                                      data_format, interpolation,
                                      ir_version):
        # create TensorFlow 2 model with Keras UpSampling2D operation
        tf.keras.backend.clear_session()
        x1 = tf.keras.Input(shape=input_shapes[0][1:], name=input_names[0], dtype=input_type)
        y = tf.keras.layers.UpSampling2D(size=size, data_format=data_format,
                                         interpolation=interpolation)(x1)
        tf2_net = tf.keras.Model(inputs=[x1], outputs=[y])

        # TODO: create a reference net. Now it is ommitted and tests only inference since it is more important
        ref_net = None

        return tf2_net, ref_net

    # Tests for nearest interpolation
    test_data_nearest = [
        dict(input_names=["x1"], input_shapes=[[5, 4, 8, 2]], input_type=tf.float32,
             size=(2, 3), data_format='channels_last', interpolation='nearest'),
        dict(input_names=["x1"], input_shapes=[[3, 2, 4, 2]], input_type=tf.float32,
             size=(3, 1), data_format='channels_last', interpolation='nearest'),
        dict(input_names=["x1"], input_shapes=[[1, 3, 8, 6]], input_type=tf.float32,
             size=(5, 2), data_format='channels_last', interpolation='nearest'),
    ]

    @pytest.mark.parametrize("params", test_data_nearest)
    @pytest.mark.nightly
    @pytest.mark.precommit
    def test_keras_upsampling2d_nearest(self, params, ie_device, precision, ir_version, temp_dir,
                                        use_old_api, use_new_frontend):
        self._test(*self.create_keras_upsampling2d_net(**params, ir_version=ir_version),
                   ie_device, precision, temp_dir=temp_dir, use_old_api=use_old_api, ir_version=ir_version,
                   use_new_frontend=use_new_frontend, **params)

    # Tests for bilinear interpolation
    test_data_bilinear = [
        pytest.param(dict(input_names=["x1"], input_shapes=[[1, 6, 2, 1]], input_type=tf.float32,
                          size=(3, 1), data_format='channels_last', interpolation='bilinear'),
                     marks=pytest.mark.precommit_tf_fe),
        dict(input_names=["x1"], input_shapes=[[1, 3, 1, 6]], input_type=tf.float32,
             size=(5, 2), data_format='channels_last', interpolation='bilinear'),
    ]

    @pytest.mark.parametrize("params", test_data_bilinear)
    @pytest.mark.nightly
    def test_keras_upsampling2d_bilinear(self, params, ie_device, precision, ir_version, temp_dir,
                                         use_old_api, use_new_frontend):
        self._test(*self.create_keras_upsampling2d_net(**params, ir_version=ir_version),
                   ie_device, precision, temp_dir=temp_dir, use_old_api=use_old_api, ir_version=ir_version,
                   use_new_frontend=use_new_frontend, **params)

    test_data_channels_first = [
        dict(input_names=["x1"], input_shapes=[[5, 4, 5, 3]], input_type=tf.float32,
             size=(3, 4), data_format='channels_first', interpolation='nearest'),
        dict(input_names=["x1"], input_shapes=[[3, 2, 7, 2]], input_type=tf.float32,
             size=(2, 3), data_format='channels_first', interpolation='nearest'),
        dict(input_names=["x1"], input_shapes=[[3, 5, 4, 6]], input_type=tf.float32,
             size=(5, 2), data_format='channels_first', interpolation='nearest'),
    ]

    @pytest.mark.parametrize("params", test_data_channels_first)
    @pytest.mark.nightly
    def test_keras_upsampling2d_channels_first(self, params, ie_device, precision, ir_version,
                                               temp_dir, use_old_api, use_new_frontend):
        self._test(*self.create_keras_upsampling2d_net(**params, ir_version=ir_version),
                   ie_device, precision, temp_dir=temp_dir, use_old_api=use_old_api, ir_version=ir_version,
                   use_new_frontend=use_new_frontend, **params)

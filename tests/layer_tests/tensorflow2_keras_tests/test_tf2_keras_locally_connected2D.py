# Copyright (C) 2022-2023 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

import pytest
import tensorflow as tf

from common.tf2_layer_test_class import CommonTF2LayerTest


class TestKerasLocallyConnected2D(CommonTF2LayerTest):
    def create_keras_locally_connected2D_net(self, input_names, input_shapes, input_type, filters,
                                             kernel_size, strides,
                                             padding, data_format, activation, use_bias, ir_version,
                                             impl=1):
        """
                create TensorFlow 2 model with Keras LocallyConnected2D operation
        """
        tf.keras.backend.clear_session()  # For easy reset of notebook state
        x1 = tf.keras.Input(shape=input_shapes[0][1:], name=input_names[0])
        y = tf.keras.layers.LocallyConnected2D(filters, kernel_size, strides=strides,
                                               padding=padding,
                                               data_format=data_format, activation=activation,
                                               use_bias=use_bias,
                                               implementation=impl)(x1)
        tf2_net = tf.keras.Model(inputs=x1, outputs=[y])

        # TODO: add reference IR net. Now it is omitted since inference is more
        #  important and needs to be checked in the first
        ref_net = None

        return tf2_net, ref_net

    test_data_simple_channel_last = [
        dict(input_names=["x"], input_shapes=[[1, 4, 3, 2]], input_type=tf.float32, filters=2,
             kernel_size=(3, 2),
             strides=(2, 2), padding='valid', data_format='channels_last', activation="linear",
             use_bias=False),
        dict(input_names=["x"], input_shapes=[[2, 4, 4, 4]], input_type=tf.float32, filters=4,
             kernel_size=(2, 1),
             strides=(2, 2), padding='valid', data_format='channels_last', activation='relu',
             use_bias=True),
        dict(input_names=["x"], input_shapes=[[4, 4, 8, 6]], input_type=tf.float32, filters=5,
             kernel_size=(4, 2),
             strides=(2, 2), padding='valid', data_format='channels_last', activation='tanh',
             use_bias=False),
    ]

    @pytest.mark.parametrize("params", test_data_simple_channel_last)
    @pytest.mark.nightly
    @pytest.mark.precommit
    def test_keras_locally_connected2D_float32(self, params, ie_device, precision, temp_dir,
                                               ir_version, use_old_api, use_new_frontend):
        self._test(*self.create_keras_locally_connected2D_net(**params, ir_version=ir_version),
                   ie_device, precision, temp_dir=temp_dir, use_old_api=use_old_api, ir_version=ir_version,
                   use_new_frontend=use_new_frontend, **params)

    test_data_simple_channels_first = [
        dict(input_names=["x"], input_shapes=[[5, 7, 7, 4]], input_type=tf.float32, filters=3,
             kernel_size=(4, 4),
             strides=(2, 4), padding='valid', data_format='channels_first', activation='softmax',
             use_bias=True),
        dict(input_names=["x"], input_shapes=[[5, 2, 5, 4]], input_type=tf.float32, filters=4,
             kernel_size=(3, 2),
             strides=(1, 1), padding='valid', data_format='channels_first', activation='elu',
             use_bias=False),
        dict(input_names=["x"], input_shapes=[[5, 2, 2, 25]], input_type=tf.float32, filters=2,
             kernel_size=(1, 5),
             strides=(2, 10), padding='valid', data_format='channels_first', activation='sigmoid',
             use_bias=True),
    ]

    @pytest.mark.parametrize("params", test_data_simple_channels_first)
    @pytest.mark.nightly
    @pytest.mark.precommit
    def test_keras_locally_connected2D_channels_first_float32(self, params, ie_device, precision,
                                                              temp_dir, ir_version, use_old_api,
                                                              use_new_frontend):
        self._test(*self.create_keras_locally_connected2D_net(**params, ir_version=ir_version),
                   ie_device, precision, temp_dir=temp_dir, use_old_api=use_old_api, ir_version=ir_version,
                   use_new_frontend=use_new_frontend, **params)

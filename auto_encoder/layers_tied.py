#downloaded from: 
#https://gist.github.com/dswah/c6b3e4d47d933b057aab32c9c29c4221
'''
    This file contains layers that allow the sharing of weights between layers. 
    It is useful, when it is desired to have a layer that performs the inverse 
    operation of another layer. In case of an autoencoder this can be desirable
    for example.
'''

from keras import backend as K
from keras import activations, initializers, regularizers, constraints
from keras.engine import Layer, InputSpec
from keras.utils.conv_utils import conv_output_length
from keras.layers import Convolution1D, Convolution2D
import tensorflow as tf

class Convolution1D_tied(Layer):
    '''Convolution operator for filtering neighborhoods of one-dimensional inputs.
    When using this layer as the first layer in a model,
    either provide the keyword argument `input_dim`
    (int, e.g. 128 for sequences of 128-dimensional vectors),
    or `input_shape` (tuple of integers, e.g. (10, 128) for sequences
    of 10 vectors of 128-dimensional vectors).
    # Example
    ```python
        # apply a convolution 1d of length 3 to a sequence with 10 timesteps,
        # with 64 output filters
        model = Sequential()
        model.add(Convolution1D(64, 3, padding='same', input_shape=(10, 32)))
        # now model.output_shape == (None, 10, 64)
        # add a new conv1d on top
        model.add(Convolution1D(32, 3, padding='same'))
        # now model.output_shape == (None, 10, 32)
    ```
    # Arguments
        nb_filter: Number of convolution kernels to use
            (dimensionality of the output).
        filter_length: The extension (spatial or temporal) of each filter.
        init: name of initialization function for the weights of the layer
            (see [initializations](../initializations.md)),
            or alternatively, Theano function to use for weights initialization.
            This parameter is only relevant if you don't pass a `weights` argument.
        activation: name of activation function to use
            (see [activations](../activations.md)),
            or alternatively, elementwise Theano function.
            If you don't specify anything, no activation is applied
            (ie. "linear" activation: a(x) = x).
        weights: list of numpy arrays to set as initial weights.
        padding: 'valid' or 'same'.
        subsample_length: factor by which to subsample output.
        W_regularizer: instance of [WeightRegularizer](../regularizers.md)
            (eg. L1 or L2 regularization), applied to the main weights matrix.
        b_regularizer: instance of [WeightRegularizer](../regularizers.md),
            applied to the bias.
        activity_regularizer: instance of [ActivityRegularizer](../regularizers.md),
            applied to the network output.
        W_constraint: instance of the [constraints](../constraints.md) module
            (eg. maxnorm, nonneg), applied to the main weights matrix.
        b_constraint: instance of the [constraints](../constraints.md) module,
            applied to the bias.
        bias: whether to include a bias
            (i.e. make the layer affine rather than linear).
        input_dim: Number of channels/dimensions in the input.
            Either this argument or the keyword argument `input_shape`must be
            provided when using this layer as the first layer in a model.
        input_length: Length of input sequences, when it is constant.
            This argument is required if you are going to connect
            `Flatten` then `Dense` layers upstream
            (without it, the shape of the dense outputs cannot be computed).
    # Input shape
        3D tensor with shape: `(samples, steps, input_dim)`.
    # Output shape
        3D tensor with shape: `(samples, new_steps, nb_filter)`.
        `steps` value might have changed due to padding.
    '''
    def __init__(self, nb_filter, filter_length,
                 init='uniform', activation='linear', weights=None,
                 padding='valid', subsample_length=1,
                 W_regularizer=None, b_regularizer=None, activity_regularizer=None,
                 W_constraint=None, b_constraint=None,
                 bias=True, input_dim=None, input_length=None, tied_to=None,
                 **kwargs):

        if padding not in {'valid', 'same'}:
            raise Exception('Invalid border mode for Convolution1D:', padding)

        self.tied_to = tied_to
        self.nb_filter = nb_filter #TODO may have to change this and the one below...
        self.filter_length = tied_to.filter_length
        #self.init = initializers.get(init, data_format='channels_first')
        self.init = initializers.get(init)
        self.activation = activations.get(activation)
        assert padding in {'valid', 'same'}, 'padding must be in {valid, same}'
        self.padding = padding
        self.subsample_length = subsample_length

        self.subsample = (subsample_length, 1)

        self.W_regularizer = regularizers.get(W_regularizer)
        self.b_regularizer = regularizers.get(b_regularizer)
        self.activity_regularizer = regularizers.get(activity_regularizer)

        self.W_constraint = constraints.get(W_constraint)
        self.b_constraint = constraints.get(b_constraint)

        self.bias = bias
        self.input_spec = [InputSpec(ndim=3)]
        self.initial_weights = tied_to._initial_weights
        self.input_dim = input_dim
        self.input_length = input_length
        if self.input_dim:
            kwargs['input_shape'] = (self.input_length, self.input_dim)
        super(Convolution1D_tied, self).__init__(**kwargs)

    def build(self, input_shape):
        # input_dim = input_shape[2]
        # self.W_shape = (self.nb_filter, input_dim, self.filter_length, 1)
        # self.W = self.init(self.W_shape, name='{}_W'.format(self.name))
        if self.bias:
            self.b = K.zeros((self.nb_filter,), name='{}_b'.format(self.name))
            self.trainable_weights = [self.b]
        # else:
        #     self.trainable_weights = [self.W]
        self.regularizers = []
        #
        # if self.W_regularizer:
        #     self.W_regularizer.set_param(self.W)
        #     self.regularizers.append(self.W_regularizer)
        #
        if self.bias and self.b_regularizer:
            self.b_regularizer.set_param(self.b)
            self.regularizers.append(self.b_regularizer)
        #
        # if self.activity_regularizer:
        #     self.activity_regularizer.set_layer(self)
        #     self.regularizers.append(self.activity_regularizer)
        #
        # self.constraints = {}
        # if self.W_constraint:
        #     self.constraints[self.W] = self.W_constraint
        if self.bias and self.b_constraint:
            self.constraints[self.b] = self.b_constraint
        #
        # if self.initial_weights is not None:
        #     self.set_weights(self.initial_weights)
        #     del self.initial_weights

    def get_output_shape_for(self, input_shape):
        length = conv_output_length(input_shape[1],
                                    self.filter_length,
                                    self.padding,
                                    self.subsample[0])
        return (input_shape[0], length, self.nb_filter)

    def call(self, x, mask=None):
        x = K.expand_dims(x, -1)  # add a dimension of the right
        x = K.permute_dimensions(x, (0, 2, 1, 3))

        # TF uses the last dimension as channel dimension,
        # instead of the 2nd one.
        # TH kernel shape: (depth, input_depth, rows, cols)
        # TF kernel shape: (rows, cols, input_depth, depth)

        # for us, we need to switch the rows with the columns?
        W = tf.transpose(self.tied_to.weights, (1, 0, 2, 3))
        output = K.conv2d(x, W, strides=self.subsample,
                          padding=self.padding,
                          data_format='channels_first')
        if self.bias:
            output += K.reshape(self.b, (1, self.nb_filter, 1, 1))
        output = K.squeeze(output, 3)  # remove the dummy 3rd dimension
        output = K.permute_dimensions(output, (0, 2, 1))
        output = self.activation(output)
        return output

    def get_config(self):
        config = {'nb_filter': self.nb_filter,
                  'filter_length': self.filter_length,
                  'init': self.init.__name__,
                  'activation': self.activation.__name__,
                  'padding': self.padding,
                  'subsample_length': self.subsample_length,
                  'W_regularizer': self.W_regularizer.get_config() if self.W_regularizer else None,
                  'b_regularizer': self.b_regularizer.get_config() if self.b_regularizer else None,
                  'activity_regularizer': self.activity_regularizer.get_config() if self.activity_regularizer else None,
                  'W_constraint': self.W_constraint.get_config() if self.W_constraint else None,
                  'b_constraint': self.b_constraint.get_config() if self.b_constraint else None,
                  'bias': self.bias,
                  'input_dim': self.input_dim,
                  'input_length': self.input_length}
        base_config = super(Convolution1D_tied, self).get_config()
        return dict(list(base_config.items()) + list(config.items()))




class Convolution2D_tied(Layer):
    '''Convolution operator for filtering windows of two-dimensional inputs.
    When using this layer as the first layer in a model,
    provide the keyword argument `input_shape`
    (tuple of integers, does not include the sample axis),
    e.g. `input_shape=(3, 128, 128)` for 128x128 RGB pictures.
    # Examples
    ```python
        # apply a 3x3 convolution with 64 output filters on a 256x256 image:
        model = Sequential()
        model.add(Convolution2D(64, 3, 3, padding='same', input_shape=(3, 256, 256)))
        # now model.output_shape == (None, 64, 256, 256)
        # add a 3x3 convolution on top, with 32 output filters:
        model.add(Convolution2D(32, 3, 3, padding='same'))
        # now model.output_shape == (None, 32, 256, 256)
    ```
    # Arguments
        nb_filter: Number of convolution filters to use.
        nb_row: Number of rows in the convolution kernel.
        nb_col: Number of columns in the convolution kernel.
        init: name of initialization function for the weights of the layer
            (see [initializations](../initializations.md)), or alternatively,
            Theano function to use for weights initialization.
            This parameter is only relevant if you don't pass
            a `weights` argument.
        activation: name of activation function to use
            (see [activations](../activations.md)),
            or alternatively, elementwise Theano function.
            If you don't specify anything, no activation is applied
            (ie. "linear" activation: a(x) = x).
        weights: list of numpy arrays to set as initial weights.
        padding: 'valid' or 'same'.
        subsample: tuple of length 2. Factor by which to subsample output.
            Also called strides elsewhere.
        W_regularizer: instance of [WeightRegularizer](../regularizers.md)
            (eg. L1 or L2 regularization), applied to the main weights matrix.
        b_regularizer: instance of [WeightRegularizer](../regularizers.md),
            applied to the bias.
        activity_regularizer: instance of [ActivityRegularizer](../regularizers.md),
            applied to the network output.
        W_constraint: instance of the [constraints](../constraints.md) module
            (eg. maxnorm, nonneg), applied to the main weights matrix.
        b_constraint: instance of the [constraints](../constraints.md) module,
            applied to the bias.
        data_format: 'th' or 'tf'. In 'th' mode, the channels dimension
            (the depth) is at index 1, in 'tf' mode is it at index 3.
            It defaults to the `image_data_format` value found in your
            Keras config file at `~/.keras/keras.json`.
            If you never set it, then it will be "th".
        bias: whether to include a bias
            (i.e. make the layer affine rather than linear).
    # Input shape
        4D tensor with shape:
        `(samples, channels, rows, cols)` if data_format='th'
        or 4D tensor with shape:
        `(samples, rows, cols, channels)` if data_format='tf'.
    # Output shape
        4D tensor with shape:
        `(samples, nb_filter, new_rows, new_cols)` if data_format='th'
        or 4D tensor with shape:
        `(samples, new_rows, new_cols, nb_filter)` if data_format='tf'.
        `rows` and `cols` values might have changed due to padding.
    '''
    def __init__(self, nb_filter, nb_row, nb_col,
                 init='glorot_uniform', activation='linear', weights=None,
                 padding='valid', subsample=(1, 1), data_format='default',
                 W_regularizer=None, b_regularizer=None, activity_regularizer=None,
                 W_constraint=None, b_constraint=None,
                 bias=True, tied_to=None, **kwargs):
        if data_format == 'default':
            data_format = K.image_data_format()
        if padding not in {'valid', 'same'}:
            raise Exception('Invalid border mode for Convolution2D:', padding)
        self.tied_to = tied_to
        self.nb_filter = nb_filter
        self.nb_row = tied_to.kernel_size[0]
        self.nb_col = tied_to.kernel_size[1]
        #self.init = initializers.get(init, data_format=data_format)
        self.init = initializers.get(init)
        self.activation = activations.get(activation)
        assert padding in {'valid', 'same'}, 'padding must be in {valid, same}'
        self.padding = padding
        self.subsample = tuple(subsample)
        assert data_format in {'channels_first','channels_last'}, 'data_format must be in {tf, th}'
        self.data_format = data_format

        self.W_regularizer = regularizers.get(W_regularizer)
        self.b_regularizer = regularizers.get(b_regularizer)
        self.activity_regularizer = regularizers.get(activity_regularizer)

        self.W_constraint = constraints.get(W_constraint)
        self.b_constraint = constraints.get(b_constraint)

        self.bias = bias
        self.input_spec = [InputSpec(ndim=4)]
        self.initial_weights = tied_to._initial_weights
        super(Convolution2D_tied, self).__init__(**kwargs)

    def build(self, input_shape):
        if self.data_format == 'channels_first':
            stack_size = input_shape[1]
            self.W_shape = (self.nb_filter, stack_size, self.nb_row, self.nb_col)
        elif self.data_format == 'channels_last':
            stack_size = input_shape[3]
            self.W_shape = (self.nb_row, self.nb_col, stack_size, self.nb_filter)
        else:
            raise Exception('Invalid data_format: ' + self.data_format)
        # self.W = self.init(self.W_shape, name='{}_W'.format(self.name))
        if self.bias:
            self.b = K.zeros((self.nb_filter,), name='{}_b'.format(self.name))
            self.trainable_weights = [self.b]
        # else:
        #     self.trainable_weights = [self.W]
        self.regularizers = []

        # if self.W_regularizer:
        #     self.W_regularizer.set_param(self.W)
        #     self.regularizers.append(self.W_regularizer)

        if self.bias and self.b_regularizer:
            self.b_regularizer.set_param(self.b)
            self.regularizers.append(self.b_regularizer)

        if self.activity_regularizer:
            self.activity_regularizer.set_layer(self)
            self.regularizers.append(self.activity_regularizer)

        self.constraints = {}
        # if self.W_constraint:
        #     self.constraints[self.W] = self.W_constraint
        if self.bias and self.b_constraint:
            self.constraints[self.b] = self.b_constraint

        # if self.initial_weights is not None:
        #     self.set_weights(self.initial_weights)
        #     del self.initial_weights

    def get_output_shape_for(self, input_shape):
        if self.data_format == 'channels_first':
            rows = input_shape[2]
            cols = input_shape[3]
        elif self.data_format == 'channels_last':
            rows = input_shape[1]
            cols = input_shape[2]
        else:
            raise Exception('Invalid data_format: ' + self.data_format)

        rows = conv_output_length(rows, self.nb_row,
                                  self.padding, self.subsample[0])
        cols = conv_output_length(cols, self.nb_col,
                                  self.padding, self.subsample[1])

        if self.data_format == 'channels_first':
            return (input_shape[0], self.nb_filter, rows, cols)
        elif self.data_format == 'channels_last':
            return (input_shape[0], rows, cols, self.nb_filter)
        else:
            raise Exception('Invalid data_format: ' + self.data_format)

    def call(self, x, mask=None):
        #W = tf.transpose(self.tied_to.weights[0], (1, 0, 2, 3))
        W = tf.transpose(self.tied_to.weights[0], (1, 0, 2, 3))
        #W = self.tied_to.weights[0]
        #output = K.conv2d(x, W, strides=self.subsample,
        #                  padding=self.padding,
        #                  data_format=self.data_format,
        #                  filter_shape=self.W_shape)
        output = K.conv2d(x, W, strides=self.subsample,
                          padding=self.padding,
                          data_format=self.data_format)
        #K.conv2d_transpose
        #output = K.conv2d(x, W, strides=self.subsample,
        #                  padding=self.padding,
        #                  data_format=self.data_format,
        #                  output_shape=self.W_shape)
        print("vla")
        print(output)
        if self.bias:
            if self.data_format == 'channels_first':
                output += K.reshape(self.b, (1, self.nb_filter, 1, 1))
                print(output)
            elif self.data_format == 'channels_last':
                #print(self.nb_filter)
                #print(K.reshape(self.b, (1, 1, 1, self.nb_filter)))
                #print(output)
                output += K.reshape(self.b, (1, 1, 1, self.nb_filter))
                print(output)
            else:
                raise Exception('Invalid data_format: ' + self.data_format)
        output = self.activation(output)
        print(output)
        print("flip")
        return output

    def get_config(self):
        config = {'nb_filter': self.nb_filter,
                  'nb_row': self.nb_row,
                  'nb_col': self.nb_col,
                  'init': self.init.__name__,
                  'activation': self.activation.__name__,
                  'padding': self.padding,
                  'subsample': self.subsample,
                  'data_format': self.data_format,
                  'W_regularizer': self.W_regularizer.get_config() if self.W_regularizer else None,
                  'b_regularizer': self.b_regularizer.get_config() if self.b_regularizer else None,
                  'activity_regularizer': self.activity_regularizer.get_config() if self.activity_regularizer else None,
                  'W_constraint': self.W_constraint.get_config() if self.W_constraint else None,
                  'b_constraint': self.b_constraint.get_config() if self.b_constraint else None,
                  'bias': self.bias}
        base_config = super(Convolution2D_tied, self).get_config()
        return dict(list(base_config.items()) + list(config.items()))

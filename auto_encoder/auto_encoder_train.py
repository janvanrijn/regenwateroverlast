#original code:
#https://blog.keras.io/building-autoencoders-in-keras.html

from keras.layers import Input, Dense, Conv2D, MaxPooling2D, UpSampling2D, Layer, InputSpec, Conv2DTranspose, Dropout, Activation,ZeroPadding2D,Concatenate,Add,BatchNormalization,Reshape,Flatten,Deconvolution2D
from keras.models import Model, model_from_json, load_model
from keras.callbacks import Callback
from keras import backend as K
from keras.callbacks import TensorBoard, ModelCheckpoint
from keras import regularizers, activations, initializers, constraints
from keras.constraints import UnitNorm, Constraint
import load_random_tif as lrt
import numpy as np
import matplotlib.pyplot as plt
import sys
import time
import json
import os
import tensorflow

import sklearn
from sklearn import datasets
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler, MinMaxScaler
from sklearn import decomposition

from keras.utils import conv_utils
from keras.utils import plot_model
from keras.regularizers import l2

import layers_tied

class Skip_manager(object):
    def __init__(self,skip_ints,skip_ints_count):
        self.skip_ints= skip_ints
        self.skip_ints_count = skip_ints_count
        self.skip_connections = []
        self.layer_num = 0 #layer number of currently build layer
    
    def identity(self,num):
        return num
    
    def gray(self,num):
        return num ^ (num >> 1)
    
    def startpoint(self,func,num):
        return (func(num) >> self.layer_num) & 1
    
    def set_dropout(self,dropout_val):
        for i in range(len(self.skip_connections)):
            self.skip_connections[i][3] = dropout_val
        return
    
    def pad_and_connect(self, layer, incoming_layer):
        if K.int_shape(incoming_layer)[1] != K.int_shape(layer)[1] or K.int_shape(incoming_layer)[2] != K.int_shape(layer)[2]:
            pad_tpl1 = (int(np.floor(np.abs(K.int_shape(incoming_layer)[1]-K.int_shape(layer)[1])/2)),int(np.ceil(np.abs(K.int_shape(incoming_layer)[1]-K.int_shape(layer)[1])/2)))
            pad_tpl2 = (int(np.floor(np.abs(K.int_shape(incoming_layer)[2]-K.int_shape(layer)[2])/2)),int(np.ceil(np.abs(K.int_shape(incoming_layer)[2]-K.int_shape(layer)[2])/2)))
            #print(pad_tpl)
            if K.int_shape(incoming_layer)[1] < K.int_shape(layer)[1] and K.int_shape(incoming_layer)[2] < K.int_shape(layer)[2]:
                padded = ZeroPadding2D(padding=(pad_tpl1, pad_tpl2))(incoming_layer)
                layer = Concatenate()([layer, padded])
            elif K.int_shape(incoming_layer)[1] < K.int_shape(layer)[1] and K.int_shape(incoming_layer)[2] >= K.int_shape(layer)[2]:
                padded1 = ZeroPadding2D(padding=(pad_tpl1, 0))(incoming_layer)
                padded2 = ZeroPadding2D(padding=(0, pad_tpl2))(layer)
                layer = Concatenate()([padded1, padded2])
            elif K.int_shape(incoming_layer)[1] >= K.int_shape(layer)[1] and K.int_shape(incoming_layer)[2] < K.int_shape(layer)[2]:
                padded1 = ZeroPadding2D(padding=(0, pad_tpl2))(incoming_layer)
                padded2 = ZeroPadding2D(padding=(pad_tpl1, 0))(layer)
                layer= Concatenate()([padded1, padded2])
            else:
                #print(layer.shape)
                padded = ZeroPadding2D(padding=(pad_tpl1, pad_tpl2))(layer)
                #print(padded.shape)
                #print(incoming_layer.shape)
                layer= Concatenate()([padded, incoming_layer])
        else:
            layer= Concatenate()([layer, incoming_layer])
        return layer

    def pool_pad_connect(self, layer, incoming_layer,dropout_val):
        if K.int_shape(incoming_layer)[1] != K.int_shape(layer)[1] or K.int_shape(incoming_layer)[2] != K.int_shape(layer)[2]:
            #print('layer dimensions:')
            #print(K.int_shape(layer)[1], K.int_shape(layer)[2])
            #print('incoming_layer dimensions:')
            #print(K.int_shape(incoming_layer)[1],  K.int_shape(incoming_layer)[2])
            if K.int_shape(incoming_layer)[1] < K.int_shape(layer)[1] and K.int_shape(incoming_layer)[2] < K.int_shape(layer)[2]:
                pass
            elif K.int_shape(incoming_layer)[1] < K.int_shape(layer)[1] and K.int_shape(incoming_layer)[2] >= K.int_shape(layer)[2]:
                scalar = int(np.ceil(K.int_shape(incoming_layer)[2] / K.int_shape(layer)[2]))
                incoming_layer = MaxPooling2D(pool_size=(1, scalar), strides=(1, scalar), padding='same')(incoming_layer)
                print('warning: code used that is not tested, see: all_cnn_bi_skippy.py --> pool_pad_connect()')
            elif K.int_shape(incoming_layer)[1] >= K.int_shape(layer)[1] and K.int_shape(incoming_layer)[2] < K.int_shape(layer)[2]:
                scalar = int(np.ceil(K.int_shape(incoming_layer)[1] / K.int_shape(layer)[1]))
                incoming_layer = MaxPooling2D(pool_size=(scalar, 1), strides=(scalar, 1), padding='same')(incoming_layer)
                print('warning: code used that is not tested, see: all_cnn_bi_skippy.py --> pool_pad_connect()')
            else: #K.int_shape(incoming_layer)[1] > K.int_shape(layer)[1] and K.int_shape(incoming_layer)[2] > K.int_shape(layer)[2]
                scalar_1 =  int(np.ceil(K.int_shape(incoming_layer)[1] / K.int_shape(layer)[1]))
                scalar_2 =  int(np.ceil(K.int_shape(incoming_layer)[2] / K.int_shape(layer)[2]))
                incoming_layer = MaxPooling2D(pool_size=(scalar_1, scalar_2), strides=(scalar_1, scalar_2), padding='same')(incoming_layer)
                #print('Did a max pool')
        if dropout_val is not None:
            incoming_layer = Dropout(dropout_val)(incoming_layer)
        return self.pad_and_connect(layer, incoming_layer)

    def start_skip(self,layer):
        for j in range(len(self.skip_ints)):
            if self.skip_ints_count[j] > 1 and self.startpoint(self.identity,self.skip_ints[j]):#CHRIS skip connections smaller than 2 are not made, thus mean no skip connection.
                self.skip_connections.append([layer,self.skip_ints_count[j],self.layer_num,None])#save layer output, skip counter, layer this skip connection starts (to remove duplicates)
        return layer
    
    def end_skip(self,layer,filters,kernel,regulizer,act):
        for j in range(len(self.skip_connections)):
            self.skip_connections[j][1] -= 1 #decrease skip connection counters
        j = 0
        prev_skip = -1
        connected = False #CHRIS check if an end skip connection is made
        while j < len(self.skip_connections):
            if self.skip_connections[j][1] <= 0:
                #print(prev_skip,self.skip_connections[j][2])
                if prev_skip != self.skip_connections[j][2]:#this removes skip connection duplicates (works because same skip connections are next to eachother) TODO maybe better to make more robust
                    #CHRIS TODO add pooling, because this becomes too complex to train
                    #layer = self.pad_and_connect(layer, self.skip_connections[j][0])#CHRIS warning! pad_and_connect does not do dropout!
                    layer = self.pool_pad_connect(layer, self.skip_connections[j][0],self.skip_connections[j][3])
                    connected = True#CHRIS an end skip connection is made
                #if upscaling is desired: (can result in enormous tensors though)
                #shape1 = K.int_shape(layer)
                #shape2 = K.int_shape(self.skip_connections[j][0])
                #gcd_x = gcd(shape1[1], shape2[1])
                #gcd_y = gcd(shape1[2], shape2[2])
                #scale1 =shape2[1] // gcd_x, shape2[2] // gcd_y
                #scale2 =shape1[1] // gcd_x, shape1[2] // gcd_y
                #upscaled1 = UpSampling2D(size=scale1, interpolation='nearest')(layer)
                #upscaled2 = UpSampling2D(size=scale2, interpolation='nearest')(self.skip_connections[j][0])
                #layer = keras.layers.Concatenate()([upscaled1, upscaled2])
                prev_skip = self.skip_connections[j][2]
                del self.skip_connections[j]
            else:
                j += 1
        if connected and K.int_shape(layer)[3] > filters:#CHRIS we only want projection if an end skip connection is made, hence: ''connected''
            #CHRIS convolution to bound amount of features
            #CHRIS can funcion as addition, or projection followed by addition
            layer = Conv2D(filters, (1,1), padding='same', kernel_regularizer=l2(regulizer), bias_regularizer=l2(regulizer))(layer)#CHRIS kernel value set to (1,1) in order to simply act as projection
            #layer = Activation(act)(layer)
        for j in range(len(self.skip_connections)):#CHRIS TODO this is a bit hacky
            self.skip_connections[j][1] += 1 #decrease skip connection counters
        return layer

    def connect_skip(self,layer,filters,kernel,regulizer,act):
        
        #end skip connections
        layer = self.end_skip(layer,filters,kernel,regulizer,act)
        for j in range(len(self.skip_connections)):#CHRIS TODO this is a bit hacky
            self.skip_connections[j][1] -= 1 #decrease skip connection counters
        
        #start skip connections
        layer = self.start_skip(layer)
        
        self.layer_num +=1 #increase layer number where currently building takes place
        return layer

class TimedAccHistory(Callback):
    def on_train_begin(self, logs={}):
        self.my_hist = {}
        self.my_hist['time'] = []
        self.my_hist['train_acc'] = []
        self.my_hist['train_loss'] = []
        self.my_hist['test_acc'] = []
        self.my_hist['test_loss'] = []
        self.start_time = time.time()
    
    def on_epoch_end(self, batch, logs={}):
        self.my_hist['time'].append(time.time() - self.start_time)
        if logs.get('accuracy') is None:
            self.my_hist['train_acc'].append(logs.get('acc'))
        else:
            self.my_hist['train_acc'].append(logs.get('accuracy'))
        self.my_hist['train_loss'].append(logs.get('loss'))
        
        if logs.get('val_accuracy') is None:
            self.my_hist['test_acc'].append(logs.get('val_acc'))
        else:
            self.my_hist['test_acc'].append(logs.get('val_accuracy'))
        self.my_hist['test_loss'].append(logs.get('val_loss'))

#TODO add saving network with lowest loss

class DenseTied(Layer):
    #best practices and code derived from following post by Chitta Ranjan:
    #https://towardsdatascience.com/build-the-right-autoencoder-tune-and-optimize-using-pca-principles-part-ii-24b9cca69bd6
    
    def __init__(self, units,
                 activation=None,
                 use_bias=True,
                 kernel_initializer='glorot_uniform',
                 bias_initializer='zeros',
                 kernel_regularizer=None,
                 bias_regularizer=None,
                 activity_regularizer=None,
                 kernel_constraint=None,
                 bias_constraint=None,
                 tied_to=None,
                 **kwargs):
        self.tied_to = tied_to
        if 'input_shape' not in kwargs and 'input_dim' in kwargs:
            kwargs['input_shape'] = (kwargs.pop('input_dim'),)
        super().__init__(**kwargs)
        self.units = units
        self.activation = activations.get(activation)
        self.use_bias = use_bias
        self.kernel_initializer = initializers.get(kernel_initializer)
        self.bias_initializer = initializers.get(bias_initializer)
        self.kernel_regularizer = regularizers.get(kernel_regularizer)
        self.bias_regularizer = regularizers.get(bias_regularizer)
        self.activity_regularizer = regularizers.get(activity_regularizer)
        self.kernel_constraint = constraints.get(kernel_constraint)
        self.bias_constraint = constraints.get(bias_constraint)
        self.input_spec = InputSpec(min_ndim=2)
        self.supports_masking = True
    
    def build(self, input_shape):
        assert len(input_shape) >= 2
        input_dim = input_shape[-1]
        
        if self.tied_to is not None:
            self.kernel = K.transpose(self.tied_to.kernel)
            self._non_trainable_weights.append(self.kernel)
        else:
            self.kernel = self.add_weight(shape=(input_dim, self.units),
                                          initializer=self.kernel_initializer,
                                          name='kernel',
                                          regularizer=self.kernel_regularizer,
                                          constraint=self.kernel_constraint)
        if self.use_bias:
            self.bias = self.add_weight(shape=(self.units,),
                                        initializer=self.bias_initializer,
                                        name='bias',
                                        regularizer=self.bias_regularizer,
                                        constraint=self.bias_constraint)
        else:
            self.bias = None
        self.input_spec = InputSpec(min_ndim=2, axes={-1: input_dim})
        self.built = True
    
    def compute_output_shape(self, input_shape):
        assert input_shape and len(input_shape) >= 2
        output_shape = list(input_shape)
        output_shape[-1] = self.units
        return tuple(output_shape)
    
    def call(self, inputs):
        output = K.dot(inputs, self.kernel)
        if self.use_bias:
            output = K.bias_add(output, self.bias, data_format='channels_last')
        if self.activation is not None:
            output = self.activation(output)
        return output

class Conv2DTied(Conv2DTranspose):
    #best practices and code derived from following post by Chitta Ranjan:
    #https://towardsdatascience.com/build-the-right-autoencoder-tune-and-optimize-using-pca-principles-part-ii-24b9cca69bd6
    
    def __init__(self,
                 filters,
                 kernel_size,
                 strides=1,
                 padding='valid',
                 data_format=None,
                 dilation_rate=1,
                 activation=None,
                 use_bias=True,
                 kernel_initializer='glorot_uniform',
                 bias_initializer='zeros',
                 kernel_regularizer=None,
                 bias_regularizer=None,
                 activity_regularizer=None,
                 kernel_constraint=None,
                 bias_constraint=None,
                 tied_to=None,
                 **kwargs):

        super().__init__(filters,kernel_size,**kwargs)
        self.bias_constraint = constraints.get(bias_constraint)
        self.supports_masking = True
        
        self.tied_to = tied_to
        self.filters = filters
        self.kernel_size = conv_utils.normalize_tuple(kernel_size, 2,'kernel_size')
        self.strides = conv_utils.normalize_tuple(strides, 2, 'strides')
        self.padding = conv_utils.normalize_padding(padding)
        self.data_format = K.normalize_data_format(data_format)
        self.dilation_rate = conv_utils.normalize_tuple(dilation_rate, 2,
                                                        'dilation_rate')
        self.activation = activations.get(activation)
        self.use_bias = use_bias
        self.kernel_initializer = initializers.get(kernel_initializer)
        self.bias_initializer = initializers.get(bias_initializer)
        self.kernel_regularizer = regularizers.get(kernel_regularizer)
        self.bias_regularizer = regularizers.get(bias_regularizer)
        self.activity_regularizer = regularizers.get(activity_regularizer)
        self.kernel_constraint = constraints.get(kernel_constraint)
        self.bias_constraint = constraints.get(bias_constraint)
        self.input_spec = InputSpec(ndim=4)
    

    def build(self, input_shape):
        if self.data_format == 'channels_first':
            channel_axis = 1
        else:
            channel_axis = -1
        if input_shape[channel_axis] is None:
            raise ValueError('The channel dimension of the inputs '
                             'should be defined. Found `None`.')
        input_dim = input_shape[channel_axis]
        kernel_shape = self.kernel_size + (input_dim, self.filters)
        if self.tied_to is not None:
            print(self.tied_to.kernel)
            #print(self.tied_to.kernel[::,::,0,0].shape)
            kernel_transposed = K.transpose(self.tied_to.kernel)
            #kernel_concatinated = kernel_transposed[0,0,::,::]
            kernel_concatinated = tensorflow.concat(0, [[kernel_transposed[0,0,::,::]], [kernel_transposed[1,0,::,::]], [kernel_transposed[2,0,::,::]],[kernel_transposed[3,0,::,::]]])
            #for i in range(1,3):
            #    tensorflow.concat([kernel_concatinated,kernel_transposed[i,0,::,::]],1)
            print(kernel_concatinated)
            exit()
            #self.kernel = K.transpose(self.tied_to.kernel[::,::,j,i])
            self.kernel = K.transpose(self.tied_to.kernel)
            self._non_trainable_weights.append(self.kernel)
        else:
            self.kernel = self.add_weight(shape=kernel_shape,
                                        initializer=self.kernel_initializer,
                                        name='kernel',
                                        regularizer=self.kernel_regularizer,
                                        constraint=self.kernel_constraint)
        if self.use_bias:
            self.bias = self.add_weight(shape=(self.filters,),
                                        initializer=self.bias_initializer,
                                        name='bias',
                                        regularizer=self.bias_regularizer,
                                        constraint=self.bias_constraint)
        else:
             self.bias = None
        # Set input spec.
        self.input_spec = InputSpec(ndim=2 + 2,axes={channel_axis: input_dim})
        self.built = True
    
    def compute_output_shape(self, input_shape):
        if self.data_format == 'channels_last':
            space = input_shape[1:-1]
        elif self.data_format == 'channels_first':
            space = input_shape[2:]
        new_space = []
        for i in range(len(space)):
            new_dim = conv_utils.conv_output_length(
                                                    space[i],
                                                    self.kernel_size[i],
                                                    padding=self.padding,
                                                    stride=self.strides[i],
                                                    dilation=self.dilation_rate[i])
            new_space.append(new_dim)
        if self.data_format == 'channels_last':
            return (input_shape[0],) + tuple(new_space) + (self.filters,)
        elif self.data_format == 'channels_first':
            return (input_shape[0], self.filters) + tuple(new_space)
    
    def call(self, inputs):
        output = K.conv2d(
                           inputs,
                           self.kernel,
                           strides=self.strides,
                           padding=self.padding,
                           data_format=self.data_format,
                           dilation_rate=self.dilation_rate)
        if self.use_bias:
            outputs = K.bias_add(
                                 outputs,
                                 self.bias,
                                 data_format=self.data_format)
                
        if self.activation is not None:
            return self.activation(outputs)
        return outputs

def build_raw_ae(parameters,data_shape,kernel_size):
    input_img = Input(shape=(data_shape[1],data_shape[2],data_shape[3]))
    encoded = MaxPooling2D((kernel_size,kernel_size), padding='same')(input_img)
    
    encoder = Model(input_img, encoded)
    
    output_img = UpSampling2D((kernel_size,kernel_size))(encoded)
    autoencoder = Model(input_img, output_img)
    
    autoencoder.compile(optimizer=parameters['optimizer'], loss=parameters['loss'],metrics=['accuracy'])
    return autoencoder,encoder

def build_blur_ae(parameters,data_shape,kernel_size):
    input_img = Input(shape=(data_shape[1],data_shape[2],data_shape[3]))
    encoded = MaxPooling2D((kernel_size,kernel_size), padding='same')(input_img)
    
    encoder = Model(input_img, encoded)
    
    x = UpSampling2D((kernel_size,kernel_size))(encoded)
    output_img = Conv2D(data_shape[3], (kernel_size, kernel_size), activation='sigmoid', padding='same',use_bias=False, name='conv1')(x)
    autoencoder = Model(input_img, output_img)
    keras_layer = [layer for layer in autoencoder.layers if layer.name=='conv1'][0]
    #TODO in case of 2 channel input, this also blurs across channels, which is not intended
    blur_filter = np.array([np.array([[[[1/(kernel_size*kernel_size)]*data_shape[3]]*data_shape[3]]*kernel_size]*kernel_size)])
    keras_layer.set_weights(blur_filter)
    
    autoencoder.compile(optimizer=parameters['optimizer'], loss=parameters['loss'],metrics=['accuracy'])
    return autoencoder,encoder
    
    
def build_simple_ae(parameters,data_shape):
    #edited version of method from:
    #https://blog.keras.io/building-autoencoders-in-keras.html
    
    # this is the size of our encoded representations
    encoding_dim = parameters['encoding_dim']

    # this is our input placeholder
    input_img = Input(shape=(data_shape[1],))
    # "encoded" is the encoded representation of the input
    encoded = Dense(encoding_dim, activation='relu')(input_img)
    # "decoded" is the lossy reconstruction of the input
    decoded = Dense(data_shape[1], activation='sigmoid')(encoded)

    # this model maps an input to its reconstruction
    autoencoder = Model(input_img, decoded)

    # this model maps an input to its encoded representation
    encoder = Model(input_img, encoded)

    # create a placeholder for an encoded (32-dimensional) input
    #encoded_input = Input(shape=(encoding_dim,))
    # retrieve the last layer of the autoencoder model
    #decoder_layer = autoencoder.layers[-1]
    # create the decoder model
    #decoder = Model(encoded_input, decoder_layer(encoded_input))

    autoencoder.compile(optimizer=parameters['optimizer'], loss=parameters['loss'],metrics=['accuracy'])
    
    return autoencoder, encoder#, decoder

def build_shallow_cnn_ae(parameters,data_shape):
    #edited version of method from:
    #https://blog.keras.io/building-autoencoders-in-keras.html
    
    input_img = Input(shape=(data_shape[1],data_shape[2],data_shape[3]))  # adapt this if using `channels_first` image data format

    x = Conv2D(16, (3, 3), activation='relu', padding='same')(input_img)
    x = MaxPooling2D((2,2), padding='same')(x)
    x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    x = MaxPooling2D((2,2), padding='same')(x)
    x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    encoded = MaxPooling2D((parameters['pooling_factor'], parameters['pooling_factor']), padding='same')(x)
    
    #encoded_input = Input(shape=(encoded.shape[1],encoded.shape[2],encoded.shape[3],))

    x = Conv2D(8, (3, 3), activation='relu', padding='same')(encoded)
    x = UpSampling2D((parameters['pooling_factor'], parameters['pooling_factor']))(x)
    x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    x = UpSampling2D((2,2))(x)
    x = Conv2D(16, (3, 3), activation='relu',padding='same')(x)
    x = UpSampling2D((2,2))(x)
    decoded = Conv2D(data_shape[3], (3, 3), activation='sigmoid', padding='same')(x)
    
    autoencoder = Model(input_img, decoded)
    #decoder_layer = autoencoder.layers[-1]
    encoder = Model(input_img, encoded)
    #decoder = Model(encoded_input, decoder_layer)
    autoencoder.compile(optimizer='adadelta', loss='binary_crossentropy',metrics=['accuracy'])
    return autoencoder, encoder#, decoder

def build_shallow_narrow_cnn_ae(parameters,data_shape):
    #edited version of method from:
    #https://blog.keras.io/building-autoencoders-in-keras.html
    
    input_img = Input(shape=(data_shape[1],data_shape[2],data_shape[3]))  # adapt this if using `channels_first` image data format
    
    x = Conv2D(16, (3, 3), activation='relu', padding='same')(input_img)
    x = MaxPooling2D((2,2), padding='same')(x)
    x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    x = MaxPooling2D((2,2), padding='same')(x)
    #x = Conv2D(1, (3, 3), activation='relu', padding='same')(x)
    x = Conv2D(2, (3, 3), activation='relu', padding='same')(x)
    encoded = MaxPooling2D((parameters['pooling_factor'], parameters['pooling_factor']), padding='same')(x)
    
    #encoded_input = Input(shape=(encoded.shape[1],encoded.shape[2],encoded.shape[3],))
    
    #x = Conv2DTranspose(1, (3, 3), activation='relu', padding='same')(encoded)
    x = Conv2DTranspose(2, (3, 3), activation='relu', padding='same')(encoded)
    x = UpSampling2D((parameters['pooling_factor'], parameters['pooling_factor']))(x)
    x = Conv2DTranspose(8, (3, 3), activation='relu', padding='same')(x)
    x = UpSampling2D((2,2))(x)
    x = Conv2DTranspose(16, (3, 3), activation='relu',padding='same')(x)
    x = UpSampling2D((2,2))(x)
    decoded = Conv2DTranspose(data_shape[3], (3, 3), activation='sigmoid', padding='same')(x)
    
    autoencoder = Model(input_img, decoded)
    #decoder_layer = autoencoder.layers[-1]
    encoder = Model(input_img, encoded)
    #decoder = Model(encoded_input, decoder_layer)
    autoencoder.compile(optimizer='adadelta', loss='binary_crossentropy',metrics=['accuracy'])
    autoencoder.summary()
    return autoencoder, encoder#, decoder

def build_deep_cnn_ae(parameters,data_shape):
    #edited version of method from:
    #https://blog.keras.io/building-autoencoders-in-keras.html
    multiple = 5
    pooling_factor = parameters['pooling_var']
    
    input_img = Input(shape=(data_shape[1],data_shape[2],data_shape[3]))  # adapt this if using `channels_first` image data format
    x=input_img
    for i in range(multiple):
        x = Conv2D(16, (3, 3), activation='relu', padding='same')(x)
    x = MaxPooling2D((2,2), padding='same')(x)
    for i in range(multiple):
        x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    x = MaxPooling2D((2,2), padding='same')(x)
    for i in range(multiple):
        x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    encoded = MaxPooling2D((pooling_factor,pooling_factor), padding='same')(x)
    
    #encoded_input = Input(shape=(encoded.shape[1],encoded.shape[2],encoded.shape[3],))
    
    # at this point the representation is (4, 4, 8) i.e. 128-dimensional
    x = encoded
    for i in range(multiple):
        x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    x = UpSampling2D((pooling_factor,pooling_factor))(x)
    for i in range(multiple):
        x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    x = UpSampling2D((2,2))(x)
    for i in range(multiple):
        x = Conv2D(16, (3, 3), activation='relu',padding='same')(x)
    x = UpSampling2D((2,2))(x)
    decoded = Conv2D(data_shape[3], (3, 3), activation='sigmoid', padding='same')(x)
    
    autoencoder = Model(input_img, decoded)
    #decoder_layer = autoencoder.layers[-1]
    encoder = Model(input_img, encoded)
    #decoder = Model(encoded_input, decoder_layer)
    autoencoder.compile(optimizer='adadelta', loss='binary_crossentropy',metrics=['accuracy'])
    autoencoder.summary()
    return autoencoder, encoder#, decoder

def make_skip_int(stepsize,network_depth):
    cnt = 0
    skint = 1
    while cnt <= network_depth:
        skint = skint << stepsize
        skint += 1
        cnt += stepsize
    return skint

def build_advanced_cnn_ae(parameters,data_shape):
    #edited version of method from:
    #https://blog.keras.io/building-autoencoders-in-keras.html
    
    multiple = parameters['multiple']
    
    network_depth = multiple * 6
    skints = []
    sksteps = []
    for i in range(2,8):
        skints.append(make_skip_int(i,network_depth))
        sksteps.append(i)
    
    skip_manager = Skip_manager(skints,sksteps)
    activation = 'elu'
    input_img = Input(shape=(data_shape[1],data_shape[2],data_shape[3]))  # adapt this if using `channels_first` image data format
    x=input_img
    
    filter_amount = parameters['filter_amount']
    kernel_size = [3,3,3,3]
    dropout = [0.2,0.2,0.2,0.2]
    pooling = parameters['pooling']
    for i in range(len(filter_amount)):
        for j in range(multiple):
            x = Conv2D(filter_amount[i], (kernel_size[i], kernel_size[i]), padding='same')(x)
            x = skip_manager.connect_skip(x,filter_amount[i],kernel_size[i],0,activation)
            x = Activation(activation)(x)
        x = MaxPooling2D((pooling[i],pooling[i]), padding='same')(x)
        x = Dropout(dropout[i])(x)
        skip_manager.set_dropout(dropout[i])
    encoded = x

    skip_manager.skip_connections = []#no skip connections may cross the compression gap
    
    #encoded_input = Input(shape=(encoded.shape[1],encoded.shape[2],encoded.shape[3],))
    
    # at this point the representation is (4, 4, 8) i.e. 128-dimensional
    x = encoded
    for i in range(len(filter_amount)-1,-1,-1):
        for j in range(multiple):
            x = Conv2DTranspose(filter_amount[i], (kernel_size[i], kernel_size[i]), padding='same')(x)
            x = skip_manager.connect_skip(x,filter_amount[i],kernel_size[i],0,activation)
            x = Activation(activation)(x)
        x = UpSampling2D((pooling[i],pooling[i]))(x)
        x = Dropout(dropout[i])(x)
        skip_manager.set_dropout(dropout[i])
    decoded = Conv2DTranspose(data_shape[3], (3, 3), activation='sigmoid', padding='same')(x)
    
    autoencoder = Model(input_img, decoded)
    #decoder_layer = autoencoder.layers[-1]
    encoder = Model(input_img, encoded)
    #decoder = Model(encoded_input, decoder_layer)
    autoencoder.compile(optimizer='adadelta', loss='binary_crossentropy',metrics=['accuracy'])
    #plot_model(autoencoder, to_file='./autoencoder_skippy_test.png',show_shapes=True,show_layer_names=True)
    #autoencoder.summary()
    return autoencoder, encoder#, decoder

def build_well_posed_dense_ae(parameters,data_shape):
    #best practices and code derived from following post by Chitta Ranjan:
    #https://towardsdatascience.com/build-the-right-autoencoder-tune-and-optimize-using-pca-principles-part-ii-24b9cca69bd6
    
    input_img = Input(shape=(data_shape[1],))
    encode_layer = Dense(parameters['encoding_dim'], activation="linear", use_bias = True)
    decode_layer = DenseTied(data_shape[1], activation="linear", tied_to=encode_layer, use_bias = True)
    #decode_layer = DenseTied(data_shape[1], activation="linear", use_bias = True)
    
    encoded = encode_layer(input_img)
    decoded = decode_layer(encoded)
    
    autoencoder = Model(input_img, decoded)
    encoder = Model(input_img, encoded)
    autoencoder.compile(optimizer='adadelta', loss='binary_crossentropy',metrics=['accuracy'])
    #autoencoder.compile(metrics=['accuracy'],
    #                    loss='mean_squared_error',
    #                    optimizer='sgd')
    return autoencoder, encoder

def build_well_posed_cnn_ae(parameters,data_shape):
    #best practices and code derived from following post by Chitta Ranjan:
    #https://towardsdatascience.com/build-the-right-autoencoder-tune-and-optimize-using-pca-principles-part-ii-24b9cca69bd6
    
    #edited version of method from:
    #https://blog.keras.io/building-autoencoders-in-keras.html
    
    #--------------------------------------------------------------------------------
    
    input_img = Input(shape=(data_shape[1],data_shape[2],data_shape[3]))  # adapt this if using `channels_first` image data format
    
    layer1 = Conv2D(8, (3, 3), activation='relu', padding='same')
    layer2 = MaxPooling2D((2,2), padding='same')
    layer3 = Conv2D(8, (3, 3), activation='relu', padding='same')
    layer4 = MaxPooling2D((2,2), padding='same')
    layer5 = Conv2D(8, (3, 3), activation='relu', padding='same')
    layer6 = MaxPooling2D((parameters['pooling_factor'], parameters['pooling_factor']), padding='same')
    encoded_layer = Conv2D(8, (3, 3), activation='relu', padding='same')
    
    layer7 = layers_tied.Convolution2D_tied(8, 3, 3, activation='relu', padding='same',tied_to=encoded_layer)
    layer8 = UpSampling2D((parameters['pooling_factor'], parameters['pooling_factor']))
    layer9 = layers_tied.Convolution2D_tied(8, 3, 3, activation='relu', padding='same',tied_to=layer5)
    layer10 = UpSampling2D((2,2))
    layer11 = layers_tied.Convolution2D_tied(8, 3, 3, activation='relu',padding='same',tied_to=layer3)
    layer12 = UpSampling2D((2,2))
    layer13 = layers_tied.Convolution2D_tied(8, 3, 3, activation='sigmoid', padding='same',tied_to=layer1)
    decoded_layer = Conv2D(data_shape[3], (3, 3), activation='relu', padding='same')
    
    print("input_img")
    print(input_img.shape)
    x=layer1(input_img)
    print("layer1")
    print(x.shape)
    x=layer2(x)
    print("layer2")
    print(x.shape)
    x=layer3(x)
    print("layer3")
    print(x.shape)
    x=layer4(x)
    print("layer4")
    print(x.shape)
    x=layer5(x)
    print("layer5")
    print(x.shape)
    x=layer6(x)
    print("layer6")
    print(x.shape)
    encoded=encoded_layer(x)
    print("encoded")
    print(encoded.shape)
    x=layer7(encoded)
    print("layer7")
    print(x.shape)
    x=layer8(x)
    print("layer8")
    print(x.shape)
    x=layer9(x)
    print("layer9")
    print(x.shape)
    x=layer10(x)
    print("layer10")
    print(x.shape)
    x=layer11(x)
    print("layer11")
    print(x.shape)
    x=layer12(x)
    print("layer12")
    print(x.shape)
    x=layer13(x)
    print("layer13")
    print(x.shape)
    decoded=decoded_layer(x)
    print("decoded")
    print(decoded.shape)
    
    #--------------------------------------------------------------------------------
    
    autoencoder = Model(input_img, decoded)
    encoder = Model(input_img, encoded)
    autoencoder.compile(optimizer='adadelta', loss='binary_crossentropy',metrics=['accuracy'])
    #autoencoder.compile(metrics=['accuracy'],
    #                    loss='mean_squared_error',
    #                    optimizer='sgd')
    autoencoder.summary()
    return autoencoder, encoder

def build_surajits_ae(parameters,data_shape):
    #ae designed by Surajit Saikia
    #source: https://medium.com/analytics-vidhya/building-a-convolutional-autoencoder-using-keras-using-conv2dtranspose-ca403c8d144e
    encoded_dim = int(data_shape[1]/(parameters['pooling_factor']*2*2))
    inp = Input(shape=(data_shape[1],data_shape[2],data_shape[3]))
    e = Conv2D(16, (3, 3), activation='relu')(inp)
    e = MaxPooling2D((2, 2))(e)
    e = Conv2D(32, (3, 3), activation='relu')(e)
    e = MaxPooling2D((parameters['pooling_factor'], parameters['pooling_factor']))(e)
    e = Conv2D(32, (3, 3), activation='relu')(e)
    l = Flatten()(e)
    encoded = Dense(encoded_dim*encoded_dim, activation='softmax')(l)#DECODER
    d = Reshape((encoded_dim,encoded_dim,1))(encoded)
    d = Conv2DTranspose(32,(3, 3), strides=parameters['pooling_factor'], activation='relu', padding='same')(d)
    d = BatchNormalization()(d)
    d = Conv2DTranspose(32,(3, 3), strides=2, activation='relu', padding='same')(d)
    d = BatchNormalization()(d)
    d = Conv2DTranspose(16,(3, 3), activation='relu', padding='same')(d)
    decoded = Conv2D(data_shape[3], (3, 3), activation='sigmoid', padding='same')(d)
    autoencoder = Model(inp, decoded)
    encoder = Model(inp, encoded)
    autoencoder.compile(optimizer=parameters['optimizer'], loss=parameters['loss'])#Train it by providing training images
    autoencoder.summary()
    return autoencoder, encoder

def auto_encoder_train(parameters,data):
    '''
        This function selects an autoencoder defined by parameters['encoder_type']. It then builds the selected autoencoder, compiles it and trains it.
        ---arguments---
        parameters: dictionary conatining various parameters needed to build an autoencoder
        data: tuple containing three numpy arrays: the train, test and validation set. These sets contain image data and it is assumed they are properly formatted for the selected autoencoder
        
        ---returns---
        autoencoder: compiled and trained autoencoder fitted on x_train. It's output is trained to closely match the input.
        encoder: compiled and trained encoder section of the autoencoder fitted on x_train. When fed an input, it outputs the learned features.
        history: dictionary containing log of the training session. It contains time stamps, train and validation loss and train and validataion accuracy, measured during training.
    '''
    np.random.seed(parameters['seed'])
    tensorflow.set_random_seed(parameters['seed'])
    x_train,x_test,x_val = data
    if parameters['encoder_type'] == 'simple':
        autoencoder, encoder = build_simple_ae(parameters,x_train.shape)
    elif parameters['encoder_type']=='shallow_cnn':
        autoencoder, encoder = build_shallow_cnn_ae(parameters,x_train.shape)
    elif parameters['encoder_type']=='deep_cnn':
        autoencoder, encoder = build_deep_cnn_ae(parameters,x_train.shape)
    elif parameters['encoder_type']=='blur':
        autoencoder, encoder = build_blur_ae(parameters,x_train.shape,parameters['kernel_size'])
    elif parameters['encoder_type']=='raw':
        autoencoder, encoder = build_raw_ae(parameters,x_train.shape,parameters['kernel_size'])
    elif parameters['encoder_type']=='shallow_narrow_cnn':
        autoencoder, encoder = build_shallow_narrow_cnn_ae(parameters,x_train.shape)
    elif parameters['encoder_type']=='well_posed_dense':
        autoencoder, encoder = build_well_posed_dense_ae(parameters,x_train.shape)
    elif parameters['encoder_type']=='well_posed_cnn':
        autoencoder, encoder = build_well_posed_cnn_ae(parameters,x_train.shape)
    elif parameters['encoder_type']=='advanced':
        autoencoder,encoder = build_advanced_cnn_ae(parameters,x_train.shape)
    elif parameters['encoder_type']=='surajit':
        autoencoder,encoder = build_surajits_ae(parameters,x_train.shape)
    else:
        print('unknown autoencoder type auto_encoder_train()')
        exit()
    
    history = TimedAccHistory()
    #checkpoint = ModelCheckpoint(parameters['results_folder']+parameters['file_name']+"autoencoder_best_weights.h5", monitor='loss', verbose=1, save_best_only=True, mode='min', period=1)
    #10 epochs
    if not parameters['encoder_type'] == 'blur' and not parameters['encoder_type'] == 'raw':
        autoencoder.fit(x_train, x_train,
                        epochs=parameters['epochs'],
                        batch_size=parameters['batch_size'],
                        shuffle=True,
                        validation_data=(x_test, x_test),callbacks=[history])
    
    return autoencoder, encoder, history
    

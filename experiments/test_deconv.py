#source of code: https://medium.com/analytics-vidhya/building-a-convolutional-autoencoder-using-keras-using-conv2dtranspose-ca403c8d144e

import os
os.environ['KERAS_BACKEND'] = 'tensorflow'
from keras.preprocessing.image import ImageDataGenerator

gen = ImageDataGenerator()
train_im = ImageDataGenerator(
                            rescale=1./255,
                            shear_range=0.2,
                            horizontal_flip=False)
def train_images():
    train_generator = train_im.flow_from_directory (
                                                    'Path to the folder containing image folders', 
                                                    target_size=(224, 224),
                                                    color_mode='rgb',
                                                    batch_size=100,
                                                    shuffle = True,
                                                    class_mode='categorical')
    x =  train_generator
    return x[0][0], x[0][1] 

# Import all the required Libraries
import tensorflow
import keras
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from keras.datasets import mnist
from keras.models import Model, Sequential
from keras.layers import Dense, Conv2D, Dropout, BatchNormalization, Input, Reshape, Flatten, Deconvolution2D, Conv2DTranspose, MaxPooling2D, UpSampling2D
from keras.layers.advanced_activations import LeakyReLU
from keras.optimizers import adam

#ENCODER
inp = Input((28, 28,1))
e = Conv2D(32, (3, 3), activation='relu')(inp)
e = MaxPooling2D((2, 2))(e)
e = Conv2D(64, (3, 3), activation='relu')(e)
e = MaxPooling2D((2, 2))(e)
e = Conv2D(64, (3, 3), activation='relu')(e)
l = Flatten()(e)
l = Dense(49, activation='softmax')(l)#DECODER
d = Reshape((7,7,1))(l)
d = Conv2DTranspose(64,(3, 3), strides=2, activation='relu', padding='same')(d)
d = BatchNormalization()(d)
d = Conv2DTranspose(64,(3, 3), strides=2, activation='relu', padding='same')(d)
d = BatchNormalization()(d)
d = Conv2DTranspose(32,(3, 3), activation='relu', padding='same')(d)
decoded = Conv2D(1, (3, 3), activation='sigmoid', padding='same')(d)
ae = Model(inp, decoded)
ae.summary()

(train_images, train_labels), (test_images, test_labels) = mnist.load_data()
train_images = train_images.reshape((60000, 28, 28, 1))
test_images = test_images.reshape((10000, 28, 28, 1))# Normalize pixel values to be between 0 and 1
train_images, test_images = train_images / 255.0, test_images / 255.0

# compile it using adam optimizer
ae.compile(optimizer="adam", loss="mse")#Train it by providing training images
ae.fit(train_images, train_images, epochs=2)#IF you want to save the model
model_json = ae.to_json()
with open("model_tex.json", "w") as json_file:
    json_file.write(model_json)

ae.save_weights("model_tex.h5")
print("Saved model")

prediction = ae.predict(train_images, verbose=1, batch_size=100)# you can now display an image to see it is reconstructed well
x =prediction[0].reshape(28,28)
plt.imshow(x)

json_file = open('model_tex.json', 'r')
loaded_model_json = json_file.read()
json_file.close()
loaded_model = model_from_json(loaded_model_json)
# load weights into new model
loaded_model.load_weights("model_tex.h5")
print("Loaded model from disk")
y = loaded_model.predict(train_images, verbose=1, batch_size=10)

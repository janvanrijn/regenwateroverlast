#to run on grace:
#source activate myenv

import sys
import os
import json
sys.path.append(os.path.realpath('../auto_encoder/'))

from keras.models import model_from_json

import load_random_tif as lrt
from plotters import acc_plot_dims, auto_encoder_plot
from auto_encoder_train import auto_encoder_train,build_well_posed_dense_ae,build_well_posed_cnn_ae
from keras.datasets import mnist
import numpy as np
import pandas as pd
from scipy import ndimage

def normalize_data(x_train,x_test,x_val,the_min=None,the_max=None):
    '''
        This function scales the data so that all it's values lie between 0 and 1. If the_min and the_max are defined, these values are used as scalars during the normalization process, if they are not defined (i.e. None), the maximum and minimum values of the union of the x_train, x_test and x_val set are defined to be the_max and the_min.
        ---arguments---
        x_train: 3d numpy array, train set containing 2d images
        x_test: 3d numpy array, test set containing 2d images
        x_val: 3d numpy array, validation set containing 2d images
        the_min: float, minimum value expected to be found in the data. This is mapped to 0. If it is None, the minium is taken from the union of the data sets.
        the_max: float, maximum value expected to be found in the data. This is mapped to 1. If it is None, the maximum is taken from the union of the data sets.
        
        ---returns---
        x_train: 3d numpy array, normalized x_train set
        x_test: 3d numpy array, normalized x_test set
        x_val: 3d numpy array, normalized x_val set
        the_min: float, In case the_min is defined during this function call, it needs to be returned.
        the_max: float, In case the_max is defined during this function call, it needs to be returned.
    '''
    if the_max==None:
        the_max = np.max([np.max(x_train), np.max(x_test), np.max(x_val)])
    if the_min==None:
        the_min = np.min([np.min(x_train), np.min(x_test), np.min(x_val)])
    x_train = (x_train-the_min)/(the_max-the_min)
    x_test = (x_test-the_min)/(the_max-the_min)
    x_val = (x_val-the_min)/(the_max-the_min)
    return x_train,x_test,x_val,the_max,the_min

def do_sobel(x):
    '''
        This function performs a sobel operation on a set of images. A sobel operation calculates the amount of change between neighboring pixels in the x and y direction. These x and y components represent the gradient per pixel. The magnitude of these x and y components is calculated and saved in one image. This magnitude represents the amount of change for each pixel, or in other words the magnitude of the gradient, or edge detection. 
        ---arguments---
        x: 3d numpy array, containing a set of 2d images
        ---returns---
        x: 3d numpy array, contains input x where the sobel operation is performed upon.
    '''
    kx = np.array([[1,0,-1],[2,0,-2],[1,0,-1]])
    ky = np.array([[1,2,1] ,[0,0,0], [-1,-2,-1]])
    for i in range(len(x)):
        sx=ndimage.convolve(x[i],kx)
        sy=ndimage.convolve(x[i],ky)
        x[i]=np.hypot(sx,sy)
    return x

def do_sobel_components(x):
    '''
        Just like the function do_sobel(), this function performs a sobel operation, but it skips the step where it calculates the magnitude of the components. This way it returns the gradient of the input. Note that in the output, each input image is represented by 2 output images, since the gradient has 2 components.
        ---arguments---
        x: 3d numpy array, containing a set of 2d images
        ---returns---
        x: 4d numpy array, containing a set of pairs of 2d images
    '''
    kx = np.array([[1,0,-1],[2,0,-2],[1,0,-1]])
    ky = np.array([[1,2,1] ,[0,0,0], [-1,-2,-1]])
    x_out = []
    for i in range(len(x)):
        sx=ndimage.convolve(x[i],kx)
        sy=ndimage.convolve(x[i],ky)
        x_out.append(np.dstack((sx,sy)))
    x_out = np.array(x_out)
    return x_out
    
def preprocessing(x_train,x_test,x_val,parameters,the_min=None,the_max=None):
    '''
        This function performs various steps of preprocessing on the x_train, x_test and x_val sets. 
        ---arguments---
        x_train: 3d numpy array, train set containing 2d images
        x_test: 3d numpy array, test set containing 2d images
        x_val: 3d numpy array, validation set containing 2d images
        parameters: dictionary containing various settings that define which preprocessing steps must be performed and how they should be performed
        the_min: float, This value is used by normalize_data()
        the_max:float, This value is used by normalize_data()
        --returns---
        x_train: (in case do_sobel_components() is called 4d numpy array, containing a set of pairs of 2d images, else 3d numpy array, containing a set of 2d images), preprocessed train set
        x_test: (in case do_sobel_components() is called 4d numpy array, containing a set of pairs of 2d images, else 3d numpy array, containing a set of 2d images), preprocessed test set
        x_val: (in case do_sobel_components() is called 4d numpy array, containing a set of pairs of 2d images, else 3d numpy array, containing a set of 2d images), preprocessed validation set
        the_min: float, In case the_min is defined during the function normalize_data() call, it needs to be returned.
        the_max: float, In case the_max is defined during the function normalize_data() call, it needs to be returned.
    '''
    if parameters['clamp']:
        np.clip(x_train,parameters['lc'],parameters['uc'],x_train)
        np.clip(x_test,parameters['lc'],parameters['uc'],x_test)
        np.clip(x_val,parameters['lc'],parameters['uc'],x_val)
    
    if parameters['filter'] == 'identity':
        pass#do nothing
    elif parameters['filter'] == 'sobel':
        x_train = do_sobel(x_train)
        x_test = do_sobel(x_test)
        x_val = do_sobel(x_val)
    elif parameters['filter'] == 'sobel_components':
        x_train = do_sobel_components(x_train)
        x_test = do_sobel_components(x_test)
        x_val = do_sobel_components(x_val)
    elif parameters['filter'] == 'all':
        pass #perform all filters in parallel and pass whole stack to NN
    else:
        print('unknown filter type')
    
    if parameters['normalize_data']:
        #normalize and format data such that model can handle it
        x_train, x_test, x_val, the_max, the_min = normalize_data(x_train,x_test,x_val,the_min=the_min,the_max=the_max)
    #TODO save the_max and the_min
    
    if parameters['encoder_type'] == 'simple' or parameters['encoder_type'] == 'well_posed_dense':
        #flatten each picture to 1D because we use a very simple network
        x_train = np.array([i.flatten() for i in x_train])
        x_test = np.array([i.flatten() for i in x_test])
        x_val = np.array([i.flatten() for i in x_val])
    elif parameters['encoder_type']=='shallow_cnn' or parameters['encoder_type']=='deep_cnn' or parameters['encoder_type']=='blur' or parameters['encoder_type']=='raw' or parameters['encoder_type']=='shallow_narrow_cnn' or parameters['encoder_type']=='well_posed_cnn' or parameters['encoder_type']=='advanced' or parameters['encoder_type']=='surajit':
        if not parameters['filter'] == 'sobel_components':
            x_train = np.reshape(x_train, (len(x_train), parameters['y'], parameters['x'], 1))
            x_test = np.reshape(x_test, (len(x_test), parameters['y'], parameters['x'], 1))
            x_val = np.reshape(x_val, (len(x_val), parameters['y'], parameters['x'], 1))
    else:
        print('unknown autoencoder type class_experiment()')
        exit()
    the_min = float(the_min)
    the_max = float(the_max)
    return x_train, x_test, x_val,the_min,the_max

class Experiment(object):
    def __init__(self,parameters):
        '''
            This class builds an experiment that can be ran while saving its results. It can load its own results. An experiment trains a series of autoencoders and measures their performance.
            ---arguments---
            self: an instance of the Experiments object
            parameters: a dictionary containing a set of parameters that define the experiment
        '''
        self.parameters = parameters
        self.load_data = self.select_data_loader()
        self.run_exp = self.select_experiment()
        self.x_train = None
        self.x_test = None
        self.x_val = None
        self.run_lock = self.define_run_lock()
    
    def define_run_lock(self):
        '''
            A run lock is defined to make it harder to run an experiment that allready ran before. The aim is to protect data that might have taken a lot of computing resources to generate. The function request_run() uses this run lock to check if the experiment was run before. Directly calling run_exp() ignores this run lock completely, thereby circumventing this extra layer of protection.
            ---arguments---
            self: an instance of the Experiments object
            ---returns---
            run_lock: boolean value that defines if the experiment was allready performed before
        '''
        try:
            os.makedirs(self.parameters['results_folder'])
        except:
            pass
        run_lock = False
        try:
            with open(self.parameters['results_folder']+'experiment_ran.json') as fp:
                for line in fp:
                    pass
            run_lock = True
        except:
            with open(self.parameters['results_folder']+'experiment_ran.json', 'w') as fp:
                json.dump([], fp)
        return run_lock
    
    def select_data_loader(self):
        '''
            This function select which function needs to be used to load the data
            ---arguments---
            self: an instance of the Experiments object
            ---returns---
            data_loader: a python function that loads the appropriate data and returns a train, test and validation set
        '''
        if self.parameters['data_set'] == 'AHN2_5m_random':
            def data_loader():
                x_train, x_test, x_val = lrt.load_random_tif(self.parameters)
                return x_train, x_test, x_val
        elif self.parameters['data_set'] == 'MNIST':
            def data_loader():
                (x_train, _), (x_test, _) = mnist.load_data()
                x_val = x_test#TODO we don't have a validation set yet
                if self.parameters['downsampling']:
                    pass#TODO downsample by parameters['downsample_factor']
                return x_train, x_test, x_val
        else:
            print('This dataset is unknown and can not be loaded.')
            exit()
        return data_loader

    def select_experiment(self):
        '''
            This function select which function needs to be used to run the experiment
            ---arguments---
            self: an instance of the Experiments object
            ---returns---
            exp: a python function that performs the experiment
        '''
        if self.parameters['experiment_type'] == 'varying_encoding_dimensions':
            def exp():
                self.x_train, self.x_test, self.x_val = self.load_data()
                self.x_train, self.x_test, self.x_val,self.parameters['the_min'],self.parameters['the_max'] = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters)
                
                counter = 0
                for i in self.parameters['dims']:
                    print("experiment no. " + str(counter))
                    self.parameters['encoding_dim'] = i
                    self.parameters['file_name'] = 'dims_' + str(i) + '_'
                    autoencoder, encoder, history = auto_encoder_train(self.parameters,data=(self.x_train, self.x_test, self.x_val))
                    counter += 1
                    for i in history.my_hist:
                        self.parameters[i] = [np.double(j) for j in history.my_hist[i]]
                    self.save_exp(self.parameters,autoencoder, encoder)
        elif self.parameters['experiment_type'] == 'varying_pooling_factor':
            def exp():
                self.x_train, self.x_test, self.x_val = self.load_data()
                self.x_train, self.x_test, self.x_val,self.parameters['the_min'],self.parameters['the_max'] = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters)
                
                counter = 0
                for i in self.parameters['pooling_factor_vars']:
                    print("experiment no. " + str(counter))
                    self.parameters['pooling_factor'] = i
                    self.parameters['file_name'] = 'pool_' + str(i) + '_'
                    autoencoder, encoder, history = auto_encoder_train(self.parameters,data=(self.x_train, self.x_test, self.x_val))
                    counter += 1
                    for i in history.my_hist:
                        self.parameters[i] = [np.double(j) for j in history.my_hist[i]]
                    self.save_exp(self.parameters,autoencoder, encoder)
        elif self.parameters['experiment_type'] == 'deep_pooling':
            def exp():
                self.x_train, self.x_test, self.x_val = self.load_data()
                self.x_train, self.x_test, self.x_val,self.parameters['the_min'],self.parameters['the_max'] = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters)
                
                counter = 0
                for i in self.parameters['deep_pooling_vars']:
                    print("experiment no. " + str(counter))
                    self.parameters['pooling_var'] = i
                    self.parameters['file_name'] = 'deep_pool_' + str(i) + '_'
                    autoencoder, encoder, history = auto_encoder_train(self.parameters,data=(self.x_train, self.x_test, self.x_val))
                    counter += 1
                    for i in history.my_hist:
                        self.parameters[i] = [np.double(j) for j in history.my_hist[i]]
                    self.save_exp(self.parameters,autoencoder, encoder)
        elif self.parameters['experiment_type'] == 'blurring':
            def exp():
                self.x_train, self.x_test, self.x_val = self.load_data()
                self.x_train, self.x_test, self.x_val,self.parameters['the_min'],self.parameters['the_max'] = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters)

                counter = 0
                for i in self.parameters['blurring_vars']:
                    print("experiment no. " + str(counter))
                    self.parameters['kernel_size'] = i
                    self.parameters['file_name'] = 'blurring_' + str(i) + '_'
                    autoencoder, encoder, history = auto_encoder_train(self.parameters,data=(self.x_train, self.x_test, self.x_val))
                    counter += 1
                    self.save_exp(self.parameters,autoencoder, encoder)
        elif self.parameters['experiment_type'] == 'raw_filtering':
            def exp():
                self.x_train, self.x_test, self.x_val = self.load_data()
                self.x_train, self.x_test, self.x_val,self.parameters['the_min'],self.parameters['the_max'] = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters)
                
                counter = 0
                for i in self.parameters['filtering_vars']:
                    print("experiment no. " + str(counter))
                    self.parameters['kernel_size'] = i
                    self.parameters['file_name'] = 'raw_' + str(i) + '_'
                    autoencoder, encoder, history = auto_encoder_train(self.parameters,data=(self.x_train, self.x_test, self.x_val))
                    counter += 1
                    self.save_exp(self.parameters,autoencoder, encoder)
        elif self.parameters['experiment_type'] == 'varying_depth':
            def exp():
                self.x_train, self.x_test, self.x_val = self.load_data()
                self.x_train, self.x_test, self.x_val,self.parameters['the_min'],self.parameters['the_max'] = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters)
                
                counter = 0
                for i in self.parameters['depth_vars']:
                    print("experiment no. " + str(counter))
                    self.parameters['multiple'] = i
                    self.parameters['file_name'] = 'advanced_' + str(i) + '_'
                    autoencoder, encoder, history = auto_encoder_train(self.parameters,data=(self.x_train, self.x_test, self.x_val))
                    counter += 1
                    for i in history.my_hist:
                        self.parameters[i] = [np.double(j) for j in history.my_hist[i]]
                    self.save_exp(self.parameters,autoencoder, encoder)
        else:
            print("unknown experiment type")
            exit()
        return exp

    def load_models(self,parameters,file_name,x_train,x_test,x_val):
        '''
            This function loads an autoencoder from the harddrive that was previously saved during the run of the experiment
            ---arguments---
            self: an instance of the Experiments object
            parameters: dictionary containing various parameters that define the experiment
            file_name: string containing path to folder where encoder and autoencoder are saved
            x_train: (in case of sobel components, 4d numpy array, containing a set of pairs of 2d images, else 3d numpy array, containing a set of 2d images), preprocessed train set
            x_test: (in case of sobel components, 4d numpy array, containing a set of pairs of 2d images, else 3d numpy array, containing a set of 2d images), preprocessed test set
            x_val: (in case of sobel components, 4d numpy array, containing a set of pairs of 2d images, else 3d numpy array, containing a set of 2d images), preprocessed validation set
            ---returns---
            autoencoder: trained and compiled autoencoder loaded from disc. It's output is trained to closely match the input.
            encoder: trained and compiled encoder section of the autoencoder loaded from disc. When fed an input, it outputs the learned features.
        '''
        json_file = open(file_name + 'encoder.json', 'r')
        encoder_json = json_file.read()
        json_file.close()
        encoder = model_from_json(encoder_json)
        # load weights into new autoencoder
        encoder.load_weights(file_name +"encoder.h5")
        
        if not (parameters['encoder_type'] == 'well_posed_dense' or parameters['encoder_type'] =='well_posed_cnn'):
            # load json and create model
            json_file = open(file_name + 'autoencoder.json', 'r')
            loaded_autoencoder_json = json_file.read()
            json_file.close()
            autoencoder = model_from_json(loaded_autoencoder_json)
            # load weights into new autoencoder
            autoencoder.load_weights(file_name +"autoencoder.h5")
            autoencoder.compile(optimizer=parameters['optimizer'], loss=parameters['loss'],metrics=['accuracy'])
        else: 
            if parameters['encoder_type'] == 'well_posed_dense':
                autoencoder, _ = build_well_posed_dense_ae(parameters,x_train.shape)
            elif parameters['encoder_type'] == 'well_posed_cnn':
                autoencoder, _ = build_well_posed_cnn_ae(parameters,x_train.shape)
            else:
                print("unknown encoder type in class_experiment.load_models()")
            autoencoder.set_weights(encoder.get_weights()) 
            #for i in range(len(encoder.layers)):
            #    autoencoder.layers[i] = encoder.layers[i]
            autoencoder.compile(optimizer=parameters['optimizer'], loss=parameters['loss'],metrics=['accuracy'])
        #autoencoder.compile(metrics=['accuracy'],
        #                    loss='mean_squared_error',
        #                    optimizer='sgd')

        #print(autoencoder.layers[1].kernel)
        #print(tensorflow.dtypes.cast(autoencoder.layers[2].kernel,dtype=tensorflow.python.ops.variables.Variable))

        #json_file = open(file_name + 'decoder.json', 'r')
        #decoder_json = json_file.read()
        #json_file.close()
        #decoder = model_from_json(decoder_json)
        # load weights into new autoencoder
        #decoder.load_weights(file_name +"decoder.h5")

        print("Loaded autoencoder from disk")
        return autoencoder, encoder

    def load_exp(self):
        '''
            This function loads the results of the experiment that was ran previously
            
            ---arguments---
            self: an instance of the Experiments object
            ---returns---
            Does not return anything
        '''
        self.x_train,self.x_test,self.x_val = self.load_data()
        if self.parameters['experiment_type'] == 'varying_encoding_dimensions':
            history_dict = {}
            for i in self.parameters['dims']:
                file_name = self.parameters['results_folder']+'dims_' + str(i) + '_history.json'
                with open(file_name) as f:
                    for line in f:
                        history = json.loads(line)
                history_dict[str(i)]=history
            
            self.parameters['history'] = history_dict
            
            the_min = self.parameters['history']['the_min']
            the_max = self.parameters['history']['the_max']
            self.x_train,self.x_test,self.x_val,_,_ = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters,the_min=the_min,the_max=the_max)
            acc_plot_dims(parameters=self.parameters,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name'])
            
            for i in self.parameters['dims']:
                self.parameters['encoding_dim']= i
                file_name = self.parameters['results_folder']+'dims_' + str(i) + '_'
                autoencoder, encoder = self.load_models(self.parameters,file_name,self.x_train,self.x_test,self.x_val)
                auto_encoder_plot(self.parameters,self.x_train,self.x_test,self.x_val,encoder,autoencoder,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name']+"_dim_"+str(i))
        elif self.parameters['experiment_type'] == 'varying_pooling_factor':
            history_dict = {}
            for i in self.parameters['pooling_factor_vars']:
                file_name = self.parameters['results_folder']+'pool_' + str(i) + '_history.json'
                with open(file_name) as f:
                    for line in f:
                        history = json.loads(line)
                history_dict[str(i)]=history
                the_min = history['the_min']
                the_max = history['the_max']
            
            self.parameters['history'] = history_dict
            
            self.x_train,self.x_test,self.x_val,_,_ = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters,the_min=the_min,the_max=the_max)
            acc_plot_dims(parameters=self.parameters,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name'])
            
            for i in self.parameters['pooling_factor_vars']:
                self.parameters['pooling_factor']= i
                file_name = self.parameters['results_folder']+'pool_' + str(i) + '_'
                autoencoder, encoder = self.load_models(self.parameters,file_name,self.x_train,self.x_test,self.x_val)
                auto_encoder_plot(self.parameters,self.x_train,self.x_test,self.x_val,encoder,autoencoder,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name']+"_pool_"+str(i))
        elif self.parameters['experiment_type'] == 'deep_pooling':
            history_dict = {}
            for i in self.parameters['deep_pooling_vars']:
                file_name = self.parameters['results_folder']+'deep_pool_' + str(i) + '_history.json'
                with open(file_name) as f:
                    for line in f:
                        history = json.loads(line)
                history_dict[str(i)]=history
                        
            self.parameters['history'] = history_dict
            
            the_min = self.parameters['history']['the_min']
            the_max = self.parameters['history']['the_max']
            self.x_train,self.x_test,self.x_val,_,_ = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters,the_min=the_min,the_max=the_max)
            acc_plot_dims(parameters=self.parameters,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name'])
            
            for i in self.parameters['deep_pooling_vars']:
                self.parameters['pooling_factor']= i
                file_name = self.parameters['results_folder']+'deep_pool_' + str(i) + '_'
                autoencoder, encoder = self.load_models(self.parameters,file_name,self.x_train,self.x_test,self.x_val)
                auto_encoder_plot(self.parameters,self.x_train,self.x_test,self.x_val,encoder,autoencoder,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name']+"_deep_pool_"+str(i))
        elif self.parameters['experiment_type'] == 'blurring':
            history_dict = {}
            for i in self.parameters['blurring_vars']:
                file_name = self.parameters['results_folder']+'blurring_' + str(i) + '_history.json'
                with open(file_name) as f:
                    for line in f:
                        history = json.loads(line)
                history_dict[str(i)]=history
                the_min = history['the_min']
                the_max = history['the_max']
            
            self.parameters['history'] = history_dict
            
            self.x_train,self.x_test,self.x_val,_,_ = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters,the_min=the_min,the_max=the_max)
            
            for i in self.parameters['blurring_vars']:
                self.parameters['kernel']= i
                file_name = self.parameters['results_folder']+'blurring_' + str(i) + '_'
                autoencoder, encoder = self.load_models(self.parameters,file_name,self.x_train,self.x_test,self.x_val)
                auto_encoder_plot(self.parameters,self.x_train,self.x_test,self.x_val,encoder,autoencoder,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name']+"_blurring_"+str(i))
        elif self.parameters['experiment_type'] == 'raw_filtering':
            history_dict = {}
            for i in self.parameters['filtering_vars']:
                file_name = self.parameters['results_folder']+'raw_' + str(i) + '_history.json'
                with open(file_name) as f:
                    for line in f:
                        history = json.loads(line)
                history_dict[str(i)]=history
                the_min = history['the_min']
                the_max = history['the_max']
            
            self.parameters['history'] = history_dict
            
            self.x_train,self.x_test,self.x_val,_,_ = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters,the_min=the_min,the_max=the_max)
            for i in self.parameters['filtering_vars']:
                self.parameters['kernel']= i
                file_name = self.parameters['results_folder']+'raw_' + str(i) + '_'
                autoencoder, encoder = self.load_models(self.parameters,file_name,self.x_train,self.x_test,self.x_val)
                auto_encoder_plot(self.parameters,self.x_train,self.x_test,self.x_val,encoder,autoencoder,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name']+"_raw_"+str(i))
        elif self.parameters['experiment_type'] == 'varying_depth':
            history_dict = {}
            for i in self.parameters['depth_vars']:
                print("Depth var: " +str(i))
                file_name = self.parameters['results_folder']+'advanced_' + str(i) + '_history.json'
                with open(file_name) as f:
                    for line in f:
                        history = json.loads(line)
                        history_dict[str(i)]=history
                
            self.parameters['history'] = history_dict
            
            the_min = self.parameters['history']['the_min']
            the_max = self.parameters['history']['the_max']
            self.x_train,self.x_test,self.x_val,_,_ = preprocessing(self.x_train,self.x_test,self.x_val,self.parameters,the_min=the_min,the_max=the_max)
            acc_plot_dims(parameters=self.parameters,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name'])
            for i in self.parameters['depth_vars']:
                print("Depth var: " +str(i))
                self.parameters['multiple']= i
                file_name = self.parameters['results_folder']+'advanced_' + str(i) + '_'
                autoencoder, encoder = self.load_models(self.parameters,file_name,self.x_train,self.x_test,self.x_val)
            auto_encoder_plot(self.parameters,self.x_train,self.x_test,self.x_val,encoder,autoencoder,xlabel="epochs",ylabel="test_loss",title=self.parameters['experiment_name']+"_advanced_"+str(i))
        else:
            print('unknown experiment type')
        

    def save_exp(self,parameters,autoencoder, encoder):
        '''
            This function saves the outcome of the experiment. It saves the trained autoencoders and decoders to disc, this consists of their topology as well as their weights. It saves the history of the training of the autoencoders.
            
            ---arguments---
            self: an instance of the Experiments object
            parameters: dictionary containing various parameters that define the experiment
            autoencoder: compiled and trained autoencoder fitted on x_train. It's output is trained to closely match the input.
            encoder: compiled and trained encoder section of the autoencoder fitted on x_train. When fed an input, it outputs the learned features.
            parameters: 
            ---returns---
            Does not return anything
        '''
        with open(parameters['results_folder']+parameters['file_name']+'history.json', 'w') as fp:
            json.dump(parameters, fp)
        
        if not (parameters['encoder_type'] == 'well_posed_dense'):
            # serialize autoencoder to JSON
            autoencoder_json = autoencoder.to_json()
            with open(parameters['results_folder']+parameters['file_name']+"autoencoder.json", "w") as json_file:
                json_file.write(autoencoder_json)
            # serialize weights to HDF5
            autoencoder.save_weights(parameters['results_folder']+parameters['file_name']+"autoencoder.h5")

        encoder_json = encoder.to_json()
        with open(parameters['results_folder']+parameters['file_name']+"encoder.json", "w") as json_file:
            json_file.write(encoder_json)
        # serialize weights to HDF5
        encoder.save_weights(parameters['results_folder']+parameters['file_name']+"encoder.h5")
            
        #TODO reengage decoder
        #decoder_json = decoder.to_json()
        #with open(parameters['results_folder']+parameters['file_name']+"decoder.json", "w") as json_file:
        #    json_file.write(decoder_json)
        # serialize weights to HDF5
        #decoder.save_weights(parameters['results_folder']+parameters['file_name']+"decoder.h5")
            
        print("Saved autoencoder to disk")
    
    def request_run(self):
        '''
            A run lock is defined to make it harder to run an experiment that allready ran before. The aim is to protect data that might have taken a lot of computing resources to generate. The function request_run() uses this run lock to check if the experiment was run before. Directly calling run_exp() ignores this run lock completely, thereby circumventing this extra layer of protection.
            ---arguments---
            self: an instance of the Experiments object
            ---returns---
            Does not return anything
        '''
        if self.run_lock:
            while True:
                answer = input("Experiment allready did run, run again? <y/n>")
                if answer == 'y':
                    break
                elif answer == 'n':
                    exit()
        self.run_exp()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("usuage: python3 experiment_script.py <run/load> (<trit/grace>)")
        exit()
    if sys.argv[1] == 'run':
        run = True
    elif sys.argv[1] == 'load':
        run = False
    else:
        print("usuage: python3 experiment_script.py <run/load> (<trit/grace>)")
        exit()
    trit = len(sys.argv) >= 3 and sys.argv[2] == 'trit'
    grace = len(sys.argv) >= 3 and sys.argv[2] == 'grace'
    print("trit: " + str(trit))
    print("grace: " + str(grace))
    
    #parameters needed for all experiments
    parameters = {}
    parameters['experiment_name'] = 'class_experiment'
    parameters['experiment_type'] = 'varying_encoding_dimensions'
    parameters['encoder_type'] = 'advanced'
    #parameters['encoding_dim'] = 400 #encoding dimensions are varied for this experiment
    parameters['epochs'] = 3
    parameters['batch_size'] = 256
    parameters['x'] = 200
    parameters['y'] = 200
    parameters['data_set'] = 'AHN2_5m_random'
    parameters['normalize_data'] = True
    parameters['train_part'] = 50#5000
    parameters['test_part'] = 10#1000
    parameters['val_part'] = 10#1000
    if run: 
        parameters['train_part'] = 5000
        parameters['test_part'] = 1000
        parameters['val_part'] = 1000
    else:
        parameters['train_part'] = 500
        parameters['test_part'] = 100
        parameters['val_part'] = 100
    parameters['n_data'] = parameters['train_part']+parameters['test_part']+parameters['val_part']
    parameters['print_results'] = False
    parameters['seed'] = 42
    #parameters['file_name'] = 'test_' #each iteration gets its own file name for this experiment
    parameters['clamp'] = True
    parameters['uc'] = 210 #upper clamp value
    parameters['lc'] = -15 #lower clamp value
    parameters['optimizer'] = 'adadelta'
    parameters['loss'] = 'binary_crossentropy'
    #parameters['filter'] = 'identity'
    parameters['filter'] = 'sobel'
    parameters['downsampling'] = False #TODO do downsampling at data loading stage or in case of MNIST right after loading MNIST
    parameters['downsample_factor'] = 4
    
    if trit:
        parameters['data_folder'] = '../../../../data/s0315435/AHN2_5m/'
        parameters['results_folder'] = '../../../../data/s0315435/results/' + parameters['experiment_name'] + '/'
    elif grace:
        parameters['data_folder'] = '/scratch/lamers/AHN2_5m/'
        parameters['results_folder'] = '/scratch/lamers/results/' + parameters['experiment_name'] + '/'
    else:
        parameters['data_folder'] = '../../AHN2_5m/'
        parameters['results_folder'] = '../../results/' + parameters['experiment_name'] + '/'
    
    #parameters specific to this experiment
    parameters['dims'] = [20,40,80,160,320,400]
    parameters['pooling_factor_vars'] = [1,2,5,10]
    parameters['deep_pooling_vars'] = [10,5,2,1]
    parameters['filtering_vars'] = [4,8,20,40]
    parameters['depth_vars'] = [2,3,4,5,6]
    
    experiment = Experiment(parameters)
    
    if run:
        experiment.request_run()
    else:
        experiment.load_exp()

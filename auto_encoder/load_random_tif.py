from osgeo import osr, gdal
import numpy as np
import matplotlib.pyplot as plt
from numpy.random import randint
from os import listdir
from os.path import isfile, join
from tqdm import tqdm
import sys

def downsample(img,downsample_factor):
    return img

def load_random_tif(parameters):
    '''
        This function samples images of parameters['x'] by parameters['y'] pixels from a folder specified by parameters['data_folder']. Samples are always taken from one file at a time, so a sample never contains information from two neighboring regions. This is done for simplicity. The test, train and validation set are sampled from a non overlapping sets of files.
        ---arguments---
        parameters: dictionary containing various parameters needed to load the data
        
        ---returns---
        train: numpy array of the train set. it contains image data.
        test: numpy array of the test set. it contains image data.
        val: numpy array of the validation set. it contains image data.
    '''
    np.random.seed(parameters['seed'])
    folder = parameters['data_folder']
    x = parameters['x']
    y = parameters['y']
    onlyfiles = [f for f in listdir(folder) if isfile(join(folder, f))]
    tif_files = [f for f in onlyfiles if '.tif' in f]
    
    np.random.shuffle(tif_files)
    the_sum = parameters['test_part']+parameters['train_part']+parameters['val_part']
    train_split = parameters['train_part'] * len(tif_files) // the_sum
    test_split = train_split + (parameters['test_part'] * len(tif_files) // the_sum)
    
    if train_split == test_split:
        train_split -= 1
    
    train_i = parameters['train_part'] * parameters['n_data'] // the_sum
    test_i = train_i + (parameters['test_part'] * parameters['n_data'] // the_sum)
    
    tif_train = tif_files[0:train_split]
    tif_test = tif_files[train_split:test_split]
    tif_val = tif_files[test_split:len(tif_files)]
    
    train = []
    test = []
    val = []
    print('loading data')
    for i in tqdm(range(parameters['n_data'])):
        if i < train_i:
            while True:
                file_name = folder + tif_train[randint(len(tif_train))]
                ds = gdal.Open(file_name, gdal.GA_ReadOnly)
                rb = ds.GetRasterBand(1)
                imarray = rb.ReadAsArray()
                del ds
                
                if imarray.shape[0] > y and imarray.shape[1] > x:
                    x_start = randint(imarray.shape[1] - x)
                    y_start = randint(imarray.shape[0] - y)
                    img = imarray[y_start:y_start+y,x_start:x_start+x]
                    if parameters['downsampling']:
                        img = downsample(img,parameters['downsample_factor'])
                    train.append(img)
                    break
        elif i >= train_i and i < test_i:
            while True:
                file_name = folder + tif_test[randint(len(tif_test))]
                ds = gdal.Open(file_name, gdal.GA_ReadOnly)
                rb = ds.GetRasterBand(1)
                imarray = rb.ReadAsArray()
                del ds
                
                if imarray.shape[0] > y and imarray.shape[1] > x:
                    x_start = randint(imarray.shape[1] - x)
                    y_start = randint(imarray.shape[0] - y)
                    img = imarray[y_start:y_start+y,x_start:x_start+x]
                    if parameters['downsampling']:
                        img = downsample(img,parameters['downsample_factor'])
                    test.append(img)
                    break
        else:
            while True:
                file_name = folder + tif_val[randint(len(tif_val))]
                ds = gdal.Open(file_name, gdal.GA_ReadOnly)
                rb = ds.GetRasterBand(1)
                imarray = rb.ReadAsArray()
                del ds
                
                if imarray.shape[0] > y and imarray.shape[1] > x:
                    x_start = randint(imarray.shape[1] - x)
                    y_start = randint(imarray.shape[0] - y)
                    img = imarray[y_start:y_start+y,x_start:x_start+x]
                    if parameters['downsampling']:
                        img = downsample(img,parameters['downsample_factor'])
                    val.append(img)
                    break
    return np.array(train), np.array(test), np.array(val)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("usuage: python3 load_random_tif.py <path to data folder>")
        exit()
    parameters = {}
    parameters['type'] = 'simple'
    parameters['encoding_dim'] = 20
    parameters['epochs'] = 10
    parameters['batch_size'] = 256
    parameters['x'] = 200
    parameters['y'] = 200
    parameters['data_sampling'] = 'random'
    parameters['normalize_data'] = True
    parameters['data_folder'] = sys.argv[1]
    parameters['train_part'] = 500
    parameters['test_part'] = 2
    parameters['val_part'] = 2
    parameters['n_data'] = parameters['train_part']+parameters['test_part']+parameters['val_part']
    parameters['print_results'] = True
    parameters['seed'] = 42
    
   
    
    min_val = -15
    max_val = 210
    train, test,val = load_random_tif(parameters)
    
    np.clip(train,min_val,max_val,train)
    np.clip(test,min_val,max_val,test)
    np.clip(val,min_val,max_val,val)

    x = train.flatten()
    num_bins = 50
    n, bins, patches = plt.hist(x, num_bins, facecolor='blue', alpha=0.5)
    plt.show()
    exit()

    print(train.shape)
    print(test.shape)
    print(val.shape)
    print('train')
    for i in train:
        print(i)
        plt.imshow(i)
        plt.gray()
        plt.show()
    print('test')
    for i in test:
        print(i)
        plt.imshow(i)
        plt.gray()
        plt.show()
    print('val')
    for i in val:
        print(i)
        plt.imshow(i)
        plt.gray()
        plt.show()
        


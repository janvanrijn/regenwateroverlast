import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import json
sys.path.append(os.path.realpath('../experiments/'))
from class_experiment import normalize_data,do_sobel,preprocessing
sys.path.append(os.path.realpath('../map_visualisation/'))
import plot_maps
sys.path.append(os.path.realpath('./'))
import pandafy_radar_coord
from keras.models import model_from_json
from sklearn.metrics import confusion_matrix
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier
import ast
from sklearn.model_selection import train_test_split
from tqdm import tqdm
from joblib import Parallel, delayed
import multiprocessing
from copy import copy
import ctypes
import threading

def filter_tweets(data,search_terms):
    i = 0
    while i < len(data.index):
        #print(i,len(data.index))
        if not any([j in data['text'][data.index[i]] for j in search_terms]):
            data = data.drop(data.index[i])
        else:
            i += 1
    return data
def make_features_panda_friendly(data):
    '''
        When a pandas dataframe is loaded from disk, lists of lists are represented as strings, which is a problem. This function patches this problem. Unfortunately it is very slow. If a better method exists, please use it.
    '''
    X=[]
    print("make_features_panda_friendly():")
    for i in tqdm(data['features']):
        #i_numpy = ','.join(i.replace('[ ', '[').split())
        i_numpy = np.array(ast.literal_eval(i),dtype=np.float32)
        X.append(i_numpy)
        #X.append(i_numpy.tolist())
    data['features'] = X
    return data

def make_XY_just_radar(data):
    y=data['labels'].to_numpy(dtype=np.bool)
    X = np.array(data['features'].values.tolist(),dtype=np.float32)
    X = np.append(X, np.array(data['radarX'].values.tolist(),dtype=np.float32).reshape((len(data['radarX']), 1)), axis=1)
    X = np.append(X, np.array(data['radarY'].values.tolist(),dtype=np.float32).reshape((len(data['radarY']), 1)), axis=1)
    X = np.append(X, np.array(data['rain'].values.tolist(),dtype=np.float32).reshape((len(data['rain']), 1)), axis=1)
    return X,y
    
def make_Xy(data):
    print("make Xy 1")
    y=data['labels'].to_numpy(dtype=np.bool)
    print("make Xy 2")
    d = len(data.index)
    w = len(data['features'].values.tolist()[0]) + 1
    print("make XY 2.1")
    X=np.zeros(shape=(d,w),dtype=np.float32)
    print("make XY 2.2")
    X[:,0:w-1]=np.array(data['features'].values.tolist(),dtype=np.float32)
    print("make Xy 3")
    #X = np.multiply(X,100)
    print("make Xy 4")
    #X = np.array(X,dtype=np.int8)
    print("make Xy 5")
    #X = np.append(X, np.array(data['radarX'].values.tolist(),dtype=np.float32).reshape((len(data['radarX']), 1)), axis=1)
    print("make Xy 6")
    #X = np.append(X, np.array(data['radarY'].values.tolist(),dtype=np.float32).reshape((len(data['radarY']), 1)), axis=1)
    print("make Xy 7")
    X[:,w-1:w] = np.array(data['rain'].values.tolist(),dtype=np.float32).reshape((len(data['rain']), 1))
    print("make Xy 8")
    return X,y

def split_on_radarXY(data,test_size=0.33, random_state=42):
    print("split on radar 1")
    coord_only = data[['radarX','radarY']]
    coord_only = coord_only.drop_duplicates()
    coord_only = coord_only.reset_index(drop=True)
    print("split on radar 2")
    index = np.arange(len(coord_only.index))
    np.random.seed(random_state)
    np.random.shuffle(index)
    test_index = index[0:int(len(index)*test_size)]
    train_index = index[int(len(index)*test_size)+1:len(index)]
    test_coord = coord_only.iloc[test_index]
    train_coord = coord_only.iloc[train_index]
    print("split on radar 3")
    data_train = pd.merge(data, train_coord, on=('radarX','radarY'), how='inner').reset_index(drop=True)
    print("split on radar 4")
    data_test = pd.merge(data, test_coord, on=('radarX','radarY'), how='inner').reset_index(drop=True)
    print("split on radar 5")
    del data
    
    print("split on radar 6")
    y_train=data_train['labels'].to_numpy(dtype=np.bool)
    print("split on radar 7")
    y_test=data_test['labels'].to_numpy(dtype=np.bool)
    print("split on radar 8")
    
    d_train = len(data_train.index)
    d_test = len(data_test.index)
    w = len(data_train['features'].values.tolist()[0]) + 1
    print("split on radar 9")
    X_train=np.zeros(shape=(d_train,w),dtype=np.float32)
    X_test=np.zeros(shape=(d_test,w),dtype=np.float32)
    print("split on radar 10")
   
    X_train[:,0:w-1]=np.array(data_train['features'].values.tolist(),dtype=np.float32)
    X_test[:,0:w-1]=np.array(data_test['features'].values.tolist(),dtype=np.float32)
    
    print("split on radar 12")
    X_train[:,w-1:w] = np.array(data_train['rain'].values.tolist(),dtype=np.float32).reshape((len(data_train['rain']), 1))
    X_test[:,w-1:w] = np.array(data_test['rain'].values.tolist(),dtype=np.float32).reshape((len(data_test['rain']), 1))
    print("split on radar 12")
    return X_train, X_test, y_train, y_test

def make_labels(data,label_on):
    labels = []
    print("make_labels:")
    for i in tqdm(data[label_on]):
        try:
            if np.isnan(i):
                num=0
            else:
                num=1
        except:
            num=1
        labels.append(num)
    data['labels'] = labels
    return data

def load_encoder(file_name):
    json_file = open(file_name + '.json', 'r')
    encoder_json = json_file.read()
    json_file.close()
    encoder = model_from_json(encoder_json)
    # load weights into new autoencoder
    encoder.load_weights(file_name +".h5")
    #encoder.compile()
    return encoder

def have_tif_give_features(data,parameters,encoder=None,the_min=None,the_max=None):
    if encoder is None:
        encoder=load_encoder(parameters['encoder_file_name'])
    numpy_images = []
    for image in data['images'].to_numpy():
        numpy_images.append(ast.literal_eval(str(image)))
    numpy_images = np.array(numpy_images)
    numpy_images,_,_,_,_= preprocessing(numpy_images,np.array([]),np.array([]),parameters,the_min=the_min,the_max=the_max)
    features = encoder.predict(numpy_images)
    flat_features = []
    for feature in features:
        flat_features.append(feature.flatten())
    data['features'] = flat_features
    return data

def parallel_forest(i):
    """
        This function trains a Randomforest classifier containing only 1 tree. It allows for training multiple trees in parallel to be recombined into one model. Unfortunately this method does not work in the current state.
        
        ---arguments---
        i: index to global array all_ensembles, which holds all trained single tree random forests
    """
    model = RandomForestClassifier(n_estimators=1,max_depth=2,max_features=2).fit(X_train,y_train)
    model.fit(X_train,y_train)
    all_ensembles[i]=model

def combine(all_ensembles):
    """Combine the sub-estimators of a group of ensembles
        
        >>> from sklearn.datasets import load_iris
        >>> from sklearn.ensemble import ExtraTreesClassifier
        >>> iris = load_iris()
        >>> X, y = iris.data, iris.target
        
        >>> all_ensembles = [ExtraTreesClassifier(n_estimators=4).fit(X, y)
        ...                  for i in range(3)]
        >>> big = combine(all_ensembles)
        >>> len(big.estimators_)
        12
        >>> big.n_estimators
        12
        >>> big.score(X, y)
        1.0
        
        """
    final_ensemble = copy(all_ensembles[0])
    final_ensemble.estimators_ = []
    
    for ensemble in all_ensembles:
        final_ensemble.estimators_ += ensemble.estimators_
    
    # Required in old versions of sklearn
    final_ensemble.n_estimators = len(final_ensemble.estimators_)
    
    return final_ensemble

def load_pandafied(folder='../../pandafied_data/'):
    #rain = pd.read_csv(folder + 'pandafied_rain.csv')
    print("load 1")
    #rain = pd.read_csv(folder + 'pandafied_h5_rain.csv')
    rain = pd.read_csv(folder + 'pandafied_h5_rain_2007-2020.csv')
    print("load 2")
    #radar = pd.read_csv(folder + 'pandafied_h5_radar_img.csv')#[0:5000]
    #radar = pd.read_csv(folder + 'pandafied_h5_radar_img_raw.csv')
    radar = pd.read_csv(folder + 'pandafied_h5_radar_img_raw_NO_SOBEL.csv')
    #radar = pd.read_csv(folder + 'pandafied_h5_radar.csv')
    #file = pd.read_csv(folder + 'lat_lon_to_filename.csv')
    #tweets = pd.read_csv(folder + 'curated_twitter.csv')
    print("load 3")
    #tweets_XY = pd.read_csv(folder + 'pandafied_twitter_XY.csv')
    #tweets_XY = pd.read_csv(folder + 'curated_twitter_XY.csv')
    tweets_XY = pd.read_csv(folder + 'pandafied_twitter_2007-2020_XY.csv')
    #handpicked = pd.read_csv(folder + 'pandafied_handpicked.csv')
    return rain,radar,tweets_XY

def show_tweet_location(tweets_XY,radar):
    tweets_aggr = tweets_XY[['radarX','radarY','latlon','text']]
    tweets_aggr = pd.merge(tweets_aggr, radar, on=('radarX','radarY'), how='inner')
    tweets_aggr = pandafy_radar_coord.have_radar_give_tif(tweets_aggr,pre_process_img=True,Twitter_center=True)
    tweets_aggr.reset_index(drop=True)
    plt.gray()
    for i in tweets_aggr.index:
        print(tweets_aggr['text'][i])
        print()
        img = tweets_aggr['images'][i]
        img = img.reshape((img.shape[0],img.shape[1]))
        plt.imshow(img)
        plt.show()

def show_twitter_dense_area(tweets_XY,radar):
    tweets_aggr = tweets_XY[['radarX','radarY']]
    tweets_aggr['tweets_count'] = np.ones(len(tweets_aggr.index))
    tweets_aggr = tweets_aggr.groupby(['radarX','radarY'])['tweets_count'].sum().reset_index(name='tweets_count')
    tweets_aggr = tweets_aggr.sort_values('tweets_count', axis=0, ascending=False)
    tweets_aggr = tweets_aggr[tweets_aggr.tweets_count >= 5]
    tweets_aggr = pd.merge(tweets_aggr, radar, on=('radarX','radarY'), how='inner')
    print(tweets_aggr)
    tweets_aggr = pandafy_radar_coord.have_radar_give_tif(tweets_aggr,pre_process_img=True,Twitter_center=False)
    plt.gray()
    for img in tweets_aggr['images']:
        img = img.reshape((img.shape[0],img.shape[1]))
        plt.imshow(img)
        plt.show()

def equalize_data(data):
    '''
        Since there are orders of magnitude more negative samples than positive samples, this method makes the amount of negative samples the same as the amount of positive samples, by random sampling from the negative samples.
    '''
    pos_data = data[data.labels == 1]
    neg_data = data[data.labels == 0]
    print(len(pos_data))
    print(len(neg_data))
    data = pos_data
    data = data.append(neg_data.sample(n=len(pos_data),replace=False))
    print(len(data))
    return data.reset_index(drop=True)

def analyse_twitter(folder='../../pandafied_data/'):
    print(0)
    rain,radar,tweets_XY = load_pandafied(folder=folder)
    tweets_XY = tweets_XY[['radarX','radarY','date','text']]
    tweets_XY = tweets_XY[(tweets_XY.date >= 20100000) & (tweets_XY.date < 20191201)]
    tweets_XY_coord = tweets_XY[['radarX','radarY']]
    tweets_XY_coord = tweets_XY_coord.drop_duplicates()
    print(1)
    rain = rain[['date','radarX','radarY','rain']]
    print(2)
    rain = rain[(rain.rain >= 2000) & (rain.date >= 20100000) & (rain.date < 20191201)]
    rain = pd.merge(rain, tweets_XY_coord, on=('radarX','radarY'), how='inner')
    print("len(rain.index)")
    print(len(rain.index))
    print(rain)
    radar = radar[['radarX','radarY','features']]
    radar = make_features_panda_friendly(radar)
    print(4)
    radar_rain = pd.merge(rain, radar, on=('radarX','radarY'), how='inner')
    print("len(radar_rain.index)")
    print(len(radar_rain.index))
    print(radar_rain)
    del rain
    del radar
    print(4.5)
    radar_rain_tweets = pd.merge(radar_rain, tweets_XY, on=('radarX','radarY','date'), how='left')
    del radar_rain
    print(5)
    radar_rain_tweets = make_labels(radar_rain_tweets,'text')
    print("len(radar_rain_tweets.index)")
    print(len(radar_rain_tweets.index))
    print(radar_rain_tweets)
    print(5.75)
    acc_train_hist = []
    precision_train_hist = []
    recall_train_hist = []
    precision_train_neg_hist = []
    recall_train_neg_hist = []
    conf_matrix_train_hist = []
    
    acc_test_hist = []
    precision_test_hist = []
    recall_test_hist = []
    precision_test_neg_hist = []
    recall_test_neg_hist = []
    conf_matrix_test_hist = []
    for i in range(100):
        radar_rain_tweets_eq = equalize_data(radar_rain_tweets)
        X_train, X_test, y_train, y_test = split_on_radarXY(radar_rain_tweets_eq, test_size=0.33, random_state=i)
        model = RandomForestClassifier(n_estimators=10)
        print(6)
        model.fit(X_train,y_train)
        print(7)
        y_pred_train = model.predict(X_train)
        print(8)
        y_pred_test = model.predict(X_test)
        print(9)
        train_accuracy = model.score(X_train,y_train)
        print(10)
        test_accuracy = model.score(X_test,y_test)
        print("train accuracy:")
        print(train_accuracy)
        print("test accuracy:")
        print(test_accuracy)
        conf_matrix_train = confusion_matrix(y_train, y_pred_train)
        conf_matrix_test = confusion_matrix(y_test, y_pred_test)
        print("confusion_matrix train")
        print(conf_matrix_train)
        print("accuracy train")
        try:
            acc_train = (conf_matrix_train[0][0]+conf_matrix_train[1][1])/(conf_matrix_train[0][0]+conf_matrix_train[0][1]+conf_matrix_train[1][0]+conf_matrix_train[1][1])
        except:
            acc_train = None
        print(acc_train)
        try:
            precision_train = conf_matrix_train[1][1]/(conf_matrix_train[1][1]+conf_matrix_train[0][1]) 
        except:
            precision_train = None
        try:
            precision_train_neg = conf_matrix_train[0][0]/(conf_matrix_train[1][0]+conf_matrix_train[0][0]) 
        except:
            precision_train_neg = None
        try:
            recall_train = conf_matrix_train[1][1]/(conf_matrix_train[1][1]+conf_matrix_train[1][0]) 
        except:
            recall_train = None
        try:
            recall_train_neg = conf_matrix_train[0][0]/(conf_matrix_train[0][0]+conf_matrix_train[0][1]) 
        except:
            recall_train_neg = None
        print("precision_train")
        print(precision_train)
        print("recall_train")
        print(recall_train)
        
        print("confusion_matrix test")
        print(conf_matrix_test)
        print("accuracy test")
        try:
            acc_test = (conf_matrix_test[0][0]+conf_matrix_test[1][1])/(conf_matrix_test[0][0]+conf_matrix_test[0][1]+conf_matrix_test[1][0]+conf_matrix_test[1][1])
        except:
            acc_test = None
        print(acc_test)
        try:
              precision_test = conf_matrix_test[1][1]/(conf_matrix_test[1][1]+conf_matrix_test[0][1]) 
        except:
              precision_test = None
        try:
            precision_test_neg = conf_matrix_test[0][0]/(conf_matrix_test[1][0]+conf_matrix_test[0][0]) 
        except:
            precision_test_neg = None
        try:
              recall_test = conf_matrix_test[1][1]/(conf_matrix_test[1][1]+conf_matrix_test[1][0]) 
        except:
              recall_test = None
        try:
            recall_test_neg = conf_matrix_test[0][0]/(conf_matrix_test[0][0]+conf_matrix_test[0][1]) 
        except:
            recall_test_neg = None
        print("precision_test")
        print(precision_test)
        print("recall_test")
        print(recall_test)
        print("\n----------------------------------")
        print("----------------------------------\n")
        acc_train_hist.append(acc_train)
        precision_train_hist.append(precision_train)
        recall_train_hist.append(recall_train)
        precision_train_neg_hist.append(precision_train_neg)
        recall_train_neg_hist.append(recall_train_neg)
        conf_matrix_train_hist.append(conf_matrix_train.tolist())

        acc_test_hist.append(acc_test)
        precision_test_hist.append(precision_test)
        recall_test_hist.append(recall_test)
        precision_test_neg_hist.append(precision_test_neg)
        recall_test_neg_hist.append(recall_test_neg)
        conf_matrix_test_hist.append(conf_matrix_test.tolist())
    
    print("acc_train_hist avg, std:")
    print(np.average(acc_train_hist),np.std(acc_train_hist))
    print("precision_train_hist avg, std:")
    print(np.average(precision_train_hist),np.std(precision_train_hist))
    print("recall_train_hist avg, std:")
    print(np.average(recall_train_hist),np.std(recall_train_hist))
    print()
    print("acc_test_hist avg, std:")
    print(np.average(acc_test_hist),np.std(acc_test_hist))
    print("precision_test_hist avg, std:")
    print(np.average(precision_test_hist),np.std(precision_test_hist))
    print("recall_test_hist avg, std:")
    print(np.average(recall_test_hist),np.std(recall_test_hist))
    
    hist = {}
    hist['acc_train_hist'] = acc_train_hist
    hist['precision_train_hist'] = precision_train_hist
    hist['recall_train_hist'] = recall_train_hist
    hist['precision_train_neg_hist'] = precision_train_neg_hist
    hist['recall_train_neg_hist'] = recall_train_neg_hist
    hist['conf_matrix_train_hist'] = conf_matrix_train_hist
    
    hist['acc_test_hist'] = acc_test_hist
    hist['precision_test_hist'] = precision_test_hist
    hist['recall_test_hist'] = recall_test_hist
    hist['precision_test_neg_hist'] = precision_test_neg_hist
    hist['recall_test_neg_hist'] = recall_test_neg_hist
    hist['conf_matrix_test_hist'] = conf_matrix_test_hist

    with open('rain_only_hist2.json', 'w') as fp:
        json.dump(hist, fp)

def analyse_pandafied():
    analyse_twitter()

if __name__ == '__main__':
    analyse_pandafied()

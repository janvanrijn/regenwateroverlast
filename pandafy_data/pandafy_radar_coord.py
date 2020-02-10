import numpy as np
import sys
import os
import csv
import pandas as pd
import json
import ast
import pandafy_tiffs
from osgeo import osr, gdal
import shapely
from shapely.geometry import Polygon,Point
from tqdm import tqdm
from joblib import Parallel, delayed
import multiprocessing
import copy
sys.path.append(os.path.realpath('../experiments/'))
from class_experiment import normalize_data,do_sobel,preprocessing

from analyse_pandafied import have_tif_give_features, load_encoder

def load_km_from_tif(center,files,parameters):
    '''
        This function loads a sample image out of multiple .tiff files
        
        ---arguments---
        center: tuple containing longitude latitude coordinates of the center of the to be sampled rectangle
        files: a list containing the files that need to be opened for sampling. It is assumed at this point that these files contain all the pixels that need to be sampled. Including files that fall outside the to be sampled area is allowed.
        parameters: dictionary containing parameters on how to sample
            parameters['data_folder']: string containing the path to folder from which to sample
            parameters['x']: number of pixels that need to be sampled in the x direction (longitude)
            parameters['y']: number of pixels that need to be sampled in the y direction (latitude). Note that the direction of y indexes and latitude coordinates is flipped, i.e. higher values in the y direction mean lower values in the latitude direction.
            parameters['uc']: upper clamp value to clamp data. each value greater than parameters['uc'] is set to parameters['uc']
            parameters['lc']: l clamp value to clamp data. each value smaller than parameters['lc'] is set to parameters['lc']
            parameters['scale_cm']: amount of centimeters per pixel. This should be 500 for AHN2_5m, since each pixel is 5m x 5m, or 500cm x 500cm
        
        ---returns---
        image in list of lists representation. indexed with [y][x]
    '''
    folder=parameters['data_folder']
    x=parameters['x']
    y=parameters['y']
    max_val=parameters['uc']
    min_val=parameters['lc']
    scale_cm=parameters['scale_cm']
    center = ast.literal_eval(str(center))
    image = np.full((y, x), min_val,dtype=float)
    for file in files:
        ds = gdal.Open(folder+file, gdal.GA_ReadOnly)
        rb = ds.GetRasterBand(1)
        imarray = rb.ReadAsArray()
        minx_tif,miny_tif,maxx_tif,maxy_tif = pandafy_tiffs.give_corners(ds)
        minx_tif=int(minx_tif)
        miny_tif=int(miny_tif)
        maxx_tif=int(maxx_tif)
        maxy_tif=int(maxy_tif)
        dim_tif = (maxy_tif-miny_tif)*100//scale_cm,(maxx_tif-minx_tif)*100//scale_cm
        transform = pandafy_tiffs.give_transform(ds,old_to_new=False)
        center_transformed = transform.TransformPoint(center[0],center[1])
        center_transformed = int(center_transformed[1]),int(center_transformed[0])
        center_pixel = (center_transformed[0]-miny_tif)*100//scale_cm,(center_transformed[1]-minx_tif)*100//scale_cm
        center_pixel = (dim_tif[0]-center_pixel[0],center_pixel[1])
        
        minx_samp = center_pixel[1] - int(np.floor(x/2))
        maxx_samp = center_pixel[1] + int(np.ceil(x/2))
        miny_samp = center_pixel[0] - int(np.floor(y/2))
        maxy_samp = center_pixel[0] + int(np.ceil(y/2))
        sampy = max(miny_samp,0)
        imgy = np.abs(min(miny_samp,0))
        while sampy < dim_tif[0] and sampy < maxy_samp:
            sampx = max(minx_samp,0)
            imgx = np.abs(min(minx_samp,0))
            while sampx < dim_tif[1] and sampx < maxx_samp:
                image[imgy][imgx] = imarray[sampy][sampx]
                sampx += 1
                imgx += 1
            sampy += 1
            imgy += 1
        del ds
    return image.tolist()

def make_parallel_file_list(radar,file,marge):
    #TODO keyerror somewhere in this function
    radar = copy.deepcopy(radar)
    latlon_point = ast.literal_eval(str(radar['latlon_sw'].tolist()[0]))
    point0 = Point(latlon_point[1], latlon_point[0])
    latlon_point = ast.literal_eval(str(radar['latlon_se'].tolist()[0]))
    point1 = Point(latlon_point[1], latlon_point[0])
    latlon_point = ast.literal_eval(str(radar['latlon_ne'].tolist()[0]))
    point2 = Point(latlon_point[1], latlon_point[0])
    latlon_point = ast.literal_eval(str(radar['latlon_nw'].tolist()[0]))
    point3 = Point(latlon_point[1], latlon_point[0])
    local_file_list = []
    for j in range(len(file['file_name'])):
        sw = ast.literal_eval(str(file['latlon_sw'][j]))
        se = ast.literal_eval(str(file['latlon_se'][j]))
        ne = ast.literal_eval(str(file['latlon_ne'][j]))
        nw = ast.literal_eval(str(file['latlon_nw'][j]))
        polygon = Polygon(((sw[1]-marge, sw[0]-marge), (se[1]+marge, se[0]-marge), (ne[1]+marge, ne[0]+marge), (nw[1]-marge, nw[0]+marge), (sw[1]-marge, sw[0]-marge)))
        if polygon.contains(point0) or polygon.contains(point1) or polygon.contains(point2) or polygon.contains(point3):
            local_file_list.append(file['file_name'][j])
    radar['file_list'] = [local_file_list]
    return radar

def have_radar_give_tif(radar,file=None,parameters=None,marge=0.01,only_features=False,saved_file_list='saved_file_list.json',encoder=None,do_a_save=False,Twitter_center=False,the_min=None,the_max=None,pre_process_img = False):
    """
        This functions returns an image out of a map specifeid by parameters['data_folder']
        
        ---arguments---
        radar: pandas dataframe containing radar pixels and their corner point in longitude latitude representation. This dataframe may contain tweets and their geolocation
        file: pandas dataframe containing the corner points in longitude latitude representation of tif files in the folder specified by parameters['data_folder']
        parameters: dictionary containing parameters that are used for tif file location, sampling and preprocessing. If None, a default set of parameters is loaded.
        marge: float that specifies an arbitrary marge of error for determining if a longitude latitude point lies within a tif file. A larger value will result in more files being read for sampling information. The default value seems to never exclude a relevant tiff file.
        only_features: boolean value. If True: only the features generated by the autoencoder are saved. If False: The entire sampled image is saved, but not the features. Saving only features saves memory, but it is not possible to recalculate features after the construction of the dataframe has taken place without reconstructing the entire dataframe.
        saved_file_list: string containing the name of a .json file that per radar pixel contains a list of files that information needs to be loaded from. In case saved_file_list is None, this list is calculated. This calculation takes a long time, therefore saving this list is added as an option to save time.
        encoder: a keras autoencoder that constructs features from terrain data. If None, an autoencoder is loaded from parameters['encoder_file_name']
        do_a_save: boolean value. If True, a .json file is saved that per radar pixel has a list of tiff files it needs to load information from. Since creating these file lists takes a lot of time, saving this information can greatly reduce runtime in case of reruns.
        Twitter_center: boolean value. If True, a sample is taken with the Tweets' geolocation as the center point. If False, a sample is taken with the center of the radar pixel as the center point.
        the_min: minimum value when normalising data. If this is None, the minimum from the given data is taken, but it can be necessary to take the minimum of the entire dataset, not just the given data. Therefore this option was added.
        the_max: maximum value when normalising data. If this is None, the maximum from the given data is taken, but it can be necessary to take the maximum of the entire dataset, not just the given data. Therefore this option was added.
        pre_process_img: If True, in case only_features is False, a preprocessing step is performed before saving the sampled image. In case only_features is True, preprocessing is always performed, since the autoencoder is trained on a preprocessed dataset.
        
        ---returns---
        pandas dataframe with an added column containing either images or features, depending on the given arguments
    """
    #TODO this function is not foolproof in some edge cases where the radar rectangle's corners lie in three files, but the tif rectangle's corners lie in four files. To counteract this, a marge value is set to include a wider range of files
    if parameters is None:
        parameters = {}
        parameters['data_folder']='../../AHN2_5m/'
        parameters['x']=200
        parameters['y']=200
        parameters['uc']=210
        parameters['lc']=-15
        parameters['clamp']=True
        parameters['normalize_data']=True
        parameters['filter'] = 'identity'
        parameters['encoder_type']='shallow_narrow_cnn'
        parameters['scale_cm']=500
        parameters['encoder_file_name'] = '../../results/pooling_narrow_experiment_AHN2_5m_SOBELCOMP/pool_1_encoder'
        encoder = load_encoder(parameters['encoder_file_name'])
    if file is None:
        file = pd.read_csv('../../pandafied_data/lat_lon_to_filename.csv')
    file_list = []
    if saved_file_list is None:
        print("saved_file_list is None")
        make_new_file_list=True
    else:
        try:
            with open(saved_file_list, 'r') as f:
                file_list = json.load(f)
            make_new_file_list=False
            print("file_list is loaded")
        except:
            make_new_file_list=True
            print("file_list is not loaded")
    images = []
    features = []
    #make_new_file_list = True #somehow the file list was not the right length, so make it anyway to be sure
    if make_new_file_list:
        print("now loading file_list")
        num_cores = multiprocessing.cpu_count()
        print("num_cores: " + str(num_cores))
        #results = [make_parallel_file_list(radar[i:i+1],file,marge) for i in radar.index]
        #results = Parallel(n_jobs=num_cores)(delayed(make_parallel_file_list)(radar[i:i+1],file,marge) for i in range(6))
        results = Parallel(n_jobs=num_cores)(delayed(make_parallel_file_list)(radar[i:i+1],file,marge) for i in radar.index)# if i < 2)
        radar = {}
        for i in results[0].keys():
            print(i)
            radar[i] = []
        radar = pd.DataFrame(radar)
        for i in results:
            radar = radar.append(i)
        print(radar.keys())
        file_list=radar['file_list'].values.tolist()
        radar = radar.drop(['file_list'],1)
        if do_a_save:
            with open(saved_file_list, 'w') as f:
                json.dump(file_list, f)
    print("now loading images or features")
    for i in tqdm(range(len(file_list))):
        if not only_features:
            if Twitter_center:
                img = load_km_from_tif(radar['latlon'][i],file_list[i],parameters)
            else:
                img = load_km_from_tif(radar['latlon_center'][i],file_list[i],parameters)
            if pre_process_img:
                img,_,_,_,_= preprocessing(np.array([img]),np.array([img]),np.array([img]),parameters,the_min=the_min,the_max=the_max)
            images.append(img[0])
        else:
            if Twitter_center:
                local_image = load_km_from_tif(radar['latlon'][i],file_list[i],parameters)
            else:
                local_image = load_km_from_tif(radar['latlon_center'][i],file_list[i],parameters)
            local_df = {}
            local_df['images'] = [local_image]
            local_df = pd.DataFrame(local_df)
            local_df = have_tif_give_features(local_df,parameters,encoder=encoder,the_min=the_min,the_max=the_max)
            features.append(local_df['features'][0])
    if not only_features:
        radar['images'] = images
    else:
        print("len(features): ",len(features))
        print("len(dataframe): ",len(radar['latlon_sw']))
        radar['features'] = np.array(features).tolist()
    return radar

def pandafy_radar_coord(file_name='../data/radar.json',save_name='../../pandafied_data/pandafied_radar_coord.csv'):
    with open(file_name) as json_file:
        data = json.load(json_file)
    
    data_dict = {}
    data_dict['radarY'] = [int(i['X']) for i in data]#to keep consistency of x is horizontal and y is vertical, x and y are flipped in this step
    data_dict['radarX'] = [int(i['Y']) for i in data]
    data_dict['latlon_center'] = [(float(i['Breedtegraad']),float(i['Lengtegraad'])) for i in data]
    data_dict['latlon_sw'] = [(float(i['SW'][1]),float(i['SW'][0])) for i in data]
    data_dict['latlon_se'] = [(float(i['SE'][1]),float(i['SE'][0])) for i in data]
    data_dict['latlon_ne'] = [(float(i['NE'][1]),float(i['NE'][0])) for i in data]
    data_dict['latlon_nw'] = [(float(i['NW'][1]),float(i['NW'][0])) for i in data]
    data_dict['bebouwd'] = [i['Bebouwd'] for i in data]
    
    pandas_data = pd.DataFrame(data_dict)
    
    parameters = {}
    parameters['data_folder']='../../AHN2_5m/'
    parameters['x']=200
    parameters['y']=200
    parameters['uc']=210
    parameters['lc']=-15
    parameters['normalize_data']=True
    parameters['filter'] = 'sobel'
    parameters['encoder_type']='shallow_narrow_cnn'
    parameters['scale_cm']=500
    parameters['encoder_file_name'] = '../../results/pooling_narrow_experiment_AHN2_5m_test_SOBEL2/pool_5_encoder'
    
    file = pd.read_csv('../../pandafied_data/lat_lon_to_filename.csv')
    
    pandas_data = have_radar_give_tif(pandas_data,file,parameters)
    
    pandas_data.to_csv(save_name,index=False)


if __name__ == '__main__':
    pandafy_radar_coord()

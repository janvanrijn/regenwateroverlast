import pandas as pd
from pandafy_radar_coord import load_km_from_tif,have_radar_give_tif
from analyse_pandafied import load_encoder
import sys
import json

def add_image_to_h5_radar(save_name='../../pandafied_data/pandafied_h5_radar_img.csv',data_folder='../../AHN2_5m/',encoder_file_name='../../results/pooling_narrow_experiment_AHN2_5m_SOBELCOMP/pool_1_encoder',history_file='../../results/pooling_narrow_experiment_AHN2_5m_SOBELCOMP/pool_1_history.json',pandas_data_file_name='../../pandafied_data/pandafied_h5_radar.csv',file='../../pandafied_data/lat_lon_to_filename.csv',saved_file_list='../../pandafied_data/saved_file_list.json'):
    '''
        This function can add image data from AHN2_5m for each radar pixel. For now however, it only adds the features calculated by the specified autoencoder to save on RAM and disk space.
    '''
    parameters = {}
    parameters['data_folder']=data_folder
    #parameters['data_folder']=
    parameters['x']=200
    parameters['y']=200
    parameters['uc']=210
    parameters['lc']=-15
    parameters['clamp']=True
    parameters['normalize_data']=True
    parameters['filter'] = 'identity'#'sobel_components'
    parameters['encoder_type']='shallow_narrow_cnn'
    parameters['scale_cm']=500
    #parameters['encoder_file_name'] = '../../results/pooling_narrow_experiment_AHN2_5m_SOBEL2/pool_5_encoder'
    parameters['encoder_file_name'] = encoder_file_name
    encoder = load_encoder(parameters['encoder_file_name'])
    
    pandas_data = pd.read_csv(pandas_data_file_name)
    #pandas_data = pandas_data.head(5)
    file = pd.read_csv(file)
    
    encoder=load_encoder(parameters['encoder_file_name'])
    
    history_dict = {}
    with open(history_file) as f:
        for line in f:
            history = json.loads(line)
        
    the_min = history['the_min']
    the_max = history['the_max']
    pandas_data = have_radar_give_tif(pandas_data,file,parameters,only_features=True,saved_file_list=saved_file_list,encoder=encoder,do_a_save=True,the_min=the_min,the_max=the_max)
    pandas_data.to_csv(save_name,index=False)

if __name__ == '__main__':
    if len(sys.argv) >= 2 and sys.argv[1] == 'grace':
        add_image_to_h5_radar(data_folder='/scratch/lamers/AHN2_5m/',encoder_file_name='/scratch/lamers/results/pooling_narrow_experiment_AHN2_5m_SOBELCOMP/pool_1_encoder',history_file='/scratch/lamers/results/pooling_narrow_experiment_AHN2_5m_SOBELCOMP/pool_1_history.json')
    else:
        #add_image_to_h5_radar()
        add_image_to_h5_radar(save_name='../../pandafied_data/pandafied_h5_radar_img_raw_NO_SOBEL.csv',data_folder='../../AHN2_5m/',encoder_file_name='../../results/raw_experiment_AHN2_5m/raw_4_encoder',history_file='../../results/raw_experiment_AHN2_5m/raw_4_history.json')

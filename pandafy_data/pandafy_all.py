import pandafy_h5_make_radarXY
import pandafy_tiffs
import add_image_to_h5_radar
import pandafy_twitter
import pandafy_twitter_add_XY
import pandafy_h5_2007_2020

def pandafy_all(super_folder='../../',encoder_file_name='results/pooling_narrow_experiment_AHN2_5m_SOBELCOMP/pool_1_encoder',history_file='results/pooling_narrow_experiment_AHN2_5m_SOBELCOMP/pool_1_history.json',parameters=None,radar_feat_save_name='pandafied_data/pandafied_h5_radar_img.csv'):
    '''
        This function performs all steps to build all panda dataframes in the right order from the raw data sources. These raw data sources must be put in the right folder in order for the methods to find them. These folders can be found by looking at the source code of the methods called below. The panda dataframes are saved in the folder ../../pandafied_data
        !!!WARNING!!!
        Do not store the twitter credentials or the returned Tweets in this GitHub repository, since it can be accessed by anyone.
        ---arguments---
        No arguments
        
        ---returns---
        Does not return anything
    '''
    print("pandafy_h5_make_radarXY()")
    pandafy_h5_make_radarXY.pandafy_h5_make_radarXY(folder=super_folder+'/KNMI_24h/KNMI-data_2019-12-09_14-07-28/rad_nl25_rac_mfbs_24h/2.0/0002/2016/12/31/RAD_NL25_RAC_MFBS_24H/2017/', save_name_radar=super_folder+'/pandafied_data/pandafied_h5_radar.csv')
    print("pandafy_tiffs()")
    pandafy_tiffs.pandafy_tiffs(data_folder=super_folder+'AHN2_5m/',save_name=super_folder+'pandafied_data/lat_lon_to_filename.csv')
    print("add_image_to_h5_radar()")
    add_image_to_h5_radar.add_image_to_h5_radar(save_name=super_folder+radar_feat_save_name,data_folder=super_folder+'AHN2_5m/',encoder_file_name=super_folder+encoder_file_name,history_file=super_folder+history_file,pandas_data_file_name=super_folder+'pandafied_data/pandafied_h5_radar.csv',file=super_folder+'pandafied_data/lat_lon_to_filename.csv',saved_file_list=super_folder+'pandafied_data/saved_file_list.json',parameters=parameters)
    print("pandafy_twitter()")
    pandafy_twitter.pandafy_twitter(file_name=super_folder+'Twitter_cred/full_arch_2007-2020.txt',save_name=super_folder+'pandafied_data/pandafied_twitter_2007-2020.csv')
    print("pandafy_twitter_add_XY()")
    pandafy_twitter_add_XY.pandafy_twitter_2007_2020_add_XY(tweets_file_name=super_folder+'pandafied_data/pandafied_twitter_2007-2020.csv',radar_file_name=super_folder+'pandafied_data/pandafied_h5_radar.csv',save_name=super_folder+'pandafied_data/pandafied_twitter_2007-2020_XY.csv')
    print("pandafy_h5()")
    pandafy_h5_2007_2020.pandafy_h5(save_name_radar=super_folder+'pandafied_data/pandafied_h5_radar.csv',save_name_rain=super_folder+'pandafied_data/pandafied_h5_rain_2007-2020.csv',folder = super_folder+'KNMI_big/KNMI-data_2020-01-29_15-56-00/rad_nl25_rac_mfbs_01h/2.0/0002/')

if __name__ == '__main__':
    pandafy_all()

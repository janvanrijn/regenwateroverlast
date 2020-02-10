!!!WARNING!!!
Do not store the twitter credentials or the returned Tweets in this GitHub repository, since it can be accessed by anyone.


to build the dataset: 
run pandafy_all.py
or 
run each script referenced in pandafy_all.py in the order this file states

dependencies when building data:
pandafy_h5.pandafy_h5() needs pandafy_h5_make_radarXY.pandafy_h5_make_radarXY() to have run 
pandafy_twitter_add_XY.pandafy_curated_twitter_add_XY() needs pandafy_h5_make_radarXY.pandafy_h5_make_radarXY() and curate_twitter.curate_twitter() to have run 
pandafy_twitter_add_XY.pandafy_twitter_add_XY() needs pandafy_h5_make_radarXY.pandafy_h5_make_radarXY() and pandafy_twitter.pandafy_twitter() to have run 
add_image_to_h5_radar.add_image_to_h5_radar() needs pandafy_tiffs.pandafy_tiffs() and pandafy_h5_make_radarXY.pandafy_h5_make_radarXY() to have run

to analyse the data:
first build the dataset, then run analyse_pandafied.py

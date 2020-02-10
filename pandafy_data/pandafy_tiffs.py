#to initiate gdal environment:
#source activate gdal_test
from osgeo import osr, gdal
import sys
from os import listdir
from os.path import isfile, join
from plotly import graph_objs as go
#from plotly import express as px
#import plotly.express as px
import numpy as np
import pandas as pd

def give_corners(ds):
    #solution found at:
    #https://stackoverflow.com/questions/2922532/obtain-latitude-and-longitude-from-a-geotiff-file
    width = ds.RasterXSize
    height = ds.RasterYSize
    gt = ds.GetGeoTransform()
    minx = gt[0]
    miny = gt[3] + width*gt[4] + height*gt[5] 
    maxx = gt[0] + width*gt[1] + height*gt[2]
    maxy = gt[3] 
    return minx,miny,maxx,maxy

def give_transform(ds=None,old_to_new=True,old_wkt=None):
    #solution found at:
    #https://stackoverflow.com/questions/2922532/obtain-latitude-and-longitude-from-a-geotiff-file
    old_cs= osr.SpatialReference()
    #print(ds.GetProjectionRef())
    if not old_wkt is None:
        print("old_wkt is not None")
        old_cs .ImportFromWkt(old_wkt)
    elif not ds is None:
        old_cs.ImportFromWkt(ds.GetProjectionRef())
    else:
        print("missing projection information in pandafy_tiffs.give_transform()")
        exit()
    
    # create the new coordinate system
    wgs84_wkt = """
        GEOGCS["WGS 84",
        DATUM["WGS_1984",
        SPHEROID["WGS 84",6378137,298.257223563,
        AUTHORITY["EPSG","7030"]],
        AUTHORITY["EPSG","6326"]],
        PRIMEM["Greenwich",0,
        AUTHORITY["EPSG","8901"]],
        UNIT["degree",0.01745329251994328,
        AUTHORITY["EPSG","9122"]],
        AUTHORITY["EPSG","4326"]]"""
    new_cs = osr.SpatialReference()
    new_cs .ImportFromWkt(wgs84_wkt)
    
    # create a transform object to convert between coordinate systems
    if old_to_new:
        return osr.CoordinateTransformation(old_cs,new_cs) 
    else:
        return osr.CoordinateTransformation(new_cs,old_cs) 
    
def get_lat_lon(ds,old_wkt=None):
    #solution found at:
    #https://stackoverflow.com/questions/2922532/obtain-latitude-and-longitude-from-a-geotiff-file
    transform = give_transform(ds,old_wkt=old_wkt) 

    minx,miny,maxx,maxy = give_corners(ds)

    #get the coordinates in lat lon
    latlon_sw = transform.TransformPoint(minx,miny)
    latlon_se = transform.TransformPoint(maxx,miny)
    latlon_ne = transform.TransformPoint(maxx,maxy)
    latlon_nw = transform.TransformPoint(minx,maxy)
    return (latlon_sw[0],latlon_sw[1]), (latlon_se[0],latlon_se[1]),(latlon_ne[0],latlon_ne[1]),(latlon_nw[0],latlon_nw[1])

def make_box(latlon_sw, latlon_se,latlon_ne,latlon_nw):
    lon = [latlon_sw[1],latlon_se[1],latlon_ne[1],latlon_nw[1],latlon_sw[1]]
    lat = [latlon_sw[0],latlon_se[0],latlon_ne[0],latlon_nw[0],latlon_sw[0]]
    return lon, lat
    

def load_gdal_tiff(parameters,save_name,plot=False,old_wkt=None):
    folder = parameters['data_folder']
    x = parameters['x']
    y = parameters['y']
    onlyfiles = [f for f in listdir(folder) if isfile(join(folder, f))]
    tif_files = [f for f in onlyfiles if '.tif' in f]
    img_array_list = []
    lon_box = np.array([])
    lat_box = np.array([])
    file_name = []
    lon_point = []
    lat_point = []
    add_none = False
    latlon_sw_list = []
    latlon_se_list = []
    latlon_ne_list = []
    latlon_nw_list = []
    for i in tif_files:
        file_name.append(i)
        if add_none:
            lon_box = np.append(lon_box,None)
            lat_box = np.append(lat_box,None)
        else:
            add_none = True
        
        ds = gdal.Open(folder+i, gdal.GA_ReadOnly)
        latlon_sw, latlon_se,latlon_ne,latlon_nw = get_lat_lon(ds,old_wkt=old_wkt)
        latlon_sw_list.append(latlon_sw)
        latlon_se_list.append(latlon_se)
        latlon_ne_list.append(latlon_ne)
        latlon_nw_list.append(latlon_nw)
        
        lon_point.append((latlon_ne[1]+latlon_sw[1])/2)
        lat_point.append((latlon_ne[0]+latlon_sw[0])/2)
        lon, lat = make_box(latlon_sw, latlon_se,latlon_ne,latlon_nw)
        lon_box = np.append(lon_box,lon)
        lat_box = np.append(lat_box,lat)
        #rb = ds.GetRasterBand(1)
        #img_array = rb.ReadAsArray()
        #img_array_list.append(img_array)
        del ds
    lon_box = np.array(lon_box).flatten()
    lat_box = np.array(lat_box).flatten()
    dict_data = {"file_name":file_name,"latlon_sw":latlon_sw_list,"latlon_se":latlon_se_list,"latlon_ne":latlon_ne_list,"latlon_nw":latlon_nw_list}
    pandas_data = pd.DataFrame(dict_data)
    pandas_data.to_csv(save_name,index=False)
    if plot:
        plot_tiffs(lon_box,lat_box,lon_point,lat_point,file_name)
    return pandas_data

def plot_tiffs(lon_box,lat_box,lon_point,lat_point,file_name):
    data = {}
    data['lat'] = lat_point
    data['lon'] = lon_point
    data['file_name'] = file_name
    
    data = pd.DataFrame(data)
    
    fig = px.scatter_mapbox(data, lat="lat", lon="lon", hover_name="file_name", hover_data=["file_name"],
                            color_discrete_sequence=["fuchsia"], zoom=3, width=1300, height=600)
                            
    fig.add_trace(go.Scattermapbox(mode = "lines", fill = "toself",lon = lon_box,lat = lat_box))
                                   
    dutch_box=[3.026594,51.071778,7.155537,53.699308]
    fig.update_layout(
                     mapbox = {
                     'style': "stamen-terrain",
                     'center': {'lon': (dutch_box[2] + dutch_box[0])/2, 'lat': (dutch_box[3] + dutch_box[1])/2 },
                     'zoom': 5.6},
                     showlegend = False)
                                   
    fig.write_html('../../Twitter_cred/tweets.html', auto_open=True)

def pandafy_tiffs(data_folder='../../AHN2_5m/',save_name='../../pandafied_data/lat_lon_to_filename.csv',old_wkt=None):
    '''
        This function maps .tif files in data_folder to their longitude latitude corner points
    '''
    parameters = {}
    parameters['type'] = 'simple'
    parameters['encoding_dim'] = 20
    parameters['epochs'] = 10
    parameters['batch_size'] = 256
    parameters['x'] = 200
    parameters['y'] = 200
    parameters['data_sampling'] = 'random'
    parameters['normalize_data'] = True
    parameters['data_folder']=data_folder
    parameters['train_part'] = 500
    parameters['test_part'] = 2
    parameters['val_part'] = 2
    parameters['n_data'] = parameters['train_part']+parameters['test_part']+parameters['val_part']
    parameters['print_results'] = True
    parameters['seed'] = 42

    load_gdal_tiff(parameters,save_name,old_wkt=old_wkt)

if __name__ == '__main__':
    if len(sys.argv) >= 2 and sys.argv[1] == 'grace':
        pandafy_tiffs(data_folder='/scratch/lamers/AHN2_5m/')
    else:
        pandafy_tiffs()

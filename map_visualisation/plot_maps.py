from plotly import graph_objs as go
try:
    import plotly.express as px
except:
    pass
import pandas as pd
import numpy as np
import ast
import json

def asRadians(degrees):
    return degrees * np.pi / 180

def getXYpos(Null_long,Null_lat, p_long,p_lat):
    """ Calculates X and Y distances in meters.
        """
    deltaLatitude = p_lat - Null_lat
    deltaLongitude = p_long - Null_long
    latitudeCircumference = 40075160 * np.cos(asRadians(Null_lat))
    resultX = deltaLongitude * latitudeCircumference / 360
    resultY = deltaLatitude * 40008000 / 360
    return resultX, resultY

def plot_pandafied(data_list):
    lon_box = []
    lat_box = []
    lon_point = []
    lat_point = []
    text_sh_point = []
    text_point = []
    color = []
    for data in data_list:
        for key in data.keys():
            print(key)
            data_len = len(data[key])
        for i in range(data_len):
            #try:
            #    lon_box.append(ast.literal_eval(data['latlon_sw'][i])[1])
            #    lon_box.append(ast.literal_eval(data['latlon_se'][i])[1])
            #    lon_box.append(ast.literal_eval(data['latlon_ne'][i])[1])
            #    lon_box.append(ast.literal_eval(data['latlon_nw'][i])[1])
            #    lon_box.append(ast.literal_eval(data['latlon_sw'][i])[1])
            #    lon_box.append(None)
            #    
            #    lat_box.append(ast.literal_eval(data['latlon_sw'][i])[0])
            #    lat_box.append(ast.literal_eval(data['latlon_se'][i])[0])
            #    lat_box.append(ast.literal_eval(data['latlon_ne'][i])[0])
            #    lat_box.append(ast.literal_eval(data['latlon_nw'][i])[0])
            #    lat_box.append(ast.literal_eval(data['latlon_sw'][i])[0])
            #    lat_box.append(None)
            #except:
            #    pass
            
            try:
                #lat_radar.append(ast.literal_eval(data['latlon_center'][i])[0])
                #lon_radar.append(ast.literal_eval(data['latlon_center'][i])[1])
                #text_sh_radar.append("("+str(data['radarY'][i])+","+str(data['radarX'][i])+")")
                #text_radar.append("("+str(data['radarY'][i])+","+str(data['radarX'][i])+")")
                #color.append("orange")
                lat_point.append(ast.literal_eval(data['latlon'][i])[0])
                lon_point.append(ast.literal_eval(data['latlon'][i])[1])
                text_sh_point.append(data['text'][i][0:5])
                text_point.append(data['text'][i])
                color.append("fuchsia")
            except:
                pass
        
    points = {}
    points['lat'] = lat_point
    points['lon'] = lon_point
    points['text_sh'] = text_sh_point
    points['text'] = text_point
    points = pd.DataFrame(points)
    
    fig = px.scatter_mapbox(points, lat="lat", lon="lon", hover_name="text_sh", hover_data=["text"],
                    color_discrete_sequence=color, zoom=3, width=1300, height=600)
    
    #fig.add_trace(px.scatter_mapbox(twitter_points, lat="lat", lon="lon", hover_name="text_sh", hover_data=["text"],
    #                                color_discrete_sequence=["fuchsia"], zoom=3, width=1300, height=600))
    fig.add_trace(go.Scattermapbox(
                                 fill = "toself",
                                 lon = lon_box, lat = lat_box,
                                 marker = { 'size': 1, 'color': "orange" }))
    
    dutch_box=[3.026594,51.071778,7.155537,53.699308]
    fig.update_layout(
                      mapbox = {
                      'style': "stamen-terrain",
                      'center': {'lon': (dutch_box[2] + dutch_box[0])/2, 'lat': (dutch_box[3] + dutch_box[1])/2 },
                      'zoom': 5.6},
                      showlegend = False)
    fig.write_html('../../pandafied_data/pandafied_map.html', auto_open=True)
    

def box_plot_tweets(tweets,max_dim=1000):
    lon = []
    lat = []
    opacity = []
    for i in tweets:
        if not i['place'] is None and not i['place']['bounding_box'] is None:
            twitter_box = i['place']['bounding_box']['coordinates']
            resultX, resultY = getXYpos(twitter_box[0][0][0],twitter_box[0][0][1], twitter_box[0][2][0],twitter_box[0][2][1])
            if resultX <= max_dim and resultY <= max_dim:
                lon.append(twitter_box[0][1][0])
                lon.append(twitter_box[0][0][0])
                lon.append(twitter_box[0][3][0])
                lon.append(twitter_box[0][2][0])
                lon.append(twitter_box[0][1][0])
                lon.append(None)
                
                lat.append(twitter_box[0][1][1])
                lat.append(twitter_box[0][0][1])
                lat.append(twitter_box[0][3][1])
                lat.append(twitter_box[0][2][1])
                lat.append(twitter_box[0][1][1])
                lat.append(None)
                opacity.append(min(1.0,1.000000/(resultY*resultX)))
                print(i['text'])
                print()
    
    fig = go.Figure(go.Scattermapbox(
                                     mode = "lines", fill = "toself",
                                     lon = lon,
                                     lat = lat))

    dutch_box=[3.026594,51.071778,7.155537,53.699308]
    
    fig.update_layout(
                      mapbox = {
                      'style': "stamen-terrain",
                      'center': {'lon': (dutch_box[2] + dutch_box[0])/2, 'lat': (dutch_box[3] + dutch_box[1])/2 },
                      'zoom': 5.6},
                      showlegend = False)
     
    fig.write_html('../../Twitter_cred/tweets.html', auto_open=True)

def point_and_box_plot_tweets(tweets,max_dim=1000):
    lat_point = []
    lon_point = []
    text_sh_point = []
    text_point = []
    
    lon_box = []
    lat_box = []
    opacity_box = []
    
    for i in tweets:
        if not i['place'] is None and not i['place']['bounding_box'] is None:
            twitter_box = i['place']['bounding_box']['coordinates']
            resultX, resultY = getXYpos(twitter_box[0][0][0],twitter_box[0][0][1], twitter_box[0][2][0],twitter_box[0][2][1])
            if not i['coordinates'] is None:
                #if not i['coordinates'] is None:
                for j in i:
                    lat_point.append(i['coordinates']['coordinates'][1])
                    lon_point.append(i['coordinates']['coordinates'][0])
                    text_sh_point.append(i['text'][0:20])
                    text_point.append(i['text'])
            elif resultX <= 0.1 and resultY <= 0.1:
                lat_point.append(twitter_box[0][0][1])
                lon_point.append(twitter_box[0][0][0])
                text_sh_point.append(i['text'][0:20])
                text_point.append(i['text'])
            elif resultX <= max_dim and resultY <= max_dim:
                lon_box.append(twitter_box[0][1][0])
                lon_box.append(twitter_box[0][0][0])
                lon_box.append(twitter_box[0][3][0])
                lon_box.append(twitter_box[0][2][0])
                lon_box.append(twitter_box[0][1][0])
                lon_box.append(None)
                
                lat_box.append(twitter_box[0][1][1])
                lat_box.append(twitter_box[0][0][1])
                lat_box.append(twitter_box[0][3][1])
                lat_box.append(twitter_box[0][2][1])
                lat_box.append(twitter_box[0][1][1])
                lat_box.append(None)
                opacity_box.append(min(1.0,1.000000/(resultY*resultX)))
    
    data = {}
    data['lat'] = lat_point
    data['lon'] = lon_point
    data['text_sh'] = text_sh_point
    data['text'] = text_point

    data = pd.DataFrame(data)
    
    fig = px.scatter_mapbox(data, lat="lat", lon="lon", hover_name="text_sh", hover_data=["text"],
                            color_discrete_sequence=["fuchsia"], zoom=3, width=1300, height=600)
    
    fig.add_trace(go.Scattermapbox(
                                     mode = "lines", fill = "toself",
                                     lon = lon_box,
                                     lat = lat_box))
    
    dutch_box=[3.026594,51.071778,7.155537,53.699308]
    fig.update_layout(
                      mapbox = {
                      'style': "stamen-terrain",
                      'center': {'lon': (dutch_box[2] + dutch_box[0])/2, 'lat': (dutch_box[3] + dutch_box[1])/2 },
                      'zoom': 5.6},
                      showlegend = False)
    
    fig.write_html('../../Twitter_cred/tweets.html', auto_open=True)

def heatmap_tweets(tweets):
    lat = []
    lon = []
    
    for i in tweets:
        if not i['coordinates'] is None:
            for j in i:
                lat.append(i['coordinates']['coordinates'][1])
                lon.append(i['coordinates']['coordinates'][0])
    data = {}
    data['lat'] = lat
    data['lon'] = lon
    mag = [1]*len(lat)
    
    data = pd.DataFrame(data)

    fig = go.Figure(go.Densitymapbox(lat=lat, lon=lon, z=mag, radius=3))

    #fig = px.scatter_mapbox(data, lat="lat", lon="lon", hover_name="text_sh", hover_data=["text"],
    #                            color_discrete_sequence=["fuchsia"], zoom=3, width=1300, height=600)
    
    dutch_box=[3.026594,51.071778,7.155537,53.699308]
    fig.update_layout(
                      mapbox = {
                      'style': "stamen-terrain",
                      'center': {'lon': (dutch_box[2] + dutch_box[0])/2, 'lat': (dutch_box[3] + dutch_box[1])/2 },
                      'zoom': 5.6},
                      showlegend = False)
                      
    fig.write_html('../../Twitter_cred/tweets.html', auto_open=True)

def plot_all(tweets,data,max_dim=1000):
    from pyproj import CRS, Transformer
    lat_point = []
    lon_point = []
    text_sh_point = []
    text_point = []
    
    lon_box = []
    lat_box = []
    opacity_box = []
    
    for i in tweets:
        if not i['place'] is None and not i['place']['bounding_box'] is None:
            twitter_box = i['place']['bounding_box']['coordinates']
            resultX, resultY = getXYpos(twitter_box[0][0][0],twitter_box[0][0][1], twitter_box[0][2][0],twitter_box[0][2][1])
            if not i['coordinates'] is None:
                #if not i['coordinates'] is None:
                for j in i:
                    lat_point.append(i['coordinates']['coordinates'][1])
                    lon_point.append(i['coordinates']['coordinates'][0])
                    text_sh_point.append(i['text'][0:20])
                    text_point.append(i['text'])
            elif resultX <= 0.1 and resultY <= 0.1:
                lat_point.append(twitter_box[0][0][1])
                lon_point.append(twitter_box[0][0][0])
                text_sh_point.append(i['text'][0:20])
                text_point.append(i['text'])
            elif resultX <= max_dim and resultY <= max_dim:
                lon_box.append(twitter_box[0][1][0])
                lon_box.append(twitter_box[0][0][0])
                lon_box.append(twitter_box[0][3][0])
                lon_box.append(twitter_box[0][2][0])
                lon_box.append(twitter_box[0][1][0])
                lon_box.append(None)
                
                lat_box.append(twitter_box[0][1][1])
                lat_box.append(twitter_box[0][0][1])
                lat_box.append(twitter_box[0][3][1])
                lat_box.append(twitter_box[0][2][1])
                lat_box.append(twitter_box[0][1][1])
                lat_box.append(None)
                opacity_box.append(min(1.0,1.000000/(resultY*resultX)))
    
    data_tweets = {}
    data_tweets['lat'] = lat_point
    data_tweets['lon'] = lon_point
    data_tweets['text_sh'] = text_sh_point
    data_tweets['text'] = text_point
    
    data = pd.DataFrame(data)

    fig = px.scatter_mapbox(data_tweets, lat="lat", lon="lon", hover_name="text_sh", hover_data=["text"],
                        color_discrete_sequence=["fuchsia"], zoom=3, width=1300, height=600)
                                   
    #testing corner points from KNMI precipation map
    y_offset = 3650.0
    x_offset = 0.0
    to_proj = CRS.from_proj4("+proj=latlong +datum=WGS84 +R=+12756274")
    from_proj = CRS.from_proj4("+proj=stere +lat_0=90 +lon_0=0.0 +lat_ts=60.0 +a=6378.137 +b=6356.752 +x_0=0 +y_0=0 +type=crs")
    transform = Transformer.from_crs(from_proj, to_proj)
    lon = [0.0, 0.0, 10.856, 9.009,0.0]
    lat = [49.362, 55.974, 55.389, 48.895,49.362]
    
    if True:
        for i in range(765):
            for j in range(700):
                if i >= 380 and i < 420 and j >= 320 and j < 360:
                    lon.append(None)
                    lat.append(None)
                    local_sw = transform.transform(j-x_offset,-i-1-y_offset)
                    lat.append(local_sw[1])
                    lon.append(local_sw[0])
                    local_se = transform.transform(j+1-x_offset,-i-1-y_offset)
                    lat.append(local_se[1])
                    lon.append(local_se[0])
                    local_ne = transform.transform(j+1-x_offset,-i-y_offset)
                    lat.append(local_ne[1])
                    lon.append(local_ne[0])
                    local_nw = transform.transform(j-x_offset,-i-y_offset)
                    lat.append(local_nw[1])
                    lon.append(local_nw[0])
                    local_sw = transform.transform(j-x_offset,-i-1-y_offset)
                    lat.append(local_sw[1])
                    lon.append(local_sw[0])
    
    lon.append(None)
    lat.append(None)
    data_len = len(data.index)
    for i in range(data_len):
        lon.append(ast.literal_eval(data['latlon_sw'][i])[1])
        lon.append(ast.literal_eval(data['latlon_se'][i])[1])
        lon.append(ast.literal_eval(data['latlon_ne'][i])[1])
        lon.append(ast.literal_eval(data['latlon_nw'][i])[1])
        lon.append(ast.literal_eval(data['latlon_sw'][i])[1])
        lon.append(None)
        
        lat.append(ast.literal_eval(data['latlon_sw'][i])[0])
        lat.append(ast.literal_eval(data['latlon_se'][i])[0])
        lat.append(ast.literal_eval(data['latlon_ne'][i])[0])
        lat.append(ast.literal_eval(data['latlon_nw'][i])[0])
        lat.append(ast.literal_eval(data['latlon_sw'][i])[0])
        lat.append(None)
            
    
    #fig = go.Figure(go.Scattermapbox(
    #                                 fill = "toself",
    #                                 lon = lon, lat = lat, 
    #                                 marker = { 'size': 10, 'color': "orange" }))
    if False:
        fig.add_trace(go.Scattermapbox(
                                       mode = "lines", fill = "none",
                                       lon = lon,
                                       lat = lat,marker = { 'size': 10, 'color': "orange" }))
    #(0.0, 49.362054793776345)
    #(9.009275651861467, 48.89529831290508)
    #(10.856413348062246, 55.38893655359361)
    #(0.0, 55.97356207115716)
                                   
    dutch_box=[3.026594,51.071778,7.155537,53.699308]
    fig.update_layout(
                     mapbox = {
                     'style': "stamen-terrain",
                     'center': {'lon': (dutch_box[2] + dutch_box[0])/2, 'lat': (dutch_box[3] + dutch_box[1])/2 },
                     'zoom': 5.6},
                     showlegend = False)
                                   
    fig.write_html('../../Twitter_cred/tweets.html', auto_open=True)
    
if __name__ == '__main__':
    #file_name = '../../Twitter_cred/full_arch_2017.txt'

    #tweets = []
    #with open(file_name) as fp:
    #    for line in fp:
    #        tweets.append(json.loads(line))
    #data_list = pd.read_csv('../../pandafied_data/lat_lon_to_filename.csv')
    #plot_all(tweets,data_list)
    
    #data = pd.read_csv('../../pandafied_data/pandafied_twitter_XY.csv')
    #data = pd.read_csv('../../pandafied_data/curated_twitter_XY.csv')
    data = pd.read_csv('../../pandafied_data/pandafied_twitter_2007-2020.csv')
    data_list = [data]
    plot_pandafied(data_list)

import h5py
from os import listdir
from os.path import isfile, join
import pandas as pd
from pyproj import CRS, Transformer
import sys
from tqdm import tqdm

def pandafy_h5_make_radarXY(folder='../../KNMI_24h/KNMI-data_2019-12-09_14-07-28/rad_nl25_rac_mfbs_24h/2.0/0002/2016/12/31/RAD_NL25_RAC_MFBS_24H/2017/', save_name_radar='../../pandafied_data/pandafied_h5_radar.csv'):
    '''
        This function maps the y,x pixel coordinates of the KNMI precipitation data to boxes with a longitude latitude center and corner points.
    '''
    radar = {}
    radar['latlon_center'] = []
    radar['latlon_sw'] = []
    radar['latlon_se'] = []
    radar['latlon_ne'] = []
    radar['latlon_nw'] = []
    radar['radarX'] = []
    radar['radarY'] = []
    
    subfolder = "01/"
    onlyfiles = [f for f in listdir(folder+subfolder) if isfile(join(folder+subfolder, f))]
    h5_files = [f for f in onlyfiles if '.h5' in f]
    with h5py.File(folder+subfolder+h5_files[0], 'r') as f:
        proj4 = str(list(f['geographic/map_projection'].attrs.items())[2][1])
        proj4 = proj4[2:len(proj4)-1]
        from_proj = CRS.from_proj4(proj4)
        
        to_proj = CRS.from_proj4("+proj=latlong +datum=WGS84 +R=+12756274")#circumference around equator
        transform = Transformer.from_crs(from_proj, to_proj)
        y_offset = 3650.0
        x_offset = 0.0
        img = list(f['image1/image_data'])
        for i in tqdm(range(len(img))):
            for j in range(len(img[i])):
                if img[i][j] < 65535:
                    local_latlon_center = transform.transform(j+0.5-x_offset,-i-0.5-y_offset)
                    radar['latlon_center'].append((local_latlon_center[1],local_latlon_center[0]))
                    local_sw = transform.transform(j-x_offset,-i-1-y_offset)
                    radar['latlon_sw'].append((local_sw[1],local_sw[0]))
                    local_se = transform.transform(j+1-x_offset,-i-1-y_offset)
                    radar['latlon_se'].append((local_se[1],local_se[0]))
                    local_ne = transform.transform(j+1-x_offset,-i-y_offset)
                    radar['latlon_ne'].append((local_ne[1],local_ne[0]))
                    local_nw = transform.transform(j-x_offset,-i-y_offset)
                    radar['latlon_nw'].append((local_nw[1],local_nw[0]))
                    radar['radarX'].append(j)
                    radar['radarY'].append(i)
    
    radar = pd.DataFrame(radar)
    print(radar)
    radar.to_csv(save_name_radar,index=False)

if __name__ == '__main__':
    if len(sys.argv) >= 2 and sys.argv[1] == 'grace':
        pandafy_h5_make_radarXY(folder='/scratch/lamers/KNMI_24h/KNMI-data_2019-12-09_14-07-28/rad_nl25_rac_mfbs_24h/2.0/0002/2016/12/31/RAD_NL25_RAC_MFBS_24H/2017/')
    else:
        pandafy_h5_make_radarXY()

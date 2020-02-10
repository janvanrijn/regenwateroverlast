import h5py
from os import listdir
from os.path import isfile, join
import pandas as pd
from pyproj import CRS, Transformer
import sys
from tqdm import tqdm
from joblib import Parallel, delayed
import multiprocessing
import numpy as np

def parallel_data_parsing(subfolder,folder):
    data = {}
    data['rain'] = []
    data['radarX'] = []
    data['radarY'] = []
    data['date'] = []
    #data['start_time'] = []
    #data['stop_time'] = []
    data = pd.DataFrame(data)
    print("subfolder no.: " +str(subfolder))
    onlyfiles = [f for f in listdir(folder+subfolder) if isfile(join(folder+subfolder, f))]
    h5_files = [f for f in onlyfiles if '.h5' in f]
    h5_files.sort()
    sum_data = {}
    sum_data['rain'] = []
    sum_data['radarX'] = []
    sum_data['radarY'] = []
    sum_data['date'] = []
    sum_data = pd.DataFrame(sum_data)
    for filename_idx in range(len(h5_files)):
        temp_date = h5_files[filename_idx][22:30]
        temp_time = h5_files[filename_idx][30:34]
        if subfolder == '01/':
            print(filename_idx, len(h5_files))
        with h5py.File(folder+subfolder+h5_files[filename_idx], 'r') as f:
            img = np.array(list(f['image1/image_data']))
            len_y,len_x = img.shape
            local_data = {}
            local_data['rain']= img.flatten().tolist()
            local_data['radarY'] = [i for i in range(len_y) for j in range(len_x)]
            local_data['radarX'] = [i for i in range(len_x)]*len_y
            local_data['date']= [temp_date]*len_x*len_y
            local_data = pd.DataFrame(local_data)
            local_data = local_data[local_data.rain < 65535]
            sum_data = sum_data.append(local_data,sort=False)
            #for i, row in enumerate(img):
            #    for j, num  in enumerate(row):
            #        if num < 65535:
            #            local_data = {}
            #            local_data['rain']=num
            #            local_data['radarX']=[j]
            #            local_data['radarY']=[i]
            #            local_data['date']= [temp_date]
            #            local_data = pd.DataFrame(local_data)
            #            sum_data = sum_data.append(local_data,sort=False)
        if temp_time == '2400':
            sum_data = sum_data.groupby(['date','radarX','radarY'])['rain'].sum().reset_index(name='rain')
            data = data.append(sum_data,sort=False)
            sum_data = {}
            sum_data['rain'] = []
            sum_data['radarX'] = []
            sum_data['radarY'] = []
            sum_data['date'] = []
            sum_data = pd.DataFrame(sum_data)
    return data

def printname(name):
    print(name)

def pandafy_h5(save_name_radar='../../pandafied_data/pandafied_h5_radar.csv',save_name_rain='../../pandafied_data/pandafied_h5_rain.csv',folder = '../../KNMI_24h/KNMI-data_2019-12-09_14-07-28/rad_nl25_rac_mfbs_24h/2.0/0002/2016/12/31/RAD_NL25_RAC_MFBS_24H/2017/'):
    sub_folders = []
    for sub in range(1,13):
        if sub >= 10:
            subfolder = str(sub)+"/"
        else:
            subfolder = "0"+str(sub)+"/"
        sub_folders.append(subfolder)
    print(sub_folders)
    
    num_cores = multiprocessing.cpu_count()
    print("num_cores: " + str(num_cores))

    results = Parallel(n_jobs=num_cores)(delayed(parallel_data_parsing)(i,folder) for i in sub_folders)
    
    data = {}
    data['rain'] = []
    data['radarX'] = []
    data['radarY'] = []
    data['date'] = []
    data = pd.DataFrame(data)
    
    for result in results:
        data = data.append(result,sort=False)
    
    print(data[(data.radarX==442.0) & (data.radarY==251.0)])
    print(5)
    print(data)
    print("len index: ",len(data.index))
    print(6)
    data.to_csv(save_name_rain,index=False)
    print(7)

if __name__ == '__main__':
    if len(sys.argv) >= 2 and sys.argv[1] == 'grace':
        pandafy_h5(folder='/scratch/lamers/KNMI_24h/KNMI-data_2019-12-09_14-07-28/rad_nl25_rac_mfbs_24h/2.0/0002/2016/12/31/RAD_NL25_RAC_MFBS_24H/2017/')
    else:
        pandafy_h5()

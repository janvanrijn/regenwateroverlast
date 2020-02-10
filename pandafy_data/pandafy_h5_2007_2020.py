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

def parallel_data_parsing(subfolder,folder,year_counter,total_year,subfolder_counter,subfolder_total):
    data = {}
    data['rain'] = []
    data['radarX'] = []
    data['radarY'] = []
    data['date'] = []
    #data['start_time'] = []
    #data['stop_time'] = []
    data = pd.DataFrame(data)
    #print("subfolder no.: " +str(subfolder))
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
        print("year: ",year_counter," / ",total_year," subfolder: ", subfolder_counter, " / ",subfolder_total, " file: ",filename_idx," / ", len(h5_files))
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

def pandafy_h5(save_name_radar='../../pandafied_data/pandafied_h5_radar.csv',save_name_rain='../../pandafied_data/pandafied_h5_rain_2007-2020.csv',folder = '../../KNMI_big/KNMI-data_2020-01-29_15-56-00/rad_nl25_rac_mfbs_01h/2.0/0002/'):
    '''
        This function reads the KNMI precipitation data, aggregates it by summing up the amount of rain per day, it puts it into a pandas dataframe and saves it to disk.
    '''
    year_folders = [f for f in listdir(folder)]
    year_folders.sort()
    results = []
    year_counter = 0
    total_year = len(year_folders)
    for year_folder in year_folders:
        super_folder = folder+year_folder+'/01/01/RAD_NL25_RAC_MFBS_01H/'+year_folder+'/'
        month_folders = [f for f in listdir(super_folder)]
        month_folders.sort()
        for i in range(len(month_folders)):
            month_folders[i] += '/'
        num_cores = multiprocessing.cpu_count()
        print("num_cores: " + str(num_cores))

        results.append(Parallel(n_jobs=num_cores)(delayed(parallel_data_parsing)(month_folders[i],super_folder,year_counter,total_year,i,len(month_folders)) for i in range(len(month_folders))))
        year_counter += 1
    
    data = {}
    data['rain'] = []
    data['radarX'] = []
    data['radarY'] = []
    data['date'] = []
    data = pd.DataFrame(data)
    
    print("len(results)")
    for result in results:
        print(len(result))
    print("len(d.index)")
    for result in results:
        for d in result:
            print(len(d.index))
            data = data.append(d,sort=False)
    print(data[(data.radarX==442.0) & (data.radarY==251.0)])
    print(5)
    print(len(data.index))
    print(data)
    print("len index: ",len(data.index))
    print(6)
    data.to_csv(save_name_rain,index=False)
    print(7)

if __name__ == '__main__':
    if len(sys.argv) >= 2 and sys.argv[1] == 'grace':
        pandafy_h5(folder='/scratch/lamers/KNMI_big/KNMI-data_2020-01-29_15-56-00/rad_nl25_rac_mfbs_01h/2.0/0002/')
    else:
        pandafy_h5()

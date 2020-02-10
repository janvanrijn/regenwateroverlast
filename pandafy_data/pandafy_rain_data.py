import numpy as np
from os import listdir
from os.path import isfile, join
import sys
import csv
import pandas as pd

def pandafy_rain_data(data_folder_list=['../../export/Arnhem/20080101_20180101/','../../export/Enschede/20080101_20180101/','../../export/Zwolle/20080101_20180101/'],save_name='../../pandafied_data/pandafied_rain.csv'):
    data = []
    for data_folder in data_folder_list:
        onlyfiles = [f for f in listdir(data_folder) if isfile(join(data_folder, f))]
        csv_files = [f for f in onlyfiles if '.csv' in f]
        if 'Arnhem' in data_folder:
            city = 'Arnhem'
        elif 'Enschede' in data_folder:
            city = 'Enschede'
        elif 'Zwolle' in data_folder:
            city = 'Zwolle'
        else:
            print("error, unknown city")
            return
        
        for file in csv_files[0:len(csv_files)]:
            radarY = file[0:3]
            radarX = file[4:7]
            data_local = []
            with open(data_folder+file) as csvDataFile:
                csvReader = csv.reader(csvDataFile,delimiter=';')
                for row in csvReader:
                    data_local.append(row)
                        #just make one long array with attributes, place, rainfall,rainfall_to_date,rainfall_to_start_time, rainfall_to_end_time
            #TODO append kilometer vak uit file name in radarX, radarY
            data.append([city,data_local,radarX,radarY])
    rain = []
    start_time = []
    stop_time = []
    city = []
    date = []
    radarX = []
    radarY = []
    for instance in data:
        local_city = instance[0]
        local_radarX = int(instance[2])
        local_radarY = int(instance[3])
        for i in range(2,len(instance[1])):
            for j in range(1,len(instance[1][i])):
                if int(instance[1][i][j]) > 0:
                    rain.append(int(instance[1][i][j]))
                    city.append(local_city)
                    date.append(instance[1][i][0])
                    start_time.append(instance[1][i][0]+instance[1][0][j][0:2]+instance[1][0][j][3:5])
                    stop_time.append(instance[1][i][0]+instance[1][1][j][0:2]+instance[1][1][j][3:5])
                    radarX.append(local_radarX)
                    radarY.append(local_radarY)
    dict = {"rain":rain,"start_time":start_time,"stop_time":stop_time,"city":city,"date":date,"radarX":radarX,"radarY":radarY}
    df = pd.DataFrame(dict)
    df.to_csv(save_name,index=False)

if __name__ == '__main__':
    pandafy_rain_data()

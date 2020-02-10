import pandas as pd
import numpy as np

data = pd.read_csv('../../export/pandafied_rain.csv')

#print(data['date'].nunique())#3516
#print(max(data['rain']))#213800
#print(np.mean(data['rain']))#1123.8636439707918
#print(np.median(data['rain']))#600.0

rain_data = data[data.rain >= 600]
rain_data_2008 = rain_data[rain_data.date < 20090101]
print(rain_data_2008.date.unique())
print(min(rain_data_2008.date.unique()))
print(max(rain_data_2008.date.unique()))
print(rain_data_2008['date'].nunique())

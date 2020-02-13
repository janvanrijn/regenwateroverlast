import h5py
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import pandas as pd
import math

filename = '../../KNMI_24h/KNMI-data_2019-12-09_14-07-28/rad_nl25_rac_mfbs_24h/2.0/0002/2016/12/31/RAD_NL25_RAC_MFBS_24H/2017/01/RAD_NL25_RAC_MFBS_24H_201701010100_NL.h5'

with h5py.File(filename, 'r') as f:
    img_r = np.array(list(f['image1/image_data']))
    img_g = np.array(list(f['image1/image_data']))
    img_b = np.array(list(f['image1/image_data']))
clamp = 10000
for i in range(len(img_r)):
    for j in range(len(img_r[i])):
        if img_r[i][j] == 65535:
            img_r[i][j] = 255
            img_g[i][j] = 255
            img_b[i][j] = 255
        else: 
            img_r[i][j] = 127
            img_g[i][j] = 127
            img_b[i][j] = 127
folder = '../../pandafied_data/'
#threshold = pd.read_csv(folder + 'threshold.csv')
threshold = pd.read_csv(folder + 'n_threshold/n_threshold0_backup.csv')
twit_max = np.max(threshold['num_tweets'].values.tolist())
twit_norm = 1
for i in range(len(threshold.index)):
    y = int(threshold['radarY'][i])
    x = int(threshold['radarX'][i])
    if threshold['treshold'][i] >= clamp:
        val = 255
    else:
        val = 255*int(threshold['treshold'][i])//clamp
    if math.isnan(threshold['num_tweets'][i]):
        img_r[y][x] = 0
        img_g[y][x] = 0
        #img_b[y][x] = val//2+128
        img_b[y][x] = val
    else:
        #img_r[y][x] = 255*min(twit_norm,threshold['num_tweets'][i])//twit_norm
        #img_g[y][x] = val
        #img_b[y][x] = 0
        img_r[y][x] = 255
        img_g[y][x] = val
        img_b[y][x] = 0
print(twit_norm)

img = np.stack((img_r, img_g, img_b), axis=2)
#plt.gray()
plt.imshow(img)
plt.show()

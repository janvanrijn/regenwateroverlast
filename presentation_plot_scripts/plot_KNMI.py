import h5py
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

filename = '../../KNMI_24h/KNMI-data_2019-12-09_14-07-28/rad_nl25_rac_mfbs_24h/2.0/0002/2016/12/31/RAD_NL25_RAC_MFBS_24H/2017/01/RAD_NL25_RAC_MFBS_24H_201701010100_NL.h5'

with h5py.File(filename, 'r') as f:
    img = np.array(list(f['image1/image_data']))

for i in range(len(img)):
    for j in range(len(img[i])):
        if img[i][j] == 65535:
            img[i][j] = 100

print(np.max(img))
print(np.min(img))
plt.gray()
plt.imshow(img)
plt.show()

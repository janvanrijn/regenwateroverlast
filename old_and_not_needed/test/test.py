import csv
import numpy as np
import matplotlib.pyplot as plt

file_name = '../GeoTIFF_KNMI/A1/A1.csv'

data = []
with open(file_name) as csvDataFile:
    csvReader = csv.reader(csvDataFile)
    for row in csvReader:
        data.append(row)

data = np.array(data)
data = data.astype(np.float)
print(data.shape)
print(data)

data_norm = (data - np.min(data))/ (np.max(data) - np.min(data))
plt.imshow(data)
plt.gray()
plt.show()

#img = Image.fromarray(data, 'RGB')
#img.save('my.png')
#img.show()

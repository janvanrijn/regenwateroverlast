import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

threshold = pd.read_csv('../../pandafied_data/n_threshold/n_threshold0_backup.csv')
values, base = np.histogram(threshold['treshold'].values.tolist(), bins=4000)
cumulative = np.cumsum(values)
plt.plot(base[:-1], cumulative, c='blue')
plt.show()

import numpy as np
from PIL import Image
from scipy import ndimage
import matplotlib.pyplot as plt

x = Image.open("../presentation_plot_scripts/AHN2_5m_subsample.png")
#x.show()
x = np.array(x)
x = x[:,:,0]
kx = np.array([[1,0,-1],[2,0,-2],[1,0,-1]])
ky = np.array([[1,2,1] ,[0,0,0], [-1,-2,-1]])
sx=ndimage.convolve(x,kx)
sy=ndimage.convolve(x,ky)
plt.gray()
plt.imshow(sx)
plt.show()
plt.imshow(sy)
plt.show()

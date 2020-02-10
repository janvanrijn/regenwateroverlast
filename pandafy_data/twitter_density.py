#original code found at:
#https://towardsdatascience.com/simple-example-of-2d-density-plots-in-python-83b83b934f67

import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as st
from sklearn.datasets.samples_generator import make_blobs
import pandas as pd
import ast
#plt.ion()
#plt.show()

n_components = 3
#X, truth = make_blobs(n_samples=300, centers=n_components, 
#                      cluster_std = [2, 1.5, 1], 
#                      random_state=42)

latlon = []
tweets_XY = pd.read_csv('../../pandafied_data/pandafied_twitter_2007-2020_XY.csv')
for i in range(len(tweets_XY['latlon'])):
    #print(i, len(tweets_XY['latlon']))
    #print(tweets_XY['latlon'][i])
    try:
        if np.isnan(tweets_XY['latlon'][i]):
            continue
        else:
            latlon.append(ast.literal_eval(str(tweets_XY['latlon'][i])))
    except:
        latlon.append(ast.literal_eval(str(tweets_XY['latlon'][i])))

x = []
y = []
for i in latlon:
    y.append(i[0])
    x.append(i[1])

#plt.scatter(X[:, 0], X[:, 1], s=50, c = truth)
plt.scatter(x, y)
plt.title(f"Example of a mixture of {n_components} distributions")
plt.xlabel("x")
plt.ylabel("y");

plt.show()

#x = X[:, 0]
#y = X[:, 1]

deltaX = (max(x) - min(x))/10
deltaY = (max(y) - min(y))/10

xmin = min(x) - deltaX
xmax = max(x) + deltaX


ymin = min(y) - deltaY
ymax = max(y) + deltaY

print(xmin, xmax, ymin, ymax)


# Create meshgrid
xx, yy = np.mgrid[xmin:xmax:100j, ymin:ymax:100j]

positions = np.vstack([xx.ravel(), yy.ravel()])
values = np.vstack([x, y])
kernel = st.gaussian_kde(values)
print(values)
print(kernel)
print(kernel([[0],[0]]))
f = np.reshape(kernel(positions).T, xx.shape)



fig = plt.figure(figsize=(8,8))
ax = fig.gca()

cfset = ax.contourf(xx, yy, f, cmap='coolwarm')
ax.imshow(np.rot90(f), cmap='coolwarm', extent=[xmin, xmax, ymin, ymax])
cset = ax.contour(xx, yy, f, colors='k')
ax.clabel(cset, inline=1, fontsize=10)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_xlim(xmin, xmax)
ax.set_ylim(ymin, ymax)
plt.title('2D Gaussian Kernel density estimation')

plt.show()

from mpl_toolkits.mplot3d import axes3d



fig = plt.figure(figsize=(13, 7))
ax = plt.axes(projection='3d')
surf = ax.plot_surface(xx, yy, f, rstride=1, cstride=1, cmap='coolwarm', edgecolor='none')
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('PDF')
ax.set_title('Surface plot of Gaussian 2D KDE')
fig.colorbar(surf, shrink=0.5, aspect=5) # add color bar indicating the PDF
ax.view_init(60, 35)

plt.show()

fig = plt.figure(figsize=(13, 7))
ax = plt.axes(projection='3d')
w = ax.plot_wireframe(xx, yy, f)
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('PDF')
ax.set_title('Wireframe plot of Gaussian 2D KDE');

plt.show()

cset

plt.figure(figsize=(8,8))

for j in range(len(cset.allsegs)):
    for ii, seg in enumerate(cset.allsegs[j]):
        plt.plot(seg[:,0], seg[:,1], '.-', label=f'Cluster{j}, level{ii}')

plt.legend()
plt.show()

for l in cset.labelTexts:
    print(l)

h =plt.hist2d(x, y)
plt.colorbar(h[3])
plt.xlabel('x')
plt.ylabel('y')
plt.title('Frequency histogram')
#h
#
#h =plt.hist2d(x, y, normed = True)
#plt.colorbar(h[3]);
plt.show()



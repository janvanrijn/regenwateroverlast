import sys
import os
sys.path.append(os.path.realpath('../auto_encoder/'))
import auto_encoder_train

import json
import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
from matplotlib import cm
import seaborn as sns
sns.set(style="darkgrid")

def acc_plot_dims(parameters,xlabel,ylabel,title):
    history = parameters['history']
    for i in history:
        plt.plot(history[i]['test_loss'],label=str(i))
    #history =  pd.DataFrame(data=history)
    
    #fig, axs = plt.subplots(ncols=2)
    #sns.relplot(x="epochs_performed", y="train_acc",kind="line", data=history, ax=axs[0])
    #sns.relplot(x="epochs_performed", y="test_acc",kind="line", data=history, ax=axs[1])
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend(loc="upper left")
    plt.show()

def auto_encoder_plot(parameters,x_train,x_test,x_val,encoder,autoencoder,xlabel,ylabel,title):
    # encode and decode some digits
    # note that we take them from the *test* set
    #encoded_imgs = encoder.predict(x_test)
    #decoded_imgs = decoder.predict(encoded_imgs)
    decoded_imgs = autoencoder.predict(x_test)
    print(np.min(x_test),np.max(x_test))
    print(np.min(decoded_imgs),np.max(decoded_imgs))
    loss = autoencoder.evaluate(x_test,x_test)[0]
    print("loss: ",loss)
    #loss = (((int)(loss * 1000 + .5)) / 1000.0)
    
    #print(np.min(x_test),np.max(x_test))
    #print(np.min(decoded_imgs),np.max(decoded_imgs))
    #exit()
    plt.gray()
    #colormap = cm.gray
    if parameters['filter'] == 'sobel_components':
        n = 10  # how many digits we will display
        plt.figure(figsize=(20, 4))
        for i in range(n):
            # display original
            ax = plt.subplot(4, n, i + 1)
            plt.imshow(x_test[i].reshape(parameters['y'], parameters['x'],2)[:,:,0])#,cmap=colormap,vmin=np.min(x_test), vmax=np.max(x_test))
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
            ax = plt.subplot(4, n, i + 1 + n)
            plt.imshow(x_test[i].reshape(parameters['y'], parameters['x'],2)[:,:,1])#,cmap=colormap,vmin=np.min(x_test), vmax=np.max(x_test))
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
            
            # display reconstruction
            ax = plt.subplot(4, n, i + 1 + 2*n)
            plt.imshow(decoded_imgs[i].reshape(parameters['y'], parameters['x'],2)[:,:,0])#,cmap=colormap,vmin=np.min(x_test), vmax=np.max(x_test))
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
            ax = plt.subplot(4, n, i + 1 + 3*n)
            plt.imshow(decoded_imgs[i].reshape(parameters['y'], parameters['x'],2)[:,:,1])#,cmap=colormap,vmin=np.min(x_test), vmax=np.max(x_test))
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
    else:
        n = 10  # how many digits we will display
        plt.figure(figsize=(20, 4))
        for i in range(n):
            # display original
            ax = plt.subplot(2, n, i + 1)
            plt.imshow(x_test[i].reshape(parameters['y'], parameters['x']))
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
            
            # display reconstruction
            ax = plt.subplot(2, n, i + 1 + n)
            plt.imshow(decoded_imgs[i].reshape(parameters['y'], parameters['x']))
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
    #plt.title("loss: " +str(loss) + " " + title)
    plt.show()

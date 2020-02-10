import json
import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import sys
import seaborn as sns
sns.set(style="darkgrid")


file_name = str(sys.argv[1])
with open(file_name) as f:
    for line in f:
        history = json.loads(line)
history['epochs_performed'] = [i for i in range(history['epochs'])]
history =  pd.DataFrame(data=history)

fig, axs = plt.subplots(ncols=2)
sns.relplot(x="epochs_performed", y="train_acc",kind="line", data=history, ax=axs[0])
sns.relplot(x="epochs_performed", y="test_acc",kind="line", data=history, ax=axs[1])

plt.show()

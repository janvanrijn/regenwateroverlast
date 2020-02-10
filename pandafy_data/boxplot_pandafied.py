import json
import matplotlib
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

with open('ae_sobel_hist.json') as json_file:
    data_ae_sobel = json.load(json_file)

with open('downsample_sobel_hist.json') as json_file:
    data_downsample_sobel = json.load(json_file)

with open('downsample_no_sobel_hist.json') as json_file:
    data_downsample_no_sobel = json.load(json_file)

with open('rain_only_hist2.json') as json_file:
    data_only_rain = json.load(json_file)

with open('downsample_no_sobel_only3.json') as json_file:
    data_downsample_no_sobel_only_feat = json.load(json_file)

ae_sobel_easy = []
for matrix in data_ae_sobel['conf_matrix_test_hist']:
    matrix = np.array(matrix)
    all_neg = (matrix[0][0]+matrix[0][1])/np.sum(matrix)
    all_pos = (matrix[1][0]+matrix[1][1])/np.sum(matrix)
    ae_sobel_easy.append(max(all_neg,all_pos))

print(all_neg,all_pos)
ae_sobel_easy = np.array(ae_sobel_easy)
ae_sobel_acc = np.array(data_ae_sobel['acc_test_hist'])
ae_sobel_better = ae_sobel_acc-ae_sobel_easy

print("ae_sobel_easy")
print(np.average(ae_sobel_acc-ae_sobel_easy))
print(np.std(ae_sobel_acc-ae_sobel_easy))

downsample_sobel_easy = []
for matrix in data_downsample_sobel['conf_matrix_test_hist']:
    matrix = np.array(matrix)
    all_neg = (matrix[0][0]+matrix[0][1])/np.sum(matrix)
    all_pos = (matrix[1][0]+matrix[1][1])/np.sum(matrix)
    downsample_sobel_easy.append(max(all_neg,all_pos))

print(all_neg,all_pos)
downsample_sobel_easy = np.array(downsample_sobel_easy)
downsample_sobel_acc = np.array(data_downsample_sobel['acc_test_hist'])
downsample_sobel_better = downsample_sobel_acc-downsample_sobel_easy

print("downsample_sobel_easy")
print(np.average(downsample_sobel_acc-downsample_sobel_easy))
print(np.std(downsample_sobel_acc-downsample_sobel_easy))

downsample_no_sobel_easy = []
for matrix in data_downsample_no_sobel['conf_matrix_test_hist']:
    matrix = np.array(matrix)
    all_neg = (matrix[0][0]+matrix[0][1])/np.sum(matrix)
    all_pos = (matrix[1][0]+matrix[1][1])/np.sum(matrix)
    downsample_no_sobel_easy.append(max(all_neg,all_pos))
print(all_neg,all_pos)
downsample_no_sobel_easy = np.array(downsample_no_sobel_easy)
downsample_no_sobel_acc = np.array(data_downsample_no_sobel['acc_test_hist'])
downsample_no_sobel_better = downsample_no_sobel_acc-downsample_no_sobel_easy

print("downsample_no_sobel_easy")
print(np.average(downsample_no_sobel_acc-downsample_no_sobel_easy))
print(np.std(downsample_no_sobel_acc-downsample_no_sobel_easy))

only_rain_easy = []
for matrix in data_only_rain['conf_matrix_test_hist']:
    matrix = np.array(matrix)
    all_neg = (matrix[0][0]+matrix[0][1])/np.sum(matrix)
    all_pos = (matrix[1][0]+matrix[1][1])/np.sum(matrix)
    only_rain_easy.append(max(all_neg,all_pos))
print(all_neg,all_pos)
only_rain_easy = np.array(only_rain_easy)
only_rain_acc = np.array(data_only_rain['acc_test_hist'])
only_rain_better = only_rain_acc-only_rain_easy

print("only_rain_easy")
print(np.average(only_rain_acc-only_rain_easy))
print(np.std(only_rain_acc-only_rain_easy))

only_feat_easy = []
for matrix in data_downsample_no_sobel_only_feat['conf_matrix_test_hist']:
    matrix = np.array(matrix)
    all_neg = (matrix[0][0]+matrix[0][1])/np.sum(matrix)
    all_pos = (matrix[1][0]+matrix[1][1])/np.sum(matrix)
    only_feat_easy.append(max(all_neg,all_pos))
print(all_neg,all_pos)
only_feat_easy = np.array(only_feat_easy)
only_feat_acc = np.array(data_downsample_no_sobel_only_feat['acc_test_hist'])
only_feat_better = only_feat_acc-only_feat_easy

print("only_feat_easy")
print(np.average(only_feat_acc-only_feat_easy))
print(np.std(only_feat_acc-only_feat_easy))

for i in data_ae_sobel:
    print(i)
    print(data_ae_sobel[i])
    print()
#'acc','prec','rec','prec_neg','rec_neg','better'
plot_type = 'better'
df = {}
if plot_type == 'acc':
    df['ae sobel acc'] = data_ae_sobel['acc_test_hist']
    df['ds sobel acc'] = data_downsample_sobel['acc_test_hist']
    df['ds no sobel acc'] = data_downsample_no_sobel['acc_test_hist']
    df['only rain acc'] = data_only_rain['acc_test_hist']
    df['only feat acc'] = data_downsample_no_sobel_only_feat['acc_test_hist']
elif plot_type == 'prec':
    df['ae sobel prec pos'] = data_ae_sobel['precision_test_hist']
    df['ds sobel prec pos'] = data_downsample_sobel['precision_test_hist']
    df['ds no sobel prec pos'] = data_downsample_no_sobel['precision_test_hist']
    df['only rain prec pos'] = data_only_rain['precision_test_hist']
    df['only feat prec pos'] = data_downsample_no_sobel_only_feat['precision_test_hist']
elif plot_type == 'rec':
    df['ae sobel rec pos'] = data_ae_sobel['recall_test_hist']
    df['ds sobel rec pos'] = data_downsample_sobel['recall_test_hist']
    df['ds no sobel rec pos'] = data_downsample_no_sobel['recall_test_hist']
    df['only rain rec pos'] = data_only_rain['recall_test_hist']
    df['only feat rec pos'] = data_downsample_no_sobel_only_feat['recall_test_hist']
elif plot_type == 'prec_neg':
    df['ae sobel prec neg'] = data_ae_sobel['precision_test_neg_hist']
    df['ds sobel prec neg'] = data_downsample_sobel['precision_test_neg_hist']
    df['ds no sobel prec neg'] = data_downsample_no_sobel['precision_test_neg_hist']
    df['only rain prec neg'] = data_only_rain['precision_test_neg_hist']
    df['only feat prec neg'] = data_downsample_no_sobel_only_feat['precision_test_neg_hist']
elif plot_type == 'rec_neg':
    df['ae sobel rec neg'] = data_ae_sobel['recall_test_neg_hist']
    df['ds sobel rec neg'] = data_downsample_sobel['recall_test_neg_hist']
    df['ds no sobel rec neg'] = data_downsample_no_sobel['recall_test_neg_hist']
    df['only rain rec neg'] = data_only_rain['recall_test_neg_hist']
    df['only feat rec neg'] = data_downsample_no_sobel_only_feat['recall_test_neg_hist']
elif plot_type == 'better':
    df['ae sobel better'] = ae_sobel_better
    df['ds sobel better'] = downsample_sobel_better
    df['ds no sobel better'] = downsample_no_sobel_better
    df['only rain better'] = only_rain_better
    df['only feat better'] = only_feat_better
else:
    print("uknown plot type")
    exit()
df = pd.DataFrame(df)
#plt.boxplot(data_ae_sobel['acc_test_hist'])
#plt.boxplot(data_downsample_sobel['acc_test_hist'])
#plt.show()

#boxprops_good = dict(linestyle='-', linewidth=4, color='green')
#medianprops_good = dict(linestyle='-', linewidth=4, color='green')
fig = matplotlib.pyplot.gcf()
ax = df.plot(kind='box',showfliers=True,grid=True)
#ax = df.plot(kind='box',
#                             color=dict(boxes='g', whiskers='g', medians='g', caps='g'),
#                             boxprops=dict(linestyle='-', linewidth=2.0),
#                             flierprops=dict(linestyle='-', linewidth=2.0),
#                             medianprops=dict(linestyle='-', linewidth=2.0),
#                             whiskerprops=dict(linestyle='-', linewidth=2.0),
#                             capprops=dict(linestyle='-', linewidth=2.0),
#                             showfliers=True, grid=True, rot=0)

#normalized_df_bad.plot(kind='box',
#                    color=dict(boxes='r', whiskers='r', medians='r', caps='r'),
#                    boxprops=dict(linestyle='-', linewidth=1.0),
#                    flierprops=dict(linestyle='-', linewidth=1.0),
#                    medianprops=dict(linestyle='-', linewidth=1.0),
#                    whiskerprops=dict(linestyle='-', linewidth=1.0),
#                    capprops=dict(linestyle='-', linewidth=1.0),
#                    showfliers=False, grid=True, rot=0,ax=ax)
plt.tight_layout()
plt.show()

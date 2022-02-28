# %%
from scipy.io import arff
import numpy as np
import pandas as pd
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
from sklearn.feature_selection import SelectKBest, mutual_info_regression

data = arff.loadarff(r'/home/sara/apre/tpc-3/breast.w.arff')
df = pd.DataFrame(data[0])

data_array = df.to_numpy()

x = np.empty((0,9))
y = np.empty((0,1))

for row in data_array: 
    array_sum = np.sum(row[:-1])
    array_has_nan = np.isnan(array_sum)
    if(not array_has_nan):
        x = np.append(x, [row[:-1]], axis=0)
        y = np.append(y, row[-1])

kmeans = KMeans(n_clusters=3, random_state=0).fit(x)
y_kmeans = kmeans.predict(x)

y_aux = np.empty((0,1))

for idx, val in enumerate(y):
    if(val.decode('UTF-8') == 'benign'):
        y_aux = np.append(y_aux, 1)
    else:
        y_aux = np.append(y_aux, 0)

x_new = SelectKBest(score_func=mutual_info_regression, k=2).fit_transform(x, y_aux)

plt.scatter(x_new[y_kmeans==0, 0], x_new[y_kmeans==0, 1], s=100, c='red', label ='Cluster 1')
plt.scatter(x_new[y_kmeans==1, 0], x_new[y_kmeans==1, 1], s=100, c='blue', label ='Cluster 2')
plt.scatter(x_new[y_kmeans==2, 0], x_new[y_kmeans==2, 1], s=100, c='green', label ='Cluster 3')

centers = kmeans.cluster_centers_

plt.scatter(centers[:, 0], centers[:, 1], s=300, c='grey', label = 'Centroids')
plt.title('Clusters of breast cancer data set')
plt.xlabel('Cell Size Uniformity')
plt.ylabel('Cell Shape Uniformity')
plt.show()
# plt.scatter(x[:, 0], x[:, 1], c=cluster_labels, s=50)

# centers = kmeans.cluster_centers_
# plt.scatter(centers[:, 0], centers[:, 1], c='black', s=200, alpha=0.5);


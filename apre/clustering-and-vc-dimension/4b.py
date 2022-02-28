from scipy.io import arff
import numpy as np
import pandas as pd
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_samples, silhouette_score

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

range_n_clusters = [2,3]

for n_clusters in range_n_clusters:
    kmeans = KMeans(n_clusters=n_clusters, random_state=0).fit(x)
    cluster_labels = kmeans.predict(x)
    silhouette_avg = silhouette_score(x, cluster_labels)
    print(silhouette_avg)
    sample_silhouette_values = silhouette_samples(x, cluster_labels)
    #print(sample_silhouette_values)
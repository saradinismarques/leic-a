# %%
from scipy.io import arff
import numpy as np
import pandas as pd
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_samples, silhouette_score
import matplotlib.pyplot as plt
from sklearn.feature_selection import SelectKBest, mutual_info_regression

def calculate_ecr(clusters, y):
    c0, c1, c2 = 0,0,0
    m0, b0, m1, b1, m2, b2 = 0,0,0,0,0,0
    for i,j in zip(clusters, y):
        if (i == 0):
            c0 += 1
            if(j.decode('UTF-8') == 'benign'):
                b0 +=1
            else:
                m0 += 1
        if (i == 1):
            c1 += 1 
            if(j.decode('UTF-8') == 'benign'):
                b1 +=1
            else:
                m1 += 1
        if (i == 2):
            c2 += 1
            if(j.decode('UTF-8') == 'benign'):
                b2 +=1
            else:
                m2 += 1
          
    if(c2 == 0):
        ecr = (1/2)*((c0-max(b0,m0)+(c1-max(b1,m1))))
    else: 
        print(c0)
        print(c1)
        print(c2)
        print(b0)
        print(m0)
        print(b1)
        print(m1)
        print(b2)
        print(m2)

        ecr = (1/3)*((c0-max(b0,m0)+(c1-max(b1,m1))+(c2-max(b2,m2))))

    return ecr

data = arff.loadarff(r'/home/sara/apre/tpc-4/breast.w.arff')
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
    y_kmeans = kmeans.predict(x)
    ecr = calculate_ecr(y_kmeans, y)
    #print(ecr)
    # print(cluster_labels)
    

    
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


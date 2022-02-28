from scipy.io import arff
import numpy as np
import pandas as pd
from sklearn.model_selection import KFold 
from sklearn.neighbors import KNeighborsClassifier
from sklearn.naive_bayes import MultinomialNB
from sklearn.metrics import accuracy_score
from scipy import stats

data = arff.loadarff(r'/home/sara/apre/tpc-1/breast.w.arff')
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

kf = KFold(n_splits=10, random_state=20, shuffle=True)

knn = KNeighborsClassifier(n_neighbors=3)
mnb = MultinomialNB()
 
for train_index , test_index in kf.split(x):
    x_train , x_test = x[train_index], x[test_index]
    y_train , y_test = y[train_index], y[test_index]
    
    knn.fit(x_train,y_train)
    y_pred_knn = knn.predict(x_test)
    mnb.fit(x_train,y_train)
    y_pred_mnb = mnb.predict(x_test)

print("kNN:", accuracy_score(y_test, y_pred_knn))     
print("MultinomialNB:", accuracy_score(y_test, y_pred_mnb))

y_aux_knn = np.empty((0,1))
y_aux_mnb = np.empty((0,1))

for idx, val in enumerate(y_pred_knn):
    if(val.decode('UTF-8') == 'benign'):
        y_aux_knn = np.append(y_aux_knn, 1)
    else:
        y_aux_knn = np.append(y_aux_knn, 0)

for idx, val in enumerate(y_pred_mnb):
    if(val.decode('UTF-8') == 'benign'):
        y_aux_mnb = np.append(y_aux_mnb, 1)
    else:
        y_aux_mnb = np.append(y_aux_mnb, 0)

print(stats.ttest_rel(y_aux_knn, y_aux_mnb))
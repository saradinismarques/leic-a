from scipy.io import arff
import numpy as np
import pandas as pd
from sklearn.model_selection import KFold 
from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import accuracy_score
import statistics as st

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

knn = KNeighborsClassifier(n_neighbors=7)
 
acc_score_test = []
acc_score_train = []

for train_index , test_index in kf.split(x):
    x_train , x_test = x[train_index], x[test_index]
    y_train , y_test = y[train_index], y[test_index]
    
    knn.fit(x_train,y_train)
    y_pred_test = knn.predict(x_test)
    y_pred_train = knn.predict(x_train)

    acc_score_test.append(accuracy_score(y_pred_test , y_test))
    acc_score_train.append(accuracy_score(y_pred_train , y_train))

print("Accuracy testing set:", accuracy_score(y_test, y_pred_test))  
print('accuracy of each fold - {}'.format(acc_score_test))
print(st.stdev(acc_score_test))

print("Accuracy training set:", accuracy_score(y_train, y_pred_train))   
print('accuracy of each fold - {}'.format(acc_score_train))
print(st.stdev(acc_score_train))



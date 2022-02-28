# %%
from scipy.io import arff
import numpy as np
import pandas as pd
from sklearn import tree
import matplotlib.pyplot as plt
from sklearn.model_selection import KFold 
from sklearn.feature_selection import SelectKBest, mutual_info_regression
from sklearn.metrics import accuracy_score

data = arff.loadarff(r'/home/sara/apre/tpc-2/breast.w.arff')
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

y_aux = np.empty((0,1))

for idx, val in enumerate(y):
    if(val.decode('UTF-8') == 'benign'):
        y_aux = np.append(y_aux, 1)
    else:
        y_aux = np.append(y_aux, 0)

num_features = [1, 3, 5, 9]

train_acc_f = []
test_acc_f = []

for i in num_features:
    x_new = SelectKBest(score_func=mutual_info_regression, k=i).fit_transform(x, y_aux)

    kf = KFold(n_splits=10, random_state=20, shuffle=True)
    dtc = tree.DecisionTreeClassifier()

    for train_index , test_index in kf.split(x_new):
        x_train , x_test = x[train_index], x[test_index]
        y_train , y_test = y[train_index], y[test_index]
        dtc.fit(x_train, y_train)
    
        y_pred_test = dtc.predict(x_test)
        y_pred_train = dtc.predict(x_train)

    test_acc_f.append(accuracy_score(y_test, y_pred_test))
    train_acc_f.append(accuracy_score(y_train, y_pred_train))

plt.plot(num_features, train_acc_f, label='Training Set') 
plt.plot(num_features, test_acc_f, label='Testing Set') 
plt.xlabel('Number of Features') 
plt.ylabel('Accuracy')
plt.legend()
plt.show()

max_depth = [1, 3, 5, 9]

train_acc_d = []
test_acc_d = []

for i in max_depth:
    x_new = SelectKBest(k='all').fit_transform(x, y_aux)

    kf = KFold(n_splits=10, random_state=20, shuffle=True)
    dtc = tree.DecisionTreeClassifier(max_depth=i)

    for train_index , test_index in kf.split(x_new):
        x_train , x_test = x[train_index], x[test_index]
        y_train , y_test = y[train_index], y[test_index]
        dtc.fit(x_train, y_train)
    
        y_pred_test = dtc.predict(x_test)
        y_pred_train = dtc.predict(x_train)

    test_acc_d.append(accuracy_score(y_test, y_pred_test))
    train_acc_d.append(accuracy_score(y_train, y_pred_train))

plt.plot(max_depth, train_acc_d, label='Training Error')
plt.plot(max_depth, test_acc_d, label='Testing Error') 
plt.xlabel('Maximum Depth') 
plt.ylabel('Accuracy') 
plt.legend()
plt.show() 



   


# %%

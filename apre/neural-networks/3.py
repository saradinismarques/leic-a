# %%
from scipy.io import arff
import numpy as np
import pandas as pd
from sklearn.model_selection import KFold 
from sklearn.neural_network import MLPRegressor
from matplotlib import pyplot as plt
from sklearn.metrics import mean_absolute_percentage_error

data = arff.loadarff(r'/home/sara/apre/tpc-3/kin8nm.arff')
df = pd.DataFrame(data[0])

data_array = df.to_numpy()

x = np.empty((0,8))
y = np.empty((0,1))

for row in data_array: 
    array_sum = np.sum(row[:-1])
    array_has_nan = np.isnan(array_sum)
    if(not array_has_nan):
        x = np.append(x, [row[:-1]], axis=0)
        y = np.append(y, row[-1])

kf = KFold(n_splits=5, random_state=0, shuffle=True)

clf = MLPRegressor(activation='relu', hidden_layer_sizes=(3,2), max_iter=2000, shuffle=False, random_state=False, alpha=10) 

y_pred = np.empty((0,1))
y_pred_train = np.empty((0,1))
for train_index , test_index in kf.split(x):
    x_train , x_test = x[train_index], x[test_index]
    y_train , y_test = y[train_index], y[test_index]
    
    clf.fit(x_train,y_train)
    y_pred = clf.predict(x_test)
    y_pred_train = clf.predict(x_train)

print(y_test)
#print(y_pred)
plt.boxplot(y_test-y_pred)
plt.show()  # Shows plot

print(mean_absolute_percentage_error(y_train,y_pred_train))   
print(mean_absolute_percentage_error(y_test,y_pred))  

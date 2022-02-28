from scipy.io import arff
import numpy as np
import pandas as pd
from sklearn.model_selection import KFold 
from sklearn.neighbors import KNeighborsClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.metrics import accuracy_score
from scipy import stats

gnb = GaussianNB()

x_train = [[0.6, 1, 0.2, 0.4], [0.1, 2, -0.1, -0.4], [0.2, 1, -0.1, 0.2], [0.1, 3, 0.8, 0.8], [0.3, 2, 0.1, 0.3], [-0.1, 3, 0.2, -0.2], [-0.3, 3, -0.1, 0.2], [0.2, 2, 0.5, 0.6], [0.4, 1, -0.4, -0.7], [-0.2, 3, 0.4, 0.3]]
y_train = [0,0,0,0,1,1,1,1,1,1]
x_test = [[0.6, 1, 0.2, 0.4], [0.1, 2, -0.1, -0.4], [0.2, 1, -0.1, 0.2], [0.1, 3, 0.8, 0.8], [0.3, 2, 0.1, 0.3], [-0.1, 3, 0.2, -0.2], [-0.3, 3, -0.1, 0.2], [0.2, 2, 0.5, 0.6], [0.4, 1, -0.4, -0.7], [-0.2, 3, 0.4, 0.3]]
y_test = [0,0,0,0,1,1,1,1,1,1]

gnb.fit(x_train,y_train)
y_pred_gnb = gnb.predict(x_test)
print(gnb.predict_proba(x_test))  

print(y_pred_gnb)

print("NB:", accuracy_score(y_test, y_pred_gnb))


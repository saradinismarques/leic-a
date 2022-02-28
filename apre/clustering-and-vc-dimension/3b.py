# %%

import matplotlib.pyplot as plt
import numpy as np

def mlp(i):
    return 3*i*i + 3*i + i + 1

def tree(i):
    return np.power(2, i)

def nb(i):
    l = i*1 + (i*i-i)/2 + i
    return 2*l + 1

x = [2,5,10,12,13]
y_mlp = [0] * 5
y_tree = [0] * 5
y_nb = [0] * 5

j = 0

for i in x:
    y_mlp[j] = mlp(i)
    y_tree[j] = tree(i)
    y_nb[j] = nb(i)
    j += 1

plt.plot(x, y_mlp, label = "MLP")
plt.plot(x, y_tree, label = "Decision Tree")
plt.plot(x, y_nb, label = "Bayesian Classifier")

plt.xlabel('Data dimensionality')
plt.ylabel('VC dimension')
plt.legend()
plt.show()

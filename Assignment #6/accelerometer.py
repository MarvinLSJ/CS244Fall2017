import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn import svm
import mltools as ml

data = np.genfromtxt('train.txt')
test = np.genfromtxt('test.txt')
# print(data.shape)

def func(X, Y, Z):
    d = np.sqrt(X * X + Y * Y + Z * Z)
    # print(d)
    datasize = X.shape[0]
    windowsize = 450
    stepsize = 200
    i = 0
    fmax = []
    fmin = []
    fmean = []
    fvar = []
    valuestep = datasize / stepsize
    rest = windowsize / stepsize - 1
    while i <= datasize - windowsize:
        temp = d[i:i + windowsize]
        # print(temp.shape)
        fmax.append(np.max(temp))
        fmin.append(np.min(temp))
        fmean.append(np.mean(temp))
        fvar.append(np.var(temp))
        i += stepsize

    value = [1] * int(valuestep / 4) + [2] * int(valuestep / 4) + [3] * int(valuestep / 4) + [4] * int(
        valuestep / 4 - rest)
    # print(len(value))
    # # print('datapointsnum:', len(fmax))
    # # print(fmax)
    # value = np.array(value).T
    dataset = np.array([fmax, fmin, fmean, fvar]).T
    return dataset, value;

X = data[:,0]
Xte = test[:,0]
Y = data[:,1]
Yte = test[:,1]
Z = data[:,2]
Zte = test[:,2]
labtr = data[:,3]
labte = test[:,3]
# print("Xte", Xte.shape)
datatr, valuetr = func(X,Y,Z)
datate, valuete = func(Xte,Yte,Zte)
print(datatr.shape, "  ", len(valuetr))
# print("valuetr", valuetr.shape)
# datatr, valuetr = ml.shuffleData(datatr, valuetr)
# Xva, Xtr, Yva, Ytr = ml.splitData(datatr, valuetr, 0.0625)
# Xtr, Xva, Ytr, Yva = ml.splitData(datatr, valuetr, 0.8)
# Xtr, Ytr = ml.shuffleData(datatr,valuetr)
learner = svm.SVC(decision_function_shape='ovo')
learner.fit(datatr,valuetr)
Yhat = learner.predict(datate)
print(Yhat)
sum=0
for a in range(len(Yhat)):
    sum += (Yhat[a]!= 1)
    # print("Yhat", Yhat[a], "v", valuete[a])

print(sum)
print("Error Rate: ", sum/len(Yhat)*100,"%")









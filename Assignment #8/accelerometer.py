import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn import svm
import mltools as ml

data = np.genfromtxt('train.txt')
test = np.genfromtxt('test1.txt')
print(data.shape)

def func(X, Y, Z):
    d = np.sqrt(X * X + Y * Y + Z * Z)
    # print(d)
    datasize = X.shape[0]
    windowsize = 300
    stepsize = 100
    i = 0
    fmax = []
    fmin = []
    fmean = []
    fvar = []
    f20 =[]
    f40 = []
    f60 = []
    f80 = []
    fx = []
    fy = []
    fz = []
    valuestep = datasize / stepsize
    rest = windowsize / stepsize - 1
    while i <= datasize - windowsize:
        temp = d[i:i + windowsize]
        # print(temp)
        fx.append(np.mean(X[i:i + windowsize]))
        fy.append(np.mean(Y[i:i + windowsize]))
        fz.append(np.mean(Z[i:i + windowsize]))
        fmax.append(np.max(temp))
        fmin.append(np.min(temp))
        fmean.append(np.mean(temp))
        fvar.append(np.var(temp))
        f20.append(np.percentile(temp, 20))
        f40.append(np.percentile(temp, 40))
        f60.append(np.percentile(temp, 60))
        f80.append(np.percentile(temp, 80))
        i += stepsize

    value = [1] * int(valuestep / 6) + [2] * int(valuestep / 6  ) + [3] * int(valuestep / 6 ) + [4] * int(valuestep / 6) + [5] * int(valuestep / 6) + [6] * int(valuestep / 6 -rest)
            #+ [3] * int(valuestep / 4) + [4] * int(valuestep / 4 - rest)
    # print(len(value))
    # # print('datapointsnum:', len(fmax))
    # # print(fmax)
    # value = np.array(value).T
    dataset = np.array([fx, fy, fz, fmax, fmin, fmean, fvar, f20, f40, f60, f80]).T
    return dataset, value;

X = data[:,0]
Xte = test[:,0]
Y = data[:,1]
Yte = test[:,1]
Z = data[:,2]
Zte = test[:,2]

# print("Xte", Xte.shape)
datatr, valuetr = func(X,Y,Z)
# print(np.mean(datatr[0:49,3]))
# print(np.mean(datatr[49:98,3]))

datate, valuete = func(Xte,Yte,Zte)
print("te",valuete)
# datatr = datatr[:,1:3];
# datate = datate[:,1:3];
print(datatr.shape, "  ", len(valuetr))
# print("valuetr", valuetr.shape)
datatr, valuetr = ml.shuffleData(datatr, valuetr)

# Xva, Xtr, Yva, Ytr = ml.splitData(datatr, valuetr, 0.0625)
# Xtr, Xva, Ytr, Yva = ml.splitData(datatr, valuetr, 0.8)
# Xtr, Ytr = ml.shuffleData(datatr,valuetr)
learner = svm.SVC(decision_function_shape='ovo')
learner.fit(datatr,valuetr)
Yhat = learner.predict(datate)

print("yhat",Yhat)
sum=0
for a in range(len(Yhat)):
    sum += (Yhat[a]!= 4)
    # print("Yhat", Yhat[a], "v", valuete[a])

print(sum)
print("Error Rate: ", sum/len(Yhat)*100,"%")









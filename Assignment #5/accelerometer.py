import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn import svm
import mltools as ml

data = np.genfromtxt('data.txt')
# print(data.shape)

X = data[:,0]
Y = data[:,1]
Z = data[:,2]
labels = data[:,3]
d = np.sqrt(X*X + Y*Y + Z*Z)
# print(d)
datasize = labels.shape[0]
windowsize = 150
stepsize = 25
i=0
fmax=[]
fmin=[]
fmean=[]
fvar=[]
valuestep = datasize/stepsize
rest = windowsize/stepsize - 1
while i<=datasize - windowsize:
    temp = d[i:i+windowsize]
    # print(temp.shape)
    fmax.append(np.max(temp))
    fmin.append(np.min(temp))
    fmean.append(np.mean(temp))
    fvar.append(np.var(temp))
    i += stepsize

value = [1]*int(valuestep/5)+[2]*int(valuestep/5)+[3]*int(valuestep/5)+[4]*int(valuestep/5)+[5]*int(valuestep/5-rest)
print(len(value))
# print('datapointsnum:', len(fmax))
# print(fmax)
value = np.array(value).T
dataset = np.array([fmax,fmin,fmean,fvar]).T
print(dataset.shape)
dataset, value = ml.shuffleData(dataset, value)
Xtr, Xva, Ytr, Yva = ml.splitData(dataset, value, 0.75);

learner = svm.SVC(decision_function_shape='ovo')
learner.fit(Xtr,Ytr)
Yhat = learner.predict(Xva)
sum=0
for a in range(len(Yhat)):
    sum += (Yhat[a]!=Yva[a])

print(sum)
print(sum/len(Yhat)*100,"%")









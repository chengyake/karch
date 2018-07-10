#!/usr/bin/python3
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

avg_num = 5

data = [
0.8678
,0.8676
,0.8665
,0.8674
,0.8687
,0.868
,0.8686
,0.8664
,0.867
,0.8705
,0.8688
,0.8663
,0.8682
,0.8668
,0.8677
,0.8687
,0.8674
,0.8671
,0.869
,0.868
,0.8671
,0.8672
,0.8678
,0.8688
,0.8672
,0.8693
,0.8673
,0.8678
,0.8664
,0.8676
,0.8667
,0.8679
,0.8665
,0.8666
,0.8669
,0.868
,0.8671
,0.8667
,0.8673
,0.8671
,0.8666
,0.8673
,0.8701
]


def avg(data, num=1):
    l = len(data)
    if l <= num or num==1:
        return data
    y=[]
    for i in range(l-num+1):
        y.append(0)
        for j in range(num):
            y[i] += data[i+j]
        y[i]/=num
    return y

y = avg(data, avg_num)

mpl.rcParams['xtick.labelsize'] = 10
mpl.rcParams['ytick.labelsize'] = 10
plt.figure('data')#windows name
#plt.plot(x, y, '.')
plt.plot(range((avg_num+1)/2, len(y)+(avg_num+1)/2), y, 'r', lw=1)
plt.plot(range(len(data)), data, 'k', lw=1)
#plt.scatter(x, y)
plt.show()




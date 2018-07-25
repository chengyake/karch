#!/usr/bin/python3
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt



class showLine:

    def __init__(self, name, data, avg=3):
        self.data = data

        y = self.avg_process(self.data, avg)
        mpl.rcParams['xtick.labelsize'] = 10
        mpl.rcParams['ytick.labelsize'] = 10
        plt.figure(name)#windows name
        plt.plot(range(int((avg+1)/2), len(y)+int((avg+1)/2)), y, 'r', lw=1)
        plt.plot(range(len(data)), data, 'k', lw=1)
        plt.show()
        #plt.savefig('test.png')

    def avg_process(self, data, num=1):
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

if __name__ == '__main__':
    data = [0.8678,0.8676,0.8665,0.8674,0.8687,0.868,0.8686,0.8664,0.867,0.8705,0.8688,0.8663,0.8682,0.8668,0.8677,0.8687,0.8674,0.8671,0.869,0.868,0.8671,0.8672,0.8678,0.8688,0.8672,0.8693,0.8673,0.8678,0.8664,0.8676,0.8667,0.8679,0.8665,0.8666,0.8669,0.868,0.8671,0.8667,0.8673,0.8671,0.8666,0.8673,0.8701]
    sl = showLine(data, 3)
    print('------------')




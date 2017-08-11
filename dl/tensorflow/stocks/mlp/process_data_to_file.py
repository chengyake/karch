# -*- coding: utf-8 -*-

import os
import sys
import linecache
import numpy as np

totalnum =3383
onelines =2573
filename='./2007_2017_with_code'

width=32
height=1024
channel=4
batch=onelines-2-(width+1) # 2 = first "open line" + (line+1 - line)
batch=batch+1

print('train:',batch, width, height, channel)

x_train = np.zeros((batch, width, height, channel), dtype='float16')
y_train = np.zeros((batch, height), dtype='float16')

x_test = np.zeros((batch, width, height, channel), dtype='float16')
y_test = np.zeros((batch, height), dtype='float16')


#he
def get_line(idx, offset):
    return linecache.getline(filename, idx*onelines+offset+2)

def get_data(idx, offset):
    line = get_line(idx, offset)   
    t = line.split(',')
    if len(t[1])<2:
        return .0, .0, .0, .0
    t = np.array([t[1], t[2], t[3], t[4][0:-2]])
    return t.astype(float)

def get_percent(idx, offset):
    a,b,c,d = get_data(idx, offset)
    x,y,z,w = get_data(idx, (offset+1))
    if a*b*c*d!=0:
        return (x-a)/a, (y-b)/b, (z-c)/c, (w-d)/d
    else:
        return .0, .0, .0, .0



def fill_data():
    for i in range(batch):
        for k in range(height):
            for j in range(width):
                x_train[i, j, k, :] = get_percent(k, i+j)
                x_test[i, j, k, :] = get_percent(k+2032, i+j)
            b,y_train[i, k], q, r = get_percent(k, i+width)
            b,y_test[i, k], q, r = get_percent(k+2032, i+width)
        #sys.stdout.write("\r"+str(i)+" : " + str(batch) )
        print(str(i)+" : " + str(batch) )




def main():

    global x_train, y_train, x_test, y_test
    
    fill_data()

    x_train.tofile("x_train.bin")                                  
    y_train.tofile("y_train.bin")   

    x_test.tofile("x_test.bin")                                  
    y_test.tofile("y_test.bin")   

    print("make datasets success")







main()









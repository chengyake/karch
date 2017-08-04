# -*- coding: utf-8 -*-

import os
import linecache
import numpy as np

totalnum =3383
onelines =2573
filename='./2007_2017_with_code'

bat=0
bat2=0
batch=500000
batch2=5000
width=32
height=32
channel=4

x_train = np.zeros((batch, width, height, channel), dtype='uint8')
y_train = np.zeros(batch, dtype='uint8')

x_test = np.zeros((batch2, width, height, channel), dtype='uint8')
y_test = np.zeros(batch2, dtype='uint8')

def get_line(idx, offset):
    return linecache.getline(filename, idx*onelines+offset+1)

def get_data(idx, offset):
    line = get_line(idx, offset)   
    t = line.split(',')
    if len(t[1])<2:
        return 0.0, 0.0, 0.0, 0.0
    return float(t[1]), float(t[2]), float(t[3]), float(t[4][0:-2])

def get_percent(idx, offset):
    a,b,c,d = get_data(idx, offset)
    x,y,z,w = get_data(idx, (offset+1))
    if a*b*c*d!=0:
        return (x-a)/a, (y-b)/b, (z-c)/c, (w-d)/d
    else:
        return .0, .0, .0, .0


def fill_data(idx, offset):
    global bat, x_train, y_train
    for w in range(width):
        m = np.array(get_percent(idx, offset-width+w))
        m[m<-0.1] = -0.1
        m[m>0.1] = 0.1
        x_train[bat, w, 0, :] = (m*10)*127+128

    for h in range(1, height):
        x_train[bat, :, h, :] = x_train[bat, :, 0, :]

    _a, y, _b, _c = get_percent(idx, offset)
    if y > 0.1:
        y=0.1
    if y < -0.1:
        y=-0.1
    y_train[bat] = y*10*127+128
    bat=bat+1


def fill_data2(idx, offset):
    global bat2, x_test, y_test
    for w in range(width):
        m = np.array(get_percent(idx, offset-width+w))
        m[m<-0.1] = -0.1
        m[m>0.1] = 0.1
        x_test[bat2, w, 0, :] = (m*10)*127+128

    for h in range(1, height):
        x_test[bat2, :, h, :] = x_test[bat2, :, 0, :]

    _a, y, _b, _c = get_percent(idx, offset)
    if y > 0.1:
        y=0.1
    if y < -0.1:
        y=-0.1
    y_test[bat2] = y*10*127+128
    bat2=bat2+1


def fill_all_data():

    global bat, batch
    for s in range(totalnum):
        for d in range(width+1, onelines-1):  #onelines - (onelines-1) -> latest y
            fill_data(s, d)
            if bat >= batch:
                return bat

def fill_all_data2():

    global bat2, batch2
    for s in range(3300, totalnum):
        for d in range(width+1, onelines-1):  #onelines - (onelines-1) -> latest y
            fill_data2(s, d)
            if bat2 >= batch2:
                return bat2
   


def main():
    global x_train, y_train
    
    fill_all_data()
    fill_all_data2()

    x_train.tofile("x_train.bin")                                  
    y_train.tofile("y_train.bin")   

    x_test.tofile("x_test.bin")                                  
    y_test.tofile("y_test.bin")   

    '''
    b = np.fromfile("x_train.bin", dtype=np.uint8) 
    
    b = b.reshape((batch, width, height, channel))
    '''



main()






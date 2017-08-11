# -*- coding: utf-8 -*-



import os
import linecache
import numpy as np
import pandas as pd
import xlwt     ##< 256


totalnum =3383
onelines =2573
filename='./2007_2017_with_code'

width=32
height=1024
channel=4
batch=onelines-width-3
batch=batch+1



def main():
    global batch, width, height, channel
    #global x_train, y_train
    
    #fill_all_data()
    #fill_all_data2()

    #x_train.tofile("x_train.bin")                                  
    #y_train.tofile("y_train.bin")   

    #x_test.tofile("x_test.bin")                                  
    #y_test.tofile("y_test.bin")   
    x_train = np.fromfile("x_train.bin", dtype=np.float16) 
    y_train = np.fromfile("y_train.bin", dtype=np.float16) 
    x_test = np.fromfile("x_test.bin", dtype=np.float16) 
    y_test = np.fromfile("y_test.bin", dtype=np.float16) 

    x_train = x_train.reshape((batch, width, height, channel))
    y_train = y_train.reshape((batch, height))
    x_test = x_test.reshape((batch, width, height, channel))
    y_test = y_test.reshape((batch, height))

    #np.savetxt("x_train.txt", x_train, fmt="%f")
    #np.savetxt("y_train.txt", y_train, fmt="%f")
    #np.savetxt("x_test.txt",   x_test, fmt="%f")
    #np.savetxt("y_test.txt",   y_test, fmt="%f")
    
    #workbook = xlwt.Workbook(encoding='ascii')
    #worksheet = workbook.add_sheet('x_train')
    #for b in range(batch):
    #    for h in range(height):
    #        for w in range(width):
    #            for c in range(channel):
    #                worksheet.write(h*4+c, b*32+w, label = x_train[b,w,h,c].tostring())
    #workbook.save('data.xls')


    writer = pd.ExcelWriter('train.xlsx')
    
    for b in range(batch):
        for h in range(height/10):
            tmp = pd.DataFrame(x_train[b, :, h, :])
            tmp.to_excel(writer, 'x_train', float_format='%.5f', startrow = b*33, startcol = h*5)

    tmp = pd.DataFrame(y_train)
    tmp.to_excel(writer, 'y_train', float_format='%.5f', startrow = 0, startcol = 0)

    writer.save()


    writer = pd.ExcelWriter('test.xlsx')
    for b in range(batch):
        for h in range(height/10):
            tmp = pd.DataFrame(x_test[b, :, h, :])
            tmp.to_excel(writer, 'x_test', float_format='%.5f', startrow = b*33, startcol = h*5)

    tmp = pd.DataFrame(y_test)
    tmp.to_excel(writer, 'y_test', float_format='%.5f', startrow = 0, startcol = 0)


    writer.save()

main()






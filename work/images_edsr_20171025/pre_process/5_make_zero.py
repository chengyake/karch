#! /usr/bin/python
# -*- coding: utf-8 -*-  

import os
import sys
import numpy as np  
import matplotlib.pyplot as plt  
from PIL import Image, ImageFilter

  
def get_file_list(fmt, d):
    'add all fmt file in d to l'
    l=[]
    if not (os.path.exists(d)):
        print "folder %s is missing" % d
        return
    for path, dirname, files in os.walk(d):
        for filename in files:
            if filename.find(fmt)!=-1:
                l.append(path+"/"+filename)
    return l


def get_filename_list(fmt, d):
    'add all fmt file in d to l'
    l=[]
    if not (os.path.exists(d)):
        print "folder %s is missing" % d
        return
    for path, dirname, files in os.walk(d):
        for filename in files:
            if filename.find(fmt)!=-1:
                l.append(filename)
    return l





l = get_filename_list('.bmp', "./mu1_crop")
l.sort()

for f in l:
    im1 = Image.open("./mu1_crop/" + f)
    im2 = Image.open("./trs_register/" + f)
    w,h = im2.size

    iv1 = np.asarray(im1, dtype='uint16')
    iv2 = np.asarray(im2, dtype='uint16')
    
    for i in range(h-1):
        for j in range(w-1):
            if np.sum(iv2[0:i, j]) != 0 and np.sum(iv2[i+1:w, j])!=0 and np.sum(iv2[i, 0:j])!=0  and np.sum(iv2[i, j+1:h]) != 0:
                iv2[i,j]=255



    iv1[iv2==0] = 0

    iv1 = iv1.astype(np.uint8)
    im = Image.fromarray(iv1) 

    im.save("./mu1_register/" + f)


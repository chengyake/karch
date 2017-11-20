#! /usr/bin/python
# -*- coding: utf-8 -*-  

import os
import sys
import numpy as np  
import matplotlib.pyplot as plt  
#import display_line as dl
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

# > 200  ; normally 4000
def detect_red_pixels(pathname, th=8.0):

    im = Image.open(pathname).convert("RGB")
    w, h = im.size
    s=0
    for i in range(0, w):
          for j in range(0, h):
                r, g, b = im.getpixel((i, j))
                if r/(g+b+1) > th:
                    s+=1
    return s



#print detect_xy_range("/home/yake/Desktop/yake37.bmp")


def crop_trs(path, fmt, to):
    l =  get_file_list(fmt, path)
    index=0
    for i in l:
        if detect_red_pixels(i) < 200:
            im = Image.open(i).convert("L")
            w, h = im.size
            region = im.crop((60,10, w-75, h-50))
            region.save("%s/%08d.bmp" % (to, index))
            index+=1
    return
    


crop_trs("./mu1_img", ".png", "./mu1_crop")

























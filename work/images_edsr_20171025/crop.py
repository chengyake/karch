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



def detect_xy_box(im):


#    k5 = (0.0, 0.0, -2.0, 0.0, 0.0,
#         0.0, 0.0, -2.0, 0.0, 0.0,
#         -2.0, -2.0,  16.0, -2.0, -2.0,
#         0.0, 0.0, -2.0, 0.0, 0.0,
#         0.0, 0.0, -2.0, 0.0, 0.0)
    k3 = ( 0.0, -2.0,  0.0,
          -2.0,  8.0, -2.0, 
           0.0, -2.0,  0.0)
    imf=im.filter(ImageFilter.Kernel((3,3), k3))
    
    ws=[]
    hs=[]
    w, h = imf.size
    for i in range(0, w):
        s = 0
        for j in range(0, h):
            s+=imf.getpixel((i, j))
        ws.append(s)
    #dl.display_line(ws, "width")
    ws1 = ws[:w/2]
    ws2 = ws[w/2:]
    wmax_left = max(ws1)
    wmax_right = max(ws2)
    x1 = ws1.index(wmax_left)
    x2 = ws2.index(wmax_right)+w/2

    for j in range(0, h):
        s = 0
        for i in range(0, w):
            s+=imf.getpixel((i, j))
        hs.append(s)
    #dl.display_line(hs, "High")
    hs1 = hs[59:h/2]
    hs2 = hs[h/2:]
    hmax_left = max(hs1)
    y1 = hs1.index(hmax_left)+59

    y2=0
    hs2.reverse()
    for i in hs2:
        if i > 250:
            break
        y2+=1
    y2 = h - y2

    if wmax_right < 90000 or hmax_left < 110000:
        return None

    return x1, y1, x2, y2 




#print detect_xy_range("/home/yake/Desktop/yake37.bmp")


def crop_trs(path, fmt, to)
l =  get_file_list(fmt, path)
index=0
for i in l:
    im = Image.open(i).convert("L")
    box = detect_xy_box(im)
    if box != None:
        region = im.crop(box)
        region.save("%s/%08d.bmp" % (to, index))
        index+=1


crop("/home/yake/Desktop/trs_crop", ".bmp", "/home/yake/Desktop/crop/"

























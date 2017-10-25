#! /usr/bin/python
# -*- coding: utf-8 -*-  

import os
import sys
import numpy as np  
import matplotlib.pyplot as plt  
#import display_line as dl
from PIL import Image, ImageFilter

  

def detect_xy_range(pathname):

    im = Image.open(pathname).convert("L")

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

    return x1, x2, y1, y2 




#print detect_xy_range("/home/yake/Desktop/yake37.bmp")





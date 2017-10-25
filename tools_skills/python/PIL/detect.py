#! /usr/bin/python

import os
import sys
from PIL import Image


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



#num = detect_red_pixels("/home/yake/Desktop/yake3.bmp")
#print num





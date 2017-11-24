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



def gen_offset_images(im, path, index, width, higth, offset):
    
    ww,hh = im.size
    
    x = 0
    y = 0

    while y+higth < hh:
        while x+width < ww:
            imt = im.crop((x, y, x+width, y+higth))
            imt.save('%s/%08d.bmp' % (path, index))
            x = x+offset
            index = index + 1
        y = y+offset



    return index




l = get_file_list('.bmp', "./mu1_register")

index = 0;

for i in l:
    im = Image.open(i)  
    index = gen_offset_images(im, "./mu1_r", index, 100, 100, 10)


for i in l:
    im = Image.open(i)  
    im = im.rotate(90)
    index = gen_offset_images(im, "./mu1_r", index, 100, 100, 10)



for i in l:
    im = Image.open(i)  
    im = im.rotate(180)
    index = gen_offset_images(im, "./mu1_r", index, 100, 100, 10)


for i in l:
    im = Image.open(i)  
    im = im.rotate(270)
    index = gen_offset_images(im, "./mu1_r", index, 100, 100, 10)


for i in l:
    im = Image.open(i)  
    im = im.transpose(Image.FLIP_LEFT_RIGHT)  
    index = gen_offset_images(im, "./mu1_r", index, 100, 100, 10)


for i in l:
    im = Image.open(i)  
    im = im.transpose(Image.FLIP_TOP_BOTTOM)  
    index = gen_offset_images(im, "./mu1_r", index, 100, 100, 10)





l = get_file_list('.bmp', "./trs_register")

index = 0;

for i in l:
    im = Image.open(i)  
    index = gen_offset_images(im, "./trs_r", index, 100, 100, 10)


for i in l:
    im = Image.open(i)  
    im = im.rotate(90)
    index = gen_offset_images(im, "./trs_r", index, 100, 100, 10)



for i in l:
    im = Image.open(i)  
    im = im.rotate(180)
    index = gen_offset_images(im, "./trs_r", index, 100, 100, 10)


for i in l:
    im = Image.open(i)  
    im = im.rotate(270)
    index = gen_offset_images(im, "./trs_r", index, 100, 100, 10)


for i in l:
    im = Image.open(i)  
    im = im.transpose(Image.FLIP_LEFT_RIGHT)  
    index = gen_offset_images(im, "./trs_r", index, 100, 100, 10)


for i in l:
    im = Image.open(i)  
    im = im.transpose(Image.FLIP_TOP_BOTTOM)  
    index = gen_offset_images(im, "./trs_r", index, 100, 100, 10)



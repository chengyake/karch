import os
import sys
import numpy as np  
from PIL import Image, ImageFilter





def_width = 128

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



def edges(im):
    k3 = ( 0.0, -2.0,  0.0,
          -2.0,  8.0, -2.0, 
           0.0, -2.0,  0.0)
    imf=im.filter(ImageFilter.Kernel((3,3), k3))
    return imf


def gauss(im):


    k5 = (0.01, 0.02, 0.03, 0.02, 0.01,
          0.02, 0.06, 0.08, 0.06, 0.02,
          0.03, 0.08, 0.11, 0.08, 0.03,
          0.02, 0.06, 0.08, 0.06, 0.02,
          0.01, 0.02, 0.03, 0.02, 0.01)
#    k3 = ( 0.0, -2.0,  0.0,
#          -2.0,  8.0, -2.0, 
#           0.0, -2.0,  0.0)
    imf=im.filter(ImageFilter.Kernel((5,5), k5))
    return imf


def high_only(im, percent=0.2):

    v = np.asarray(im, dtype='uint16')
    (n, bins) = np.histogram(v, bins=256, normed=True)

    sum=0.0
    indx = 255
    while sum < percent:
        sum = sum + n[indx]
        indx = indx - 1
    
    v[v < indx] = 0

    v = v.astype(np.uint8)
    img1=Image.fromarray(v) 

    return img1


def hist(im, percent=0.1):

    v = np.asarray(im, dtype='uint16')
    (n, bins) = np.histogram(v, bins=256, normed=True) 

    sum=0.0
    indx = 255
    while sum < percent:
        sum = sum + n[indx]
        indx = indx - 1
    
    v=v*255/indx
    v[v > 255] = 255


    v = v.astype(np.uint8)
    img1=Image.fromarray(v) 

    return img1


def get_dhash_diff(path1, path2):

    imt = Image.open(path1)  
    (x, y) = imt.size   
    x_1 = def_width
    y_1 = y*def_width/x
    im1 = imt.resize((x_1, y_1), Image.NEAREST)   


    imt = Image.open(path2)  
    (x, y) = imt.size   
    x_2 = def_width
    y_2 = y*def_width/x
    im2 = imt.resize((x_2, y_2), Image.NEAREST)   

    y_y = y_1 if y_1 <= y_2 else y_2


    im1 = im1.crop((0, 0, x_1, y_y))
    im2 = im2.crop((0, 0, x_2, y_y))
    
    im1 = hist(im1)
    im2 = hist(im2)

    im1 = high_only(im1)
    im2 = high_only(im2)



    im1 = gauss(im1)
    im2 = gauss(im2)


    im1 = hist(im1)
    im2 = hist(im2)

    im1 = high_only(im1)
    im2 = high_only(im2)


    y_y = y_y*2/3
    im1 = im1.crop((2, 2, x_1, y_y))
    im2 = im2.crop((2, 2, x_2, y_y))

    #im1.show()
    #im2.show()

    arr1 = np.asarray(im1, dtype='float32')
    arr2 = np.asarray(im2, dtype='float32')
    std = np.std(arr1 - arr2)

    return std/y_y


def get_xy_diff(path1, path2):

    im1 = Image.open(path1)  
    (x1, y1) = im1.size   

    im2 = Image.open(path2)  
    (x2, y2) = im2.size  

    xx = x1 if x1 < x2 else x2
    yy = y1 if y1 < y2 else y2


    im1 = im1.resize((xx, yy), Image.NEAREST)   
    im2 = im2.resize((xx, yy), Image.NEAREST)   

    im1 = hist(im1)
    im2 = hist(im2)

    v1 = np.asarray(im1, dtype='float32')
    v2 = np.asarray(im2, dtype='float32')


    v1x = np.sum(v1, axis=0)
    v2x = np.sum(v2, axis=0)
    
    v1y = np.sum(v1, axis=1)
    v2y = np.sum(v2, axis=1)
    
    
    stdx = np.std(v1x - v2x)/xx
    stdy = np.std(v1y - v2y)/yy




    xy1 = x1*y2
    xy2 = x2*y1

    if xy1 > xy2:
        rate = xy1*1.0/xy2
    else:
        rate = xy2*1.0/xy2
    
    ret = rate*stdx*stdy
    return  ret


def get_diff(path1, path2):
    diff1 = get_xy_diff(path1, path2)
    diff2 = get_dhash_diff(path1, path2)
    diff = diff1*diff2
    return diff



#diff = get_xy_diff("/home/yake/images_register/mu1_crop/00000042.bmp", "/home/yake/images_register/trs_crop/00026189.bmp")
#print diff


diff_min = 10000000000.0
filepath = None

l = get_file_list('.bmp', "/home/yake/images_register/trs_crop")
l.sort()
l.reverse()
for i in l:
    diff = get_diff("/home/yake/images_register/mu1_crop/00000000.bmp", i)
    if diff_min > diff:
        diff_min = diff
        filepath = i
        print diff, i


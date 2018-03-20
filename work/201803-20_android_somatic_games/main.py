import cv2
import numpy as np
import copy
import math

import pyscreenshot as ImageGrab
import time
from pymouse import PyMouse
import math
from PIL import ImageFilter
import random


'''
1. get diff frame  frome background
2. get r and b color
3. calc r and b center
4. filter x and y
5. contral android

'''

# color b g r
r_lower = np.array([0, 0, 135], dtype=np.uint8)
r_upper = np.array([130, 130, 255], dtype=np.uint8)

b_lower = np.array([120, 0, 0], dtype=np.uint8)
b_upper = np.array([255, 120, 120], dtype=np.uint8)




def removeBG(frame):
    fgmask = bgModel.apply(frame)
    # kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
    # res = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)

    kernel = np.ones((3, 3), np.uint8)
    fgmask = cv2.erode(fgmask, kernel, iterations=1)
    res = cv2.bitwise_and(frame, frame, mask=fgmask)
    return res

def calc_center(gray):
    num=0
    xsum=0
    ysum=0
    for x in range(gray.shape[0]):
        for y in range(gray.shape[1]):
            if gray[x][y] != 0:
                xsum+=x
                ysum+=y
                num+=1
    if num == 0:
        return 0, 0
    return xsum/num, ysum/num


# Camera
camera = cv2.VideoCapture(0)
#camera.set(10,200)

#delay
for i in range(100):
    ret, frame = camera.read()
    cv2.imshow('Camera-View',frame)
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break

bgModel = cv2.BackgroundSubtractorMOG2(0, 50)
while camera.isOpened():
    ret, frame = camera.read()
    #frame = cv2.bilateralFilter(frame, 5, 50, 100)  # smoothing filter
    
    frame = cv2.GaussianBlur(frame, (41,41), 0)
    frame = removeBG(frame)
    


    mask = cv2.inRange(frame, r_lower, r_upper)  
    r_frame = cv2.bitwise_and(frame, frame, mask = mask)  
    r_gray = cv2.cvtColor(r_frame, cv2.COLOR_BGR2GRAY)

    mask = cv2.inRange(frame, b_lower, b_upper)  
    b_frame = cv2.bitwise_and(frame, frame, mask = mask)  
    b_gray = cv2.cvtColor(b_frame, cv2.COLOR_BGR2GRAY)
    
    rx,ry = calc_center(r_gray)
    cv2.line(r_gray, (rx, 0), (rx, 470), 255) #5
    cv2.line(r_gray, (0, ry), (470, ry), 255) #5

    cv2.imshow('Camera-View', frame)
    cv2.imshow('r-View', r_gray)
    cv2.imshow('b-View', b_gray)
    cv2.waitKey(10)






'''
adb shell sendevent /dev/input/event0 3 0 110       //x坐标
adb shell sendevent /dev/input/event0 3 1 70         //y坐标
adb shell sendevent /dev/input/event0 1 330 1       //按下状态，准确的说是有压力值
adb shell sendevent /dev/input/event0 0 0 0           //必要的一行数据

adb shell sendevent /dev/input/event0 3 0 101 //step to point (101,200)
adb shell sendevent /dev/input/event0 0 0 0

adb shell sendevent /dev/input/event0 1 330 0       //抬起状态，无压力值
adb shell sendevent /dev/input/event0 0 0 0           //必要的一行，相当于终止一段完整数据的标致
'''

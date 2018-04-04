import cv2
import os
import numpy as np

#import pyscreenshot as ImageGrab
#import time
#from pymouse import PyMouse
#from PIL import ImageFilter
#import random





'''
1. get diff frame  frome background
2. get r and b color
3. calc r and b center
4. filter x and y
5. contral android

lsusb

lsusb -d xxxx: -v


'''

# color b g r
r_lower = np.array([0, 3, 60], dtype=np.uint8)
r_upper = np.array([60, 53, 255], dtype=np.uint8)

b_lower = np.array([0, 40, 0], dtype=np.uint8)
b_upper = np.array([80, 255, 40], dtype=np.uint8)




def removeBG(frame):
    fgmask = bgModel.apply(frame)
    # kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
    # res = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)

    kernel = np.ones((3, 3), np.uint8)
    fgmask = cv2.erode(fgmask, kernel, iterations=1)
    res = cv2.bitwise_and(frame, frame, mask=fgmask)
    return res


def calc_center(gray):
    x = 0
    y = 0
    rnpxy= np.sum(gray)
    
    #if rnpxy < 10*255:
    #    return 0, 0

    rnpx = np.sum(gray, axis=0)
    rnpy = np.sum(gray, axis=1)
    
    linesum=0
    idx=0;
    for i in rnpx:
        if linesum >= rnpxy/2:
            x = idx
            break
        linesum+=i
        idx+=1


    linesum=0
    idx=0;
    for i in rnpy:
        if linesum >= rnpxy/2:
            y = idx
            break
        linesum+=i
        idx+=1

    return x, y



rl=[(0,0),(0,0),(0,0)]
bl=[(0,0),(0,0),(0,0)]
def filter_r(x,y):
    global rl
    rl.append((x,y))
    rl.pop(0)
    nznum = 3-rl.count((0,0))

    if nznum == 0:
        return 0, 0
    
    xsum=0
    ysum=0
    for a,b in rl:
        xsum+=a
        ysum+=b
    
    return xsum/nznum, ysum/nznum


def filter_b(x,y):
    global bl
    bl.append((x,y))
    bl.pop(0)
    nznum = 3-bl.count((0,0))

    if nznum == 0:
        return 0, 0
    
    xsum=0
    ysum=0
    for a,b in bl:
        xsum+=a
        ysum+=b
    
    return xsum/nznum, ysum/nznum


# Camera
camera = cv2.VideoCapture(0)
#camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,800)  
#camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,600)  
#camera.set(10,200)

#delay
for i in range(60):
    ret, frame = camera.read()
    #frame = cv2.GaussianBlur(frame, (11,11), 0)
    frame = cv2.flip(frame, 1)
    cv2.imshow('Camera-View',frame)
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
bgModel = cv2.BackgroundSubtractorMOG2(0, 50)
counter=0
print "------start-------"
while camera.isOpened():
    counter+=1
    print counter
    ret, frame = camera.read()
    #frame = cv2.bilateralFilter(frame, 5, 50, 100)  # smoothing filter
    #frame = cv2.GaussianBlur(frame, (11,11), 0)
    frame = cv2.flip(frame, 1)
    frame = removeBG(frame)
    
    #color mask
    mask = cv2.inRange(frame, r_lower, r_upper)  
    r_frame = cv2.bitwise_and(frame, frame, mask = mask)  
    mask = cv2.inRange(frame, b_lower, b_upper)  
    b_frame = cv2.bitwise_and(frame, frame, mask = mask)  

    #filter R/(B+G) > 2.4
    r_frame[r_frame[:,:,2]/(r_frame[:,:,0]+r_frame[:,:,1]+1.0)<2.4]=(0,0,0)
    b_frame[b_frame[:,:,1]/(b_frame[:,:,0]+b_frame[:,:,2]+1.0)<1.0]=(0,0,0)
    
    #convert BGR to Gray
    r_gray = cv2.cvtColor(r_frame, cv2.COLOR_BGR2GRAY)
    b_gray = cv2.cvtColor(b_frame, cv2.COLOR_BGR2GRAY)

    #calc center
    rx, ry = calc_center(r_gray)
    bx, by = calc_center(b_gray)
    #filter x y
    rx, ry = filter_r(rx, ry)
    bx, by = filter_b(bx, by)
    
    #draw line   
    if rx!=0 or ry!=0:
        cv2.line(frame, (rx, 0), (rx, 480), (0,0,255))
        cv2.line(frame, (0, ry), (640, ry), (0,0,255))
    if bx!=0 or by!=0:
        cv2.line(frame, (bx, 0), (bx, 480), (0,255,0))
        cv2.line(frame, (0, by), (640, by), (0,255,0))
    #show photo
    cv2.imshow('Camera-View', frame)
    cv2.imshow('r-View', r_gray)
    cv2.imshow('b-View', b_gray)
    cv2.waitKey(1)
    



def onPress(x, y):
    return

def onMove(x, y):
    return

def onRelease(x, y):
    return

switch=0
def ontouch(x, y):
    global switch
    if (x,y) != (0, 0):
        if switch==0:
            onPress(x,y)
            switch=1
        else:
            onMove(x,y)
    else:
        if switch!=0:
            onRelease(x, y)
            switch=0
        else:
            pass

    return














'''
adb shell sendevent /dev/input/event0 3 0 110       //x
adb shell sendevent /dev/input/event0 3 1 70         //y
adb shell sendevent /dev/input/event0 1 330 1       //
adb shell sendevent /dev/input/event0 0 0 0           

adb shell sendevent /dev/input/event0 3 0 101 //step to point (101,200)
adb shell sendevent /dev/input/event0 0 0 0

adb shell sendevent /dev/input/event0 1 330 0       //
adb shell sendevent /dev/input/event0 0 0 0           //
'''

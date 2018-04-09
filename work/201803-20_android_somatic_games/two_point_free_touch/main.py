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


# skin color:
# define the upper and lower boundaries of the HSV pixel
# intensities to be considered 'skin'
lower = np.array([0, 48, 80], dtype = "uint8")
upper = np.array([20, 255, 255], dtype = "uint8")


# define the upper and lower boundaries of the HSV pixel
# intensities to be considered 'skin'
lower = np.array([0, 48, 80], dtype = "uint8")
upper = np.array([20, 255, 255], dtype = "uint8")


'''



#red
r_lower = np.array([175, 210, 80], dtype = "uint8")
r_upper = np.array([180, 240, 230], dtype = "uint8")

#green
b_lower = np.array([66, 184, 47], dtype = "uint8")
b_upper = np.array([77, 250, 166], dtype = "uint8")



def removeBG(frame):
    fgmask = bgModel.apply(frame)
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
#bgModel = cv2.BackgroundSubtractorMOG2(10, 50, True) #frames for bg trains; threshold; detectshadow?
bgModel = cv2.BackgroundSubtractorMOG2(20, 50, True)
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
    converted = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(converted, r_lower, r_upper)  
    mask = cv2.GaussianBlur(mask, (3, 3), 0)
    r_frame = cv2.bitwise_and(frame, frame, mask = mask)  
    mask = cv2.inRange(converted, b_lower, b_upper)  
    mask = cv2.GaussianBlur(mask, (3, 3), 0)
    b_frame = cv2.bitwise_and(frame, frame, mask = mask)  

    #filter R/(B+G) > 2.4
    #r_frame[r_frame[:,:,2]/(r_frame[:,:,0]+r_frame[:,:,1]+1.0)<2.4]=(0,0,0)
    #b_frame[b_frame[:,:,1]/(b_frame[:,:,0]+b_frame[:,:,2]+1.0)<1.0]=(0,0,0)
    
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

switch_1=0
def ontouch_r(x, y):
    global switch_1
    if (x,y) != (0, 0):
        if switch_1==0:
            onPress(x,y)
            switch_1=1
        else:
            onMove(x,y)
    else:
        if switch_1!=0:
            onRelease(x, y)
            switch_1=0
        else:
            pass

    return


switch_2=0
def ontouch_b(x, y):
    global switch_2
    if (x,y) != (0, 0):
        if switch_2==0:
            onPress(x,y)
            switch_2=1
        else:
            onMove(x,y)
    else:
        if switch_2!=0:
            onRelease(x, y)
            switch_2=0
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

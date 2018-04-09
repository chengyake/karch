import cv2
import os
import time
import copy
import numpy as np

#import pyscreenshot as ImageGrab
#import time
#from pymouse import PyMouse
#from PIL import ImageFilter
#import random


W=640
H=480

lower = np.array([0, 40, 70], dtype = "uint8")
upper = np.array([30, 255, 255], dtype = "uint8")


class Circle:

    def __init__(self, name, position, r):
        self.name = name
        self.position = position
        self.mask = np.zeros((H, W, 3), dtype="uint8")
        self.r = r
        cv2.circle(self.mask, position, r, (255,255,255), -1)
        self.trigger=False

    def get_mask(self):
        return self.mask

    def is_trigger(self, f):
        frame=copy.deepcopy(f)
        frame[self.mask==0]=0
        frame[frame!=0]=1
        s = np.sum(frame)
        if s > self.r*self.r/2:
            self.trigger=True
        if s < self.r*self.r/3:
            self.trigger=False
        return self.trigger

    def add_button_to_view(self, frame):
        if self.trigger:
            frame[self.mask==255]=255
        else:
            frame[self.mask==255]=0
        return frame


class Button:

    def __init__(self, name, position, r):
        self.name = name
        self.position = position
        self.mask = np.zeros((H, W, 3), dtype="uint8")
        self.r = r
        cv2.circle(self.mask, position, r, (255,255,255), -1)
        self.trigger=False

    def get_mask(self):
        return self.mask

    def is_trigger(self, f):
        frame=copy.deepcopy(f)
        frame[self.mask==0]=0
        frame[frame!=0]=1
        s = np.sum(frame)
        if s > self.r*self.r/2:
            self.trigger=True
        if s < self.r*self.r/3:
            self.trigger=False
        return self.trigger

    def add_button_to_view(self, frame):
        if self.trigger:
            frame[self.mask==255]=255
        else:
            frame[self.mask==255]=0
        return frame

        

class Camera:
    def __init__(self):
        self.frames=0
        self.camera = cv2.VideoCapture(0)
        self.camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,W)  
        self.camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,H)  

    def frame_counter(self):
        self.frames+=1
        print("frames: %d" % self.frames)

    def is_opened(self):
        return self.camera.isOpened()

    def stable_dur(self):
        for i in range(60):
            _, f = self.camera.read()
            f = cv2.flip(f, 1)
            cv2.imshow('Camera-View',f)
            k = cv2.waitKey(30) & 0xff
            if k == 27:
                print("user Esc")
                exit(0)
                break
        

    def set_bg(self):
        self.bgModel = cv2.BackgroundSubtractorMOG2(0, 50)

    def remove_bg(self, frame):
        fgmask = self.bgModel.apply(frame)
        kernel = np.ones((3, 3), np.uint8)
        fgmask = cv2.erode(fgmask, kernel, iterations=1)
        res = cv2.bitwise_and(frame, frame, mask=fgmask)
        return res

    def get_frame(self):
        _, f = self.camera.read()
        f = cv2.flip(f, 1)
        return f




camera = Camera()

if not camera.is_opened():
    print("camera is not opened or unconnected")
    exit(0)

camera.stable_dur()
camera.set_bg()

while camera.is_opened():
    
    camera.frame_counter()
    frame = camera.get_frame()
    org_frame = copy.deepcopy(frame)
    frame = camera.remove_bg(frame)

    #skin mask
    converted = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(converted, lower, upper)  
    #mask = cv2.GaussianBlur(mask, (3, 3), 0)
    frame = cv2.bitwise_and(frame, frame, mask = mask)  


    test = Button("test", (100,100), 10)
    isTrigger = test.is_trigger(frame)
    print(isTrigger)
    org_frame = test.add_button_to_view(org_frame)





    #show photo
    cv2.imshow('Camera-View', org_frame)
    cv2.imshow('Camera-test', frame)
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







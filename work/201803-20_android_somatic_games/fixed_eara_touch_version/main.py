import cv2
import os
import time
import copy
import socket
import math
import numpy as np

#import pyscreenshot as ImageGrab
#import time
#from pymouse import PyMouse
#from PIL import ImageFilter
#import random

'''

            back             gether
                ------------
            _-`    wheel     `-_

           S         A          1
           M                    2
           L                    3


'''







W=640
H=480

lower = np.array([0, 40, 70], dtype = "uint8")
upper = np.array([30, 255, 255], dtype = "uint8")


class Minitouch:
    index=[False,False,False,False,False, 
           False,False,False,False,False]
    def __init__(self):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect(('192.168.1.100', 12345))
        print("Connect success~")

    def get_index(self):
        for i in range(10):
            if self.index[i]==False:
                self.index[i]=True
                return i
        print("Never should be here~ get index")
        return -1
    def release_index(self, i):
        if i >= 10:
            print("Never should be here~ release index")
        else:
            self.index[i]=False


    def down(self,i, (x,y)):
        self.s.send("d %d %d %d 50\n" %(i, x, y))
        #print("d %d %d %d 50\n" %(i, x, y))
        self.s.send("c\n")
        return
    
    def move(self,i, (x,y)):
        self.s.send("m %d %d %d 50\n" %(i, x, y))
        #print("m %d %d %d 50\n" %(i, x, y))
        self.s.send("c\n")
        return
    
    def up(self,i):
        self.s.send("u %d\n" % i)
        #print("u %d\n" % i)
        #self.s.send("c\n")
        return
    

class Button:
    def __init__(self, i, position, r, act_point):
        self.id = i
        self.position = position
        self.r = r
        self.act_point = act_point
        self.mask = np.zeros((H, W, 3), dtype="uint8")
        cv2.circle(self.mask, position, r, (255,255,255), -1)
        self.trigger=False
        self.long_press=False
        self.touch_index=0

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

    def add_button_to_view(self, org_frame):
        if self.trigger:
            org_frame[self.mask==255]=255
        else:
            org_frame[self.mask==255]=0
        return org_frame

    
    def top(self, touch):
        if self.trigger==True and self.long_press==False:
                self.touch_index=touch.get_index()
                touch.down(self.touch_index, self.act_point)
                self.long_press=True

        if self.trigger==False and self.long_press==True:
                touch.release_index(self.touch_index)
                touch.up(self.touch_index)
                self.long_press=False

class Wheel: #bar
    def __init__(self):
        self.id = 0
        self.position = ((140,60),(500,80))
        self.mask = np.zeros((H, W, 3), dtype="uint8")
        cv2.rectangle(self.mask,self.position[0],self.position[1],(255,255,255),-1)
        self.trigger=False
        self.long_press=False
        self.touch_index=0

    def is_trigger(self, f):
        frame=copy.deepcopy(f[50:70, 145:505])
        frame[frame!=0]=1
        s = np.sum(frame)
        if s > 20*360*3/20:
            self.trigger=True
        if s < 20*360*3/22:
            self.trigger=False
        return self.trigger

    def add_button_to_view(self, org_frame):
        if self.trigger:
            org_frame[self.mask==255]=255
        else:
            org_frame[self.mask==255]=0
        return org_frame
   

    def action(self, touch, f):
        if self.trigger==True:
            frame=f[50:70, 145:505]
            asum=np.sum(frame)
            yzsum=np.sum(frame, axis=0)
            ysum = np.sum(yzsum, axis=1)
            half_sum=0
            y=0
            for i in xrange(360):
                half_sum+=ysum[i]
                if half_sum>=asum/2:
                    y=i
                    break
            act_x = 340-120*math.cos(float(y)*2.0/360.0*math.pi) #set act wheel point(x, y)
            act_y = 370-120*math.sin(float(y)*2.0/360.0*math.pi)

            if self.long_press==False:
                self.index=touch.get_index()
                touch.down(self.index, (act_x, act_y))
                self.long_press=True
            else:
                touch.move(self.index, (act_x, act_y))
        
        if self.trigger==False and self.long_press==True:
            touch.up(self.index)
            touch.release_index(self.index)
            self.long_press=False
            #patch for 4 button
            index1=touch.get_index()
            touch.down(index1, (1600-612, 304))
            index2=touch.get_index()
            touch.down(index2, (1600-750, 2144))
            index3=touch.get_index()
            touch.down(index3, (1600-942, 1804))
            index4=touch.get_index()
            touch.down(index4, (1600-1200, 1658))
            touch.up(index1)
            touch.release_index(index1)
            touch.up(index2)
            touch.release_index(index2)
            touch.up(index3)
            touch.release_index(index3)
            touch.up(index4)
            touch.release_index(index4)






class Camera:
    def __init__(self):
        self.frames=0
        self.camera = cv2.VideoCapture(0)
        self.camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,W)  
        self.camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,H)  

    def frame_counter(self):
        self.frames+=1
        #print("frames: %d" % self.frames)

    def is_opened(self):
        return self.camera.isOpened()

    def stable_dur(self):
        for i in range(30):
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



def main():
    touch = Minitouch()
    camera = Camera()
    wheel = Wheel()
    separate_button=[]
    separate_button.append(Button(24, (320,220), 18, (1600-1368,2312))) #button_a 
    separate_button.append(Button(19, (210, 20), 18, (1600- 330,2500))) #button_b   
    separate_button.append(Button(20, (430, 20), 18, (1600- 460,2500))) #button_g 

    separate_button.append(Button(21, (130,160), 18, (1600-1406,1836))) #button_s 
    separate_button.append(Button(22, (144,220), 18, (1600-1098,2012))) #button_m 
    separate_button.append(Button(23, (160,280), 18, (1600- 928,2314))) #button_l 

    separate_button.append(Button(25, (502,160), 18, (1600-1430,1594))) #button_1 
    separate_button.append(Button(26, (496,220), 18, (1600-1430,1366))) #button_2 
    separate_button.append(Button(26, (480,280), 18, (1600-1420,1138))) #button_3 

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
        
        #separate button
        for b in separate_button:
            b.is_trigger(frame)
            b.top(touch)
            b.add_button_to_view(org_frame)

        #arch button ~ wheel
        wheel.is_trigger(frame)
        wheel.action(touch,frame)
        wheel.add_button_to_view(org_frame)
    
        #show photo
        cv2.imshow('Camera-View', org_frame)
        cv2.imshow('Camera-test', frame)
        cv2.waitKey(1)
    


main()








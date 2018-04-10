import cv2
import os
import time
import copy
import socket       
import numpy as np



'''
      ok
 A          B

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
        print("d %d %d %d 50\n" %(i, x, y))
        self.s.send("c\n")
        return
    
    def move(self,i, (x,y)):
        self.s.send("m %d %d %d 50\n" %(i, x, y))
        print("m %d %d %d 50\n" %(i, x, y))
        self.s.send("c\n")
        return
    
    def up(self,i):
        self.s.send("u %d\n" % i)
        #print("u %d\n" % i)
        self.s.send("c\n")
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
    separate_button=[]
    separate_button.append(Button(19, (320, 40 ), 25, (500, 1500))) #button_ok 
    separate_button.append(Button(20, (130, 220), 25, (100, 100))) #button_A
    separate_button.append(Button(21, (502, 220), 25, (100,2400))) #button_B 

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
        
        for b in separate_button:
            b.is_trigger(frame)
            b.top(touch)
            b.add_button_to_view(org_frame)

    
        #show photo
        cv2.imshow('Camera-View', org_frame)
        cv2.imshow('Camera-test', frame)
        cv2.waitKey(1)
    


main()









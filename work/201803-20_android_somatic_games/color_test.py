import cv2
import os
import time
import numpy as np



# color b g r
r_lower = np.array([10, 3, 140], dtype=np.uint8)
r_upper = np.array([60, 30, 240], dtype=np.uint8)

b_lower = np.array([10, 50, 0], dtype=np.uint8)
b_upper = np.array([56, 130, 18], dtype=np.uint8)



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

frame = cv2.imread("my_photo-4.jpg")
frame = cv2.GaussianBlur(frame, (5,5), 0)
frame = cv2.flip(frame, 1)

mask = cv2.inRange(frame, r_lower, r_upper)  
r_frame = cv2.bitwise_and(frame, frame, mask = mask)  
r_gray = cv2.cvtColor(r_frame, cv2.COLOR_BGR2GRAY)

mask = cv2.inRange(frame, b_lower, b_upper)  
b_frame = cv2.bitwise_and(frame, frame, mask = mask)  
b_gray = cv2.cvtColor(b_frame, cv2.COLOR_BGR2GRAY)

rx, ry = calc_center(r_gray)
bx, by = calc_center(b_gray)
rx, ry = filter_r(rx, ry)
bx, by = filter_b(bx, by)

cv2.line(frame, (rx, 0), (rx, 480), (0,0,255)) 
cv2.line(frame, (0, ry), (640, ry), (0,0,255)) 
cv2.line(frame, (bx, 0), (bx, 480), (0,255,0)) 
cv2.line(frame, (0, by), (640, by), (0,255,0)) 

cv2.imshow('Camera-View', frame)
cv2.imshow('r-View', r_gray)
cv2.imshow('b-View', b_gray)
cv2.waitKey(0)

#time.sleep(100)
    







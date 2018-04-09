import cv2
import os
import time
import numpy as np



# color b g r
r_lower = np.array([0, 3, 60], dtype=np.uint8)
r_upper = np.array([60, 53, 255], dtype=np.uint8)

b_lower = np.array([0, 40, 0], dtype=np.uint8)
b_upper = np.array([80, 255, 40], dtype=np.uint8)




# Camera
camera = cv2.VideoCapture(0)
#camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,800)  
#camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,600)  
print "-------------"
#camera.set(10,200)

#delay
for _ in range(1690):
    print _
    #ret, frame = camera.read()
    frame = cv2.imread("my_photo-4.jpg")
    #frame = cv2.GaussianBlur(frame, (5,5), 0)
    frame = cv2.flip(frame, 1)


    cv2.imshow('Camera-view', frame)

    mask = cv2.inRange(frame, b_lower, b_upper)  
    frame = cv2.bitwise_and(frame, frame, mask = mask)  



    
    frame[frame[:,:,1]/(frame[:,:,2]+frame[:,:,0]+1.0)<1.0]=(0,0,0)






    cv2.imshow('Camera-View', frame)
    cv2.waitKey(20)

    #time.sleep(100)
    







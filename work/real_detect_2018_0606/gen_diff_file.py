import numpy as np
import cv2

lab = np.fromfile("./lab.raw", dtype='float32')
print lab.shape
lab.shape=64,64,4

img = cv2.imread("./img.png")
hh,ww,_ = img.shape
rate = lab[:,:,:2]/np.expand_dims(np.sum(lab[:,:,:2], axis=-1), axis=-1)
ij = np.where(rate[2:-2,2:-2,:2]==np.max(rate[2:-2,2:-2,:2]))
i=ij[0][0]+2
j=ij[1][0]+2
index=ij[2][0]

if np.sum(lab[i,j,:2]) >=0.1:
    w = int(lab[i,j,2]*ww) 
    h = int(lab[i,j,3]*hh)
    print(j,i,w,h,index)
    percent = rate[i,j,index]
    xmin = int((j*4.0+2)/255*ww) - w
    ymin = int((i*4.0+2)/255*hh) - h
    xmax = int((j*4.0+2)/255*ww) + w
    ymax = int((i*4.0+2)/255*hh) + h

    cv2.rectangle(img, (xmin, ymin), (xmax, ymax), (0,255,0), 1)
    cv2.putText(img,"%d:%0.3f" % (index, percent), (int((xmin+xmax)/2), int((ymin+ymax)/2)),cv2.FONT_HERSHEY_COMPLEX_SMALL,0.8,(0,255,0), 1)
    cv2.imwrite('gen_diff.png', img)
























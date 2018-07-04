import numpy as np
import cv2

lab = np.fromfile("./output/Result_14/sigmoid:0.raw", dtype='float32')
print lab.shape
lab.shape=287,287,4
HW=287
img = cv2.imread("./images/jdms_20130107_050827.png")
hh,ww,_ = img.shape
rate = lab[:,:,:2]/np.expand_dims(np.sum(lab[:,:,:2], axis=-1), axis=-1)
ij = np.where(rate[:,:,:2]==np.max(rate[:,:,:2]))
i=ij[0][0]
j=ij[1][0]
index=ij[2][0]

if np.sum(lab[i,j,:2]) >=0.9:
    w = int(lab[i,j,2]*ww) 
    h = int(lab[i,j,3]*hh)
    print(j,i,w,h,index)
    percent = rate[i,j,index]
    cv2.rectangle(img, (j-w, i-h), (j+w, i+h), (0,255,0), 1)
    cv2.putText(img,"%d:%0.3f" % (index, percent), (j,i),cv2.FONT_HERSHEY_COMPLEX_SMALL,0.8,(0,255,0), 1)
    cv2.imwrite('gen_diff.png', img)
























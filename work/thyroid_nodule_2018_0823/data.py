from __future__ import division
import os
import cv2
import copy
import math
import random
import numpy as np

HW=256
split=0.9
MATH = cv2.INTER_AREA

class DATA:

    train_num = 0
    val_num = 0
    def __init__(self):
        self.get_data()
        return

    def get_data(self):
        self.dat = []
        self.lab = []
        dlist = os.listdir("./data")
        for i in dlist:
            image = cv2.imread("./data/"+i, cv2.IMREAD_GRAYSCALE)[5:-35, 30:-30]
            label = cv2.imread("./label/"+i, cv2.IMREAD_GRAYSCALE)[5:-35, 30:-30]

            h,w = image.shape
            if h > w:
                s = int((h-w)/2)
                raw1 = np.zeros((h,h))
                raw2 = np.zeros((h,h))
                raw1[:,s:s+h] = image
                raw2[:,s:s+h] = label
            else:
                s = int((w-h)/2)
                raw1 = np.zeros((w,w))
                raw2 = np.zeros((w,w))
                raw1[s:s+h,:] = image
                raw2[s:s+h,:] = label

            raw1 = cv2.resize(raw1, (HW, HW), interpolation=MATH)
            raw2 = cv2.resize(raw2, (HW, HW), interpolation=MATH)
            raw2[raw2>32]=255

            self.dat.append(raw1)
            self.lab.append(raw2)


        self.train_num = int(len(self.dat)*split)
        self.val_num = int(len(self.dat) - len(self.dat)*split)
        print("DATA:init thyroid dataset success")
        print("DATA:train", self.train_num)
        print("DATA:test ", self.val_num)
        self.dat = np.array(self.dat)
        self.dat = np.expand_dims(self.dat, axis=-1)
        self.lab = np.array(self.lab)
        self.lab = np.expand_dims(self.lab, axis=-1)
        return

    def get_train_batch(self, size):
        fm1 = random.randint(0, 2)
        

        mvf = random.randint(0, 5)
        mv1 = random.randint(1, 5)
        mv2 = random.randint(1, 5)
        mv1_x = mv1
        mv2_x = mv2
      
        choice = np.random.randint(0, self.train_num, size)
        x = self.dat[choice]
        y = self.lab[choice]
        #mirror
        if fm1 == 1:
            x=copy.deepcopy(x[:,:,::-1,:])
            y=copy.deepcopy(y[:,:,::-1,:])
        
        #shift
        x_=np.zeros_like(x)
        y_=np.zeros_like(y)
     

        if mvf == 1:
            x_[:,mv1_x:,mv2_x:,:]=x[:,:-mv1_x,:-mv2_x,:]
            y_[:,mv1:,mv2:,:]=y[:,:-mv1,:-mv2,:]

        elif mvf == 2:
            x_[:,:-mv1_x,mv2_x:,:]=x[:,mv1_x:,:-mv2_x,:]
            y_[:,:-mv1,mv2:,:]=y[:,mv1:,:-mv2,:]

        elif mvf == 3:
            x_[:,mv1_x:,:-mv2_x,:]=x[:,:-mv1_x,mv2_x:,:]
            y_[:,mv1:,:-mv2,:]=y[:,:-mv1,mv2:,:]

        elif mvf == 4:
            x_[:,:-mv1_x,:-mv2_x,:]=x[:,mv1_x:,mv2_x:,:]
            y_[:,:-mv1,:-mv2,:]=y[:,mv1:,mv2:,:]
        else:
            x_ = copy.deepcopy(x)
            y_ = copy.deepcopy(y)

        #color
        #col = np.random.random((3,))/2.0+0.75
        #x_=x_*col
        #x_[x_>255.0]=255.0

        return x_,y_

    def get_val_batch(self, size=24):
        choice = np.random.randint(self.train_num, self.train_num+self.val_num, size)
        x = self.dat[choice]
        y = self.lab[choice]
        return x, y





if __name__ == '__main__':
    data = DATA()

    img, lab = data.get_train_batch(10)
    print(img.shape, lab.shape)
    image = img[0]+lab[0]
    image[image>255]=255

    cv2.imwrite("data_val.png", image) 












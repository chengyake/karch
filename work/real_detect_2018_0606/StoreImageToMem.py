import tensorflow as tf
import os
import cv2
import math
import random
import numpy as np
import xml.etree.ElementTree as et


train_val = 0.95

HW=240


class Memimages:

    train_list=[]
    val_list=[]
    loss_table = np.zeros((121,121))

    classes = ["aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable", "dog", "horse", "motorbike", "person", "pottedplant", "sheep", "sofa", "train", "tvmonitor"]

    def __init__(self, img_dir="/home/yake/artery_yake/VOCdevkit/VOC2007/JPEGImages", xml_dir="/home/yake/artery_yake/VOCdevkit/VOC2007/Annotations", outdir="./save/"):
    
        self.init_loss_table()
        if os.path.exists(outdir+"train.npz"):
            data=np.load(outdir+"train.npz")
            self.train_list=data['train']
            self.val_list=data['val']
            print("train.npz is already")
            return

        img_list = os.listdir(img_dir)
        img_list.sort()
        xml_list = os.listdir(xml_dir)
        xml_list.sort()
        
        index = int(len(img_list)*train_val)

        for i in range(len(img_list)):
        #for i in range(1000):
            ivl=[]
            vnp=np.zeros((60,60,22))
            img_np=cv2.imread(img_dir+"/"+img_list[i])
            h,w,c=img_np.shape
            img_np=cv2.resize(img_np, (HW, HW))
            img_np=img_np.astype("float32")/255.0
            hr=h*1.0/60
            wr=w*1.0/60
            ivl.append(img_np)

            xml = open(xml_dir+"/"+xml_list[i])
            tree = et.parse(xml)
            root = tree.getroot()
            
            for obj in root.iter("object"):
                name = obj.find("name").text
                class_id = self.classes.index(name)
                box = obj.find("bndbox")
                xmin = int(box.find("xmin").text)
                ymin = int(box.find("ymin").text)
                xmax = int(box.find("xmax").text)
                ymax = int(box.find("ymax").text)

                center_y = int((ymin+ymax)/2.0/hr)
                center_x = int((xmin+xmax)/2.0/wr)
                #if center_y > 0 and center_y < 59 and center_x > 0 and center_x < 59:
                tmp = self.loss_table[60-center_y:120-center_y, 60-center_x:120-center_x]
                vnp[:,:,class_id] = np.where(tmp>vnp[:,:,class_id], tmp, vnp[:,:,class_id])
                #else:
                #    vnp[center_y+0, center_x+0,class_id]=1.0
                vnp[center_y, center_x,20]=(ymax-ymin)/2.0/h
                vnp[center_y, center_x,21]=(xmax-xmin)/2.0/w

            ivl.append(vnp)
            if i < index:
                self.train_list.append(ivl)
            else:
                self.val_list.append(ivl)

        #np.savez(outdir+"train.npz", train=self.train_list, val=self.val_list)
        print("make train.npz success")
        print("train:%d  val:%d" % self.get_sum())





    def init_loss_table(self):
        dis = math.pow(60, 2)
        for i in range(121):
            for j in range(121):
                per = 1-(math.pow(60-i, 2) + math.pow(60-j, 2))/2.0/dis
                self.loss_table[i][j]=math.pow(per, 40)
        

    def get_batch(self, size):
        rdata = random.sample(list(self.train_list), size)
        x=np.array([i[0] for i in rdata])
        y=np.array([i[1] for i in rdata])
        r=np.random.randint(4)
        x_ = x[:, :, :, :20]
        y_ = y[:, :, :, :20]

        if r==1:
            x_=x_[:,::-1,:,:]
            y_=y_[:,::-1,:,:]
        elif r==2:
            x_=x_[:,:,::-1,:]
            y_=y_[:,:,::-1,:]
        elif r==3:
            x_=x_[:,::-1,::-1,:]
            y_=y_[:,::-1,::-1,:]

        x[:,:,:,:20] = x_
        y[:,:,:,:20] = y_

        return x,y

    def get_val(self, size):
        rdata = random.sample(list(self.val_list), size)
        return [i[0] for i in rdata], [i[1] for i in rdata]

    def get_train_one(self):
        return random.choice(self.train_list)

    def get_val_one(self):
        return random.choice(self.val_list)

    def get_sum(self):
        return len(self.train_list), len(self.val_list)




if __name__ == '__main__':
    test = Memimages()
    print("train:%d  val:%d" % test.get_sum())
#check train datasets
    test_mode=3
    if test_mode==2:
        img,lab = test.get_batch(64)
        img=img[0].copy()
        img=img*255
        for i in range(60):
            for j in range(60):
                if np.sum(lab[0,i,j,:20])>=1.0:
                    h = lab[0,i,j,20]*240
                    w = lab[0,i,j,21]*240
                    index = np.where(lab[0,i,j,:20]==np.max(lab[0,i,j,:20]))
                    cv2.rectangle(img, (int(j*4-w), int(i*4-h)), (int(j*4+w), int(i*4+h)), (0,255,0), 1)
                    cv2.putText(img,test.classes[index[0][0]], (int(j*4-w), int(i*4-h)), cv2.FONT_HERSHEY_COMPLEX_SMALL, 0.8,(0,255,0), 1)
        cv2.imwrite("batch_0.png", img)


    if test_mode==3:
        img,lab = test.get_batch(64)
        img=img[0].copy()
        img=img*255
        cv2.imwrite("batch_0.png", img)
        for i in range(20):
            cv2.imwrite("batch_%s.png" % test.classes[i], (lab[0,:,:,i]*255).astype("uint8"))

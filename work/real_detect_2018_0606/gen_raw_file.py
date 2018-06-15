#source /opt/snpe-1.15.0/bin/envsetup.sh -t /usr/local/lib/python2.7/dist-packages/tensorflow
#python /usr/local/lib/python2.7/dist-packages/tensorflow/python/tools/optimize_for_inference.py --input $1 --output tmp_opt.pb --input_names reshape --output_names sigmoid
#/opt/snpe-1.15.0/bin/x86_64-linux-clang/snpe-tensorflow-to-dlc --graph tmp_opt.pb --input_dim reshape 240,240,3 --out_node sigmoid  --dlc $1.dlc  --allow_unconsumed_nodes


import os
import cv2
import numpy as np

HW=287

images = os.listdir("./images")
for i in images:
    raw = np.ones((HW,HW,3))*128
    im = cv2.imread("./images/"+i)
    h,w,c = im.shape
    if h < w:
        rate = float(HW)/w
    else:
        rate = float(HW)/h

    new_h = int(rate*h/2)*2
    new_w = int(rate*w/2)*2
    dst_img = cv2.resize(im, (new_w, new_h))

    raw[int(HW/2-new_h/2) : int(HW/2+new_h/2), int(HW/2-new_w/2) : int(HW/2+new_w/2)] = dst_img
    
    #cv2.imwrite("test_"+i, raw)
    raw=(raw.astype("float32"))/255.0
    raw.tofile(i+".raw")







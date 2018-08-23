import os
import cv2
import json
import numpy as np
import xml.etree.ElementTree as ET





for i in range(1000):
    print(i)
    fname = "thyroid/%d.xml"%i
    if os.path.exists(fname):
        tree = ET.parse(fname)
        root = tree.getroot()
        for mark in root.iter("mark"):
            if mark[1].text is not None:
                jpg = "thyroid/%d_%s.jpg"%(i,mark[0].text)
                if os.path.exists(jpg):
                    jpg_raw = cv2.imread(jpg)
                    h,w,c = jpg_raw.shape
                    if c > 1:
                        gray = cv2.cvtColor(jpg_raw, cv2.COLOR_BGR2GRAY)
                    else:
                        gray = jpg_raw

                    lab_raw = np.zeros_like(gray)
                    reg_json = json.loads(mark[1].text)
                    for j in range(len(reg_json)):
                        if reg_json[j]['points'] is not None:
                            pre_point = reg_json[j]['points'][0]
                            for points in reg_json[j]['points'][1:]:
                                cv2.line(lab_raw, (points['x'], points['y']), (pre_point['x'], pre_point['y']), 255, 1)
                                pre_point = points
                    cv2.imwrite("label/%d_%s.jpg"%(i,mark[0].text), lab_raw)
                    cv2.imwrite("data/%d_%s.jpg"%(i,mark[0].text), gray)



# -*- coding: utf-8 -*-


#http://blog.csdn.net/icamera0/article/details/50708888
#ƽ��,��,���ص�,���������þ���˲���

import Image

from PIL import ImageFilter

# ��һ��jpgͼ���ļ���ע��·��Ҫ�ĳ����Լ���:
im = Image.open('./bus.jpg')

im2 = im.filter(ImageFilter.BLUR)
# �����ź��ͼ����jpeg��ʽ����:
im2.save('./bus_blurry.jpg', 'jpeg')




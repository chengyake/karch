# -*- coding: utf-8 -*-


#http://blog.csdn.net/icamera0/article/details/50708888
#平滑,锐化,边沿等,甚至可以用卷积核操作

import Image

from PIL import ImageFilter

# 打开一个jpg图像文件，注意路径要改成你自己的:
im = Image.open('./bus.jpg')

im2 = im.filter(ImageFilter.BLUR)
# 把缩放后的图像用jpeg格式保存:
im2.save('./bus_blurry.jpg', 'jpeg')




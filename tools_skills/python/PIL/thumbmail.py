# -*- coding: utf-8 -*-

import Image

# ��һ��jpgͼ���ļ���ע��·��Ҫ�ĳ����Լ���:
im = Image.open('./bus.jpg')
# ���ͼ��ߴ�:
w, h = im.size
# ���ŵ�50%:
im.thumbnail((w//2, h//2))
# �����ź��ͼ����jpeg��ʽ����:
im.save('./bus_thumbnail.jpg', 'jpeg')




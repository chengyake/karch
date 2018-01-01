import pyscreenshot as ImageGrab
import time
from pymouse import PyMouse
import math
from PIL import ImageFilter
import random

def press_left(t=1.0) :
    m = PyMouse()

    xt=random.randint(1400, 1600)
    yt=random.randint(260, 500)

    m.position()
    m.move(xt, yt)
    time.sleep(0.2)
    m.press(xt, yt)
    time.sleep(t)
    m.release(xt, yt)

def move_down():
    m = PyMouse()
    xt=random.randint(1400, 1600)
    yt=random.randint(700, 800)

    m.position()
    m.move(xt, yt)



def get_target_point_yl(im, vo):
    yflag=0
    sx,sy=im.size
    im= im.filter(ImageFilter.GaussianBlur)
    for y in range(3,sy):
        for x in range(3,sx):
            if x > vo-25 and x < vo+25:#void user self
                continue
            r1,g1,b1=im.getpixel((x,y))
            r2,g2,b2=im.getpixel((x-1,y))
            s = pow((r1-r2),2)+pow((g1-g2),2)+pow((b1-b2),2)
            if s>16:
                if yflag>3:
                    return x,y
                else:
                    yflag+=1

    print("No target found left")
    im.show()

def get_target_point_yr(im, vo):
    yflag=0
    sx,sy=im.size
    im= im.filter(ImageFilter.GaussianBlur)
    for y in range(3,sy):
        for x in list(reversed(range(0,sx-3))):
            if x > vo-25 and x < vo+25:#void user self
                continue
            r1,g1,b1=im.getpixel((x,y))
            r2,g2,b2=im.getpixel((x+1,y))
            s = pow((r1-r2),2)+pow((g1-g2),2)+pow((b1-b2),2)
            if s>16:
                if yflag>3:
                    return x,y
                else:
                    yflag+=1
    print("No target found right")
    im.show()



def get_start_point(im):
    num=0;
    sx,sy=im.size
    for y in range(sy):
        for x in range(sx):
            r,g,b=im.getpixel((x,y))
            if r>60 and r<70 and g>50 and g<70 and b>80 and b<100:
                num+=1
            if num > 4:
                return x,y+92
    print("No boy found")
    im.show()

def get_start_point2(im):
    num=0;
    sx,sy=im.size
    for y in list(reversed(range(0,sy))):
        for x in range(sx):
            r,g,b=im.getpixel((x,y))
            if r>60 and r<70 and g>50 and g<70 and b>80 and b<100:
                #num+=1
                #if num > 4:
                return x,y-5
    print("No boy found")
    im.show()



def draw_x_line(im, px, py, v=0):
    sx,sy=im.size
    for y in range(sy):
        im.putpixel((px,y),(v,v,v))
    for x in range(sx):
        im.putpixel((x,py),(v,v,v))
    return im




for j in range(1000):
    im = ImageGrab.grab()
    cropimg=im.crop((1350,260,1865,900))
    x1,y1=get_start_point(cropimg)
    x2,y2=get_target_point_yl(cropimg, x1)
    x3,y3=get_target_point_yr(cropimg, x1)
    
    xt=(x2+x3)/2

    if j < 7:
        yt=(y2+y3)/2+45
    elif j < 60:
        yt=(y2+y3)/2+32
    else:
        yt=(y2+y3)/2+15


    dis2=pow((x1-xt), 2) + pow((y1-yt), 2)
    dis=math.sqrt(dis2)
    t=dis/370

    print j,dis,t

    
    press_left(t)
    
    cropimg=draw_x_line(cropimg, x1, y1, 0)
    cropimg=draw_x_line(cropimg, x2, y2, 255)
    cropimg=draw_x_line(cropimg, x3, y3, 255)
    cropimg.save("/home/chengyake/save/%d.png" % j)
    #cropimg.show()
    move_down()
    time.sleep(2.0+random.uniform(1, 2))
    







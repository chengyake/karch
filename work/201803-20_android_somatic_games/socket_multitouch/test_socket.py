#!/usr/bin/python
import sys
import time
import socket       

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('192.168.1.100', 12345))

def touchDown(i, x,y):
    if i == 0:
        s.send("r\n")
        time.sleep(0.1)

    print("d %d %d %d 50\n" % (i, x, y))
    s.send("d %d %d %d 50\n" %(i, x, y))
    s.send("c\n")
    return

def touchMove(i, x,y):
    print("m %d %d %d 50\n" %(i, x, y))
    s.send("m %d %d %d 50\n" %(i, x, y))
    s.send("c\n")
    return

def touchUp(i):
    print("u %d\n" % i)
    s.send("u %d\n" % i)
    s.send("c\n")
    return





print("connect success~")
time.sleep(0.3)
#print(s.recv(30))
#print(s.recv(160))
#print(s.recv(60))



for z in range(100):
    

    name = input("Test num:")
    

    touchDown(0, 200,200)
    touchDown(1, 600,400)
    for i in range(201, 500, 10):
        time.sleep(0.01)
        touchMove(0, i, i)
        touchMove(1, i+400,i+200)
    time.sleep(0.01)
    touchUp(0)
    time.sleep(0.1)
    touchUp(1)




s.close() 







#!/usr/bin/python
import sys
import socket       

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect(('192.168.50.118', 12345))
print("connect success~")
print(s.recv(1024))
print(s.recv(1024))
print(s.recv(1024))


touchDown(200,200)
for i in range(200, 500, 5):
    touchMove(i,i)
touchUp(500,500)

s.close() 







def touchDown(x,y):
    s.send("d 0 %d %d 50\n" % x, y)
    return

def touchMove(x,y):
    s.send("m 0 %d %d 50\n" % x, y)
    return

def touchUp(x,y):
    s.send("u 0\n")
    return

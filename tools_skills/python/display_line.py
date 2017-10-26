#! /usr/bin/python
# -*- coding: utf-8 -*-  
import numpy as np  
import matplotlib.pyplot as plt  
 

def display_line(dlist, strs="Display line"):

    l = len(dlist)
    minv = min(dlist)
    maxv = max(dlist)
    x = np.linspace(0, l, l)  
    y = np.array(dlist)

    plt.figure(figsize=(8,4))  
    plt.plot(x,y,color="red",linewidth=2)  
    plt.xlabel("Y")  
    plt.ylabel("X")  
    plt.title("Display line")  
    plt.ylim(minv, maxv)  
    plt.legend()  
    plt.show()  

    return


  
  
  


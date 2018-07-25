#! /usr/bin/python3
import sys
import random
import numpy as np

#Target function
#1*sin(2)+3*sin(x/4)+5*sin(x^2/6)+7*sin(x^3/8)

class FC:
    l1=[1,2,3,4,5,6,7,8]
    def __init__(self):

        self.layer_num = 1
        self.layer_nodes = 8


        self.x = np.array(range(-50, 51))
        self.x=self.x.astype("float")
        self.x/=10
        self.y = self.check_function()
        return

    def target_function(self, x):
        return 1*np.sin(2)+3*np.sin(x/4)+5*np.sin(np.power(x,2)/6)+7*np.sin(np.power(x,3)/8)
        
    def check_function(self):
        data = self.target_function(self.x)
        return data

    def train(self, genes):
        args=[]
        for i in range(self.layer_nodes):
            args.append(1)
            for j in range(self.layer_nodes):
                args[i]+=genes[0][i][j]*pow(2, j)
        _y = args[0]*np.sin(args[1]) + args[2]*np.sin(self.x/args[3]) + args[4]*np.sin(np.power(self.x, 2)/args[5]) + args[6]*np.sin(np.power(self.x,3)/args[7])

        if np.mean(np.abs(self.y-_y)) == 0:
            print("vvvvvvvvvvvvvvvvvvvvvvv")
            exit()

        if 1.0/np.mean(np.abs(self.y-_y)) > 8:
            print(args)
            exit()

        return 1.0/np.mean(np.abs(self.y-_y))
        

if __name__ == '__main__':

    dl = FC()
    r = dl.train(np.random.randint(2,size=(dl.layer_num, dl.layer_nodes, dl.layer_nodes)))
    print(r)


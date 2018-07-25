import numpy as np
import random


class DATA:
    
    num_input=2
    num_class=1

    def __init__(self):
        print("Train Dataset:\t(100000,2)\t(100000,1)")
    
    def target_function(self, x):
        
        #return np.power(x[:,0]-0.5, 2)+np.power(x[:,1]-0.5, 2)
        return np.log(10*np.cos(3*np.sin(np.power(20*(x[:,0]-0.5), 3)/100))+10.)+0.3*20*(x[:,0]-0.5) + np.log(10*np.cos(3*np.sin(np.power(20*(x[:,1]-0.5), 3)/100))+10.)+0.3*20*(x[:,1]-0.5)


    def get_train_batch(self, size):
        x = np.random.random((size,2))
        y = self.target_function(x)
        return x, y

    def get_score_batch(self, size=1000):
        x = np.random.random((size,2))
        y = self.target_function(x)
        return x, y

    def get_val_batch(self, size=5000):
        x = np.random.random((size,2))
        y = self.target_function(x)
        return x, y


if __name__ == '__main__':
    
    data = DATA()
    x, y = data.get_train_batch(100000)
    print(x.shape, y.shape)
    x, y = data.get_score_batch()
    print(x.shape, y.shape)
    x, y = data.get_val_batch()
    print(x.shape, y.shape)

    print("x=[")
    for i in range(1000):
        print(x[i][0],",")
    print("]")

    print("y=[")
    for i in range(1000):
        print(x[i][1],",")
    print("]")

    print("z=[")
    for i in range(1000):
        print(y[i],",")
    print("]")



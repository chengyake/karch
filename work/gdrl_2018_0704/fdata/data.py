import numpy as np
import random
from tensorflow.examples.tutorials.mnist import input_data


class DATA:
    
    num_input=1
    num_class=1

    def __init__(self):
        print("Train Dataset:\t(100000,)\t(100000,)")
    
    def target_function(self, x):#input 0-1
        return np.log(10*np.cos(3*np.sin(np.power(20*(x-0.5), 3)/100))+10.)+0.3*20*(x-0.5)

        


    def get_train_batch(self, size):
        x = np.random.random(size)
        x.shape=size,1
        return x, self.target_function(x)

    def get_score_batch(self, size=1000):
        x = np.random.random(size)
        x.shape=size,1
        return x, self.target_function(x)

    def get_val_batch(self, size=5000):
        x = np.random.random(size)
        x.shape=size,1
        return x, self.target_function(x)


if __name__ == '__main__':
    
    data = DATA()
    x, y = data.get_train_batch(100000)
    print(x.shape, y.shape)
    x, y = data.get_score_batch()
    print(x.shape, y.shape)
    x, y = data.get_val_batch()
    print(x.shape, y.shape)

    for i in range(len(x)):
        print(x[i], y[i])
